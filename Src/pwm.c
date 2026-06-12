//
// Created by Kok on 6/9/26.
//

#include "pwm.h"

#include "FreeRTOS.h"
#include "task.h"
#include "app_state.h"
#include "nvic_defs.h"
#include "svhal_evt_bits_def.h"
#include "tasks_common.h"

#define TIM_CLK_MHZ                         16
#define INITIAL_PWM_FREQ_HZ                 10000

#define DEAD_TIME_TICKS                     6       // ~ 375ns

volatile uint32_t input_capture_values[2] = {0};

uint16_t calc_pwm_period(uint32_t freq_hz);
uint32_t find_resonance_freq();

void pwm_task(void *arg);
void resonance_task(void *arg);

AppErrorTypeDef PWM_Init() {
    uint16_t initial_period = calc_pwm_period(INITIAL_PWM_FREQ_HZ);
    gAppState.htim2 = (TIM_HandleTypeDef){
        .Instance = TIM2,
        .Init = {
            .AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE,
            .ClockDivision = TIM_CLOCKDIVISION_DIV1,
            // .CounterMode = TIM_COUNTERMODE_CENTERALIGNED1,
            .CounterMode = TIM_COUNTERMODE_UP,
            .Prescaler = 0,                // 16 MHz
            .Period = initial_period - 1
        }
    };

    HAL_StatusTypeDef hal_err;
    if ((hal_err = HAL_TIM_PWM_Init(&gAppState.htim2)) != HAL_OK) {
        return ERROR_PWM_TIM_INIT;
    }

    // Configure input capture for resonance frequency detection
    TIM_IC_InitTypeDef ic_config = {
        .ICFilter = 0x00,
        .ICPolarity = TIM_ICPOLARITY_FALLING,
        .ICPrescaler = TIM_ICPSC_DIV1,
        .ICSelection = TIM_ICSELECTION_DIRECTTI
    };
    if ((hal_err = HAL_TIM_IC_ConfigChannel(&gAppState.htim2, &ic_config, TIM_CHANNEL_1)) != HAL_OK) {
        return ERROR_IC_TIM_CONF;
    }

    TIM_OC_InitTypeDef oc_config = {
        .OCMode = TIM_OCMODE_PWM1,
        .OCFastMode = TIM_OCFAST_ENABLE,
        .OCPolarity = TIM_OCPOLARITY_HIGH,
        .Pulse = (initial_period / 2) - DEAD_TIME_TICKS
    };
    if ((hal_err = HAL_TIM_PWM_ConfigChannel(&gAppState.htim2, &oc_config, TIM_CHANNEL_3)) != HAL_OK) {
        return ERROR_PWM_TIM_OC_CONF;
    };

    // Get inverted signal
    oc_config.OCMode = TIM_OCMODE_PWM2;
    oc_config.Pulse = (initial_period / 2) + DEAD_TIME_TICKS;

    if ((hal_err = HAL_TIM_PWM_ConfigChannel(&gAppState.htim2, &oc_config, TIM_CHANNEL_4)) != HAL_OK) {
        return ERROR_PWM_TIM_OC_CONF;
    };

    // Configure DMA to store captured value without CPU
    gAppState.hdma1ch5 = (DMA_HandleTypeDef){
        .Instance = DMA1_Channel5,
        .Init = {
            .Mode = DMA_NORMAL,
            .Direction = DMA_PERIPH_TO_MEMORY,
            .MemInc = DMA_MINC_ENABLE,
            .MemDataAlignment = DMA_MDATAALIGN_HALFWORD,
            .PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD,
            .PeriphInc = DMA_PINC_DISABLE,
            .Priority = DMA_PRIORITY_VERY_HIGH,
            .Request = DMA_REQUEST_5
        }
    };

    if ((hal_err = HAL_DMA_Init(&gAppState.hdma1ch5)) != HAL_OK) {
        return ERROR_IC_TIM_DMA_INIT;
    }

    if ((hal_err = HAL_TIM_PWM_Start(&gAppState.htim2, TIM_CHANNEL_3)) != HAL_OK) {
        return ERROR_PWM_TIM_START;
    }

    if ((hal_err = HAL_TIM_PWM_Start(&gAppState.htim2, TIM_CHANNEL_4)) != HAL_OK) {
        return ERROR_PWM_TIM_START;
    }

    __HAL_LINKDMA(&gAppState.htim2, hdma[TIM_DMA_ID_CC1], gAppState.hdma1ch5);

    xTaskCreate(
        pwm_task,
        "PWM Task",
        PWM_TASK_STACK_DEPTH,
        NULL,
        PWM_TASK_PRIORITY,
        &gAppState.Tasks.PWMTask
    );

    xTaskCreate(
        resonance_task,
        "Resonance Task",
        RESONANCE_TASK_STACK_DEPTH,
        NULL,
        RESONANCE_TASK_PRIORITY,
        &gAppState.Tasks.ResonanceTask
    );

    return ERROR_OK;
}

