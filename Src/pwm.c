//
// Created by Kok on 6/9/26.
//

#include "pwm.h"

#include "FreeRTOS.h"
#include "task.h"
#include "app_state.h"
#include "encoder.h"
#include "gpio_defs.h"
#include "nvic_defs.h"
#include "svhal_evt_bits_def.h"
#include "tasks_common.h"

#define TIM_CLK_MHZ                         16
#define INITIAL_PWM_FREQ_HZ                 120000

#define DEAD_TIME_TICKS                     6       // ~ 375ns

volatile uint32_t input_capture_values[2] = {0};

uint16_t calc_pwm_period(uint32_t freq_hz);
uint32_t find_resonance_freq();
uint32_t find_current_lc_freq();

HAL_StatusTypeDef stop_pwm();
HAL_StatusTypeDef start_pwm();

void pwm_task(void *arg);
void resonance_task(void *arg);

AppErrorTypeDef PWM_Init() {

    uint16_t initial_period = calc_pwm_period(INITIAL_PWM_FREQ_HZ);
    gAppState.htim2 = (TIM_HandleTypeDef){
        .Instance = TIM2,
        .Init = {
            .AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE,
            .ClockDivision = TIM_CLOCKDIVISION_DIV1,
            .CounterMode = TIM_COUNTERMODE_CENTERALIGNED1,
            // .CounterMode = TIM_COUNTERMODE_UP,
            .Prescaler = 0,                // 32 MHz
            .Period = initial_period - 1
        }
    };

    gAppState.hlptim = (LPTIM_HandleTypeDef){
        .Instance = LPTIM1,
        .Init = {
            .Clock = {
                .Source = LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC,
                .Prescaler = LPTIM_PRESCALER_DIV1
            },
            .CounterSource = LPTIM_COUNTERSOURCE_INTERNAL,
            .Trigger = {
                .Source = LPTIM_TRIGSOURCE_SOFTWARE,
                .SampleTime = LPTIM_TRIGSAMPLETIME_DIRECTTRANSITION
            },
            .UpdateMode = LPTIM_UPDATE_IMMEDIATE
        }
    };

    gAppState.hcomp1 = (COMP_HandleTypeDef){
        .Instance = COMP1,
        .Init = {
            .NonInvertingInput = COMP_INPUT_PLUS_IO1,                       // PA1
            .LPTIMConnection = COMP_LPTIMCONNECTION_DISABLED,
            .OutputPol = COMP_OUTPUTPOL_NONINVERTED,
            .TriggerMode = COMP_TRIGGERMODE_IT_RISING,
            .WindowMode = COMP_WINDOWMODE_DISABLE,                          // Positive input connected to PA1
            .InvertingInput = COMP_INPUT_MINUS_DAC1_CH1,                    // PA4
        }
    };

    HAL_StatusTypeDef hal_err;
    if ((hal_err = HAL_TIM_PWM_Init(&gAppState.htim2)) != HAL_OK) {
        return ERROR_PWM_TIM_INIT;
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

    // Configure LPTIM for resonance frequency detection
    if ((hal_err = HAL_LPTIM_Init(&gAppState.hlptim)) != HAL_OK) {
        return ERROR_PWM_LPTIM_INIT;
    }

    // Configure COMP1
    if ((hal_err = HAL_COMP_Init(&gAppState.hcomp1)) != HAL_OK) {
        return ERROR_PWM_COMP_INIT;
    }

    if ((hal_err = start_pwm()) != HAL_OK) {
        return ERROR_PWM_TIM_START;
    }

    if ((hal_err = HAL_LPTIM_Counter_Start(&gAppState.hlptim, 0xFFFF)) != HAL_OK) {
        return ERROR_PWM_LPTIM_START;
    }

    if ((hal_err = HAL_COMP_Start(&gAppState.hcomp1)) != HAL_OK) {
        return ERROR_PWM_COMP_START;
    }

    HAL_NVIC_SetPriority(LPTIM1_IRQn, NVIC_PRIORITY_LPTIM1, 0);

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
            if ((shval_err = SHVAL_GetValue(&gAppState.SharedValues.EncValue, &enc_value, 1000)) != SHVAL_ERROR_OK) {
                ERROR_Trigger(ERROR_ENCODER_SHVAL_READ);
                continue;
            }

            if (enc_value == RESONANCE_FREQ_MODE) {
                // Start resonance task
                vTaskResume(gAppState.Tasks.ResonanceTask);
                continue;
            }

            if ((hal_err = stop_pwm()) != HAL_OK) {
                ERROR_Trigger(ERROR_PWM_TIM_STOP);
                continue;
            }

            uint32_t new_period = calc_pwm_period(enc_value * 1000);
            __HAL_TIM_SET_AUTORELOAD(&gAppState.htim2, new_period - 1);

            __HAL_TIM_SET_COMPARE(&gAppState.htim2, TIM_CHANNEL_3, (new_period / 2) - DEAD_TIME_TICKS);
            __HAL_TIM_SET_COMPARE(&gAppState.htim2, TIM_CHANNEL_4, (new_period / 2) + DEAD_TIME_TICKS);
            __HAL_TIM_SET_COUNTER(&gAppState.htim2, 0);

            if ((hal_err = start_pwm()) != HAL_OK) {
                ERROR_Trigger(ERROR_PWM_TIM_START);
                continue;
            }
        }
    }
}

