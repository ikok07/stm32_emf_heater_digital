//
// Created by Kok on 6/5/26.
//

#include "encoder.h"

#include "app_state.h"
#include "svhal_evt_bits_def.h"
#include "tasks_common.h"
#include "bit_defs.h"
#include "gpio_defs.h"

#define ENC_TIM_MAX_VALUE                       0xFFFF

void enc_task(void *arg);

AppErrorTypeDef ENCODER_Init() {

    // Configure encoder's timer
    gAppState.htim22 = (TIM_HandleTypeDef){
        .Instance = TIM22,
        .Init = {
            .AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE,
            .ClockDivision = TIM_CLOCKDIVISION_DIV1,
            .CounterMode = TIM_COUNTERMODE_UP,
            .Period = ENC_TIM_MAX_VALUE,
            .Prescaler = 0x00
        }
    };

    TIM_Encoder_InitTypeDef config = {
        .EncoderMode = TIM_ENCODERMODE_TI12,
        .IC1Filter = 0xF,
        .IC1Polarity = TIM_ENCODERINPUTPOLARITY_RISING,
        .IC1Prescaler = TIM_ICPSC_DIV1,
        .IC1Selection = TIM_ICSELECTION_DIRECTTI,
        .IC2Polarity = TIM_ENCODERINPUTPOLARITY_RISING,
        .IC2Filter = 0xF,
        .IC2Prescaler = TIM_ICPSC_DIV1,
        .IC2Selection = TIM_ICSELECTION_DIRECTTI
    };

    HAL_StatusTypeDef err;
    if ((err = HAL_TIM_Encoder_Init(&gAppState.htim22, &config)) != HAL_OK) {
        return ERROR_ENCODER_INIT;
    };

    if ((err = HAL_TIM_Encoder_Start_IT(&gAppState.htim22, TIM_CHANNEL_ALL)) != HAL_OK) {
        return ERROR_ENCODER_START;
    }

    // Configure reset GPIO
    GPIO_InitTypeDef gpio_config = {
        .Mode = GPIO_MODE_IT_RISING,
        .Pin = GPIO_PIN_ENC_RES,
        .Pull = GPIO_PULLDOWN,
        .Speed = GPIO_SPEED_FREQ_LOW
    };
    HAL_GPIO_Init(GPIO_PORT_ENC_RES, &gpio_config);
    NVIC_SetPriority(EXTI0_1_IRQn, 5);
    NVIC_EnableIRQ(EXTI0_1_IRQn);

    SHVAL_ConfigTypeDef shval_config = {
        .InitialValue = 0,
        .SubscribersEventBits = SHVAL_EVT_BITS_ENC_VALUE_DISPLAY
    };

    gAppState.SharedValues.EncValue = SHVAL_Init(&shval_config);

    // Set default encoder value to 0
    SHVAL_SetValue(&gAppState.SharedValues.EncValue, 0, pdMS_TO_TICKS(1000));

    xTaskCreate(
        enc_task,
        "Encoder Task",
        ENC_TASK_STACK_DEPTH,
        NULL,
        ENC_TASK_PRIORITY,
        &gAppState.Tasks.EncTask
    );

    return ERROR_OK;
}

void enc_task(void *arg) {
    SHVAL_ErrorTypeDef shval_err;
    uint16_t prev_counter = __HAL_TIM_GET_COUNTER(&gAppState.htim22);
    uint16_t prev_enc_value = 0;


    while (1) {
        uint32_t task_value;
        if (xTaskNotifyWait(0x00, 0xFF, &task_value, portMAX_DELAY)) {
            uint32_t new_value = prev_enc_value;

            if (task_value == 1) {
                uint32_t curr_value;
                if ((shval_err = SHVAL_GetValue(&gAppState.SharedValues.EncValue, &curr_value, pdMS_TO_TICKS(1000))) != SHVAL_ERROR_OK) {
                    ERROR_Trigger(ERROR_ENCODER_SHVAL_READ);
                    continue;
                }

                // Toggle encoder value
                new_value = curr_value == 0 ? prev_enc_value : 0;
                if ((shval_err = SHVAL_SetValue(&gAppState.SharedValues.EncValue, new_value, pdMS_TO_TICKS(1000))) != SHVAL_ERROR_OK) {
                    ERROR_Trigger(ERROR_ENCODER_SHVAL_WRITE);
                    continue;
                }
            }

            uint16_t curr_counter = __HAL_TIM_GET_COUNTER(&gAppState.htim22);
            int16_t counter_diff = (int16_t)(curr_counter - prev_counter);
            uint16_t counter_diff_abs = counter_diff < 0 ? -counter_diff : counter_diff;

            if (counter_diff_abs >= 4) {
                uint16_t left_value = counter_diff_abs % 4;
                uint16_t steps = counter_diff_abs / 4;

                if (counter_diff > 0 && new_value < 100) {
                    new_value += counter_diff_abs / 4;
                } else if (counter_diff < 0) {
                    new_value = (prev_enc_value > steps) ? prev_enc_value - steps : 0;
                }

                if ((shval_err = SHVAL_SetValue(&gAppState.SharedValues.EncValue, new_value, pdMS_TO_TICKS(1000))) != SHVAL_ERROR_OK) {
                    ERROR_Trigger(ERROR_ENCODER_SHVAL_WRITE);
                    continue;
                }

                prev_enc_value = new_value;

                // Update value only when the encoder value is updated
                if (counter_diff > 0)
                    prev_counter = curr_counter - left_value;
                else
                    prev_counter = curr_counter + left_value;
            }
        }
    }
}