void pwm_task(void *arg) {
    uint32_t enc_value;
    HAL_StatusTypeDef hal_err;
    SHVAL_ErrorTypeDef shval_err;

    while (1) {
        if (xEventGroupWaitBits(gAppState.SharedValues.EncValue.EventGroup, SHVAL_EVT_BITS_ENC_VALUE_PWM, pdTRUE, pdFALSE, portMAX_DELAY)) {
            find_resonance_freq();

            if ((shval_err = SHVAL_GetValue(&gAppState.SharedValues.EncValue, &enc_value, 1000)) != SHVAL_ERROR_OK) {
                ERROR_Trigger(ERROR_ENCODER_SHVAL_READ);
                continue;
            }

            if ((hal_err = HAL_TIM_PWM_Stop(&gAppState.htim2, TIM_CHANNEL_3)) != HAL_OK) {
                ERROR_Trigger(ERROR_PWM_STOP);
                continue;
            };

            if ((hal_err = HAL_TIM_PWM_Stop(&gAppState.htim2, TIM_CHANNEL_4)) != HAL_OK) {
                ERROR_Trigger(ERROR_PWM_STOP);
                continue;
            };

            uint32_t new_period = calc_pwm_period(enc_value * 1000);
            __HAL_TIM_SET_AUTORELOAD(&gAppState.htim2, new_period - 1);

            __HAL_TIM_SET_COMPARE(&gAppState.htim2, TIM_CHANNEL_3, (new_period / 2) - DEAD_TIME_TICKS);
            __HAL_TIM_SET_COMPARE(&gAppState.htim2, TIM_CHANNEL_4, (new_period / 2) + DEAD_TIME_TICKS);

            if ((hal_err = HAL_TIM_PWM_Start(&gAppState.htim2, TIM_CHANNEL_3)) != HAL_OK) {
                ERROR_Trigger(ERROR_PWM_TIM_START);
            }

            if ((hal_err = HAL_TIM_PWM_Start(&gAppState.htim2, TIM_CHANNEL_4)) != HAL_OK) {
                ERROR_Trigger(ERROR_PWM_TIM_START);
            }
        }
    }
}

void resonance_task(void *arg) {
    while (1) {
        if (ulTaskNotifyTake(pdTRUE, portMAX_DELAY)) {
        }
    }
}

uint16_t calc_pwm_period(uint32_t freq_hz) {
    // Divide by 2 to account for the TIM center aligned mode
    return ((TIM_CLK_MHZ * 1000000) / freq_hz) / 2;
}

uint32_t find_resonance_freq() {
    HAL_StatusTypeDef hal_err;
    // Find current LC frequency
    __HAL_TIM_CLEAR_FLAG(&gAppState.htim2, TIM_FLAG_CC1 | TIM_FLAG_CC1OF);

    if ((hal_err = HAL_TIM_IC_Start_DMA(&gAppState.htim2, TIM_CHANNEL_1, input_capture_values, 2)) != HAL_OK) {
        return -1;
    };

    vTaskDelay(pdMS_TO_TICKS(10));

    if ((hal_err = HAL_TIM_IC_Stop_DMA(&gAppState.htim2, TIM_CHANNEL_1)) != HAL_OK) return -1;

    uint8_t remaining_transfers = __HAL_DMA_GET_COUNTER(&gAppState.hdma1ch5);
    if (remaining_transfers > 0) return -1;

    __HAL_TIM_CLEAR_FLAG(&gAppState.htim2, TIM_FLAG_CC1 | TIM_FLAG_CC1OF);

    uint32_t period_ticks = input_capture_values[1] - input_capture_values[0];
    uint32_t hclk = HAL_RCC_GetHCLKFreq();
    uint32_t pclk1 = HAL_RCC_GetPCLK1Freq();
    uint32_t timer_clock_hz = hclk == pclk1 ? pclk1 : pclk1 * 2;

    uint32_t frequency = timer_clock_hz / period_ticks;
    (void)frequency;

    // // Start with small frequency and increment by 100Hz
    // for (uint32_t freq = 10000; freq < 120000; freq += 100) {
    //     uint32_t new_period = calc_pwm_period(freq);
    //
    //     __HAL_TIM_SET_AUTORELOAD(&gAppState.htim2, new_period - 1);
    //
    //     __HAL_TIM_SET_COMPARE(&gAppState.htim2, TIM_CHANNEL_3, (new_period / 2) - DEAD_TIME_TICKS);
    //     __HAL_TIM_SET_COMPARE(&gAppState.htim2, TIM_CHANNEL_4, (new_period / 2) + DEAD_TIME_TICKS);
    //
    //     return 10000;
    // }
    return 10000;
}