void resonance_task(void *arg) {
    SHVAL_ErrorTypeDef shval_err;
    HAL_StatusTypeDef hal_err;
    uint32_t enc_value;

    // Wait for GPIO to stabilize
    vTaskDelay(pdMS_TO_TICKS(100));

    while (1) {
        // Get encoder value
        if ((shval_err = SHVAL_GetValue(&gAppState.SharedValues.EncValue, &enc_value, 1000)) != SHVAL_ERROR_OK) {
            ERROR_Trigger(ERROR_ENCODER_SHVAL_READ);
            continue;
        }

        // Stop task if not in resonance frequency mode
        if (enc_value != RESONANCE_FREQ_MODE) {
            // Enable comparator interrupts
            HAL_NVIC_DisableIRQ(EXTI4_15_IRQn);

            vTaskSuspend(NULL);
        }

        // Enable comparator interrupts
        HAL_NVIC_EnableIRQ(ADC1_COMP_IRQn);

        uint32_t delta;
        if (xTaskNotifyWait(0x00, 0xFF, &delta, pdMS_TO_TICKS(1000))) {
            // Interrupts are disabled in the ISR callback method...

            uint32_t pclk1 = HAL_RCC_GetPCLK1Freq();
            uint32_t frequency = pclk1 / delta;
            uint32_t pwm_period = calc_pwm_period(frequency);

            if ((hal_err = stop_pwm()) != HAL_OK) {
                ERROR_Trigger(ERROR_PWM_TIM_STOP);
                continue;
            }

            __HAL_TIM_SET_AUTORELOAD(&gAppState.htim2, pwm_period - 1);

            __HAL_TIM_SET_COMPARE(&gAppState.htim2, TIM_CHANNEL_3, (pwm_period / 2) - DEAD_TIME_TICKS);
            __HAL_TIM_SET_COMPARE(&gAppState.htim2, TIM_CHANNEL_4, (pwm_period / 2) + DEAD_TIME_TICKS);

            if ((hal_err = start_pwm()) != HAL_OK) {
                ERROR_Trigger(ERROR_PWM_TIM_START);
                continue;
            }
        } else {
            // Disable comparator interrupts
            HAL_NVIC_DisableIRQ(ADC1_COMP_IRQn);

            ERROR_Trigger(ERROR_PWM_COMP_OUT_TIMEOUT);
            continue;
        }
    }
}

HAL_StatusTypeDef stop_pwm() {
    HAL_StatusTypeDef hal_err = HAL_OK;
    if ((hal_err = HAL_TIM_Base_Stop(&gAppState.htim2)) != HAL_OK) {
        return hal_err;
    }
    return hal_err;
}

HAL_StatusTypeDef start_pwm() {
    HAL_StatusTypeDef hal_err = HAL_OK;

    if ((hal_err = HAL_TIM_Base_Start(&gAppState.htim2)) != HAL_OK) {
        return hal_err;
    }

    if ((hal_err = HAL_TIM_PWM_Start(&gAppState.htim2, TIM_CHANNEL_3)) != HAL_OK) {
        return hal_err;
    }

    if ((hal_err = HAL_TIM_PWM_Start(&gAppState.htim2, TIM_CHANNEL_4)) != HAL_OK) {
        return hal_err;
    }

    return hal_err;
}

uint16_t calc_pwm_period(uint32_t freq_hz) {
    // Divide by 2 to account for the TIM center aligned mode
    return ((TIM_CLK_MHZ * 1000000) / freq_hz) / 2;
}
