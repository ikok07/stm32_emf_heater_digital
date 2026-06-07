//
// Created by i.stefanov on 2.4.2026 г..
//

#include "app_state.h"
#include "display.h"
#include "encoder.h"
#include "i2c.h"
#include "power.h"
#include "stm32l0xx_hal.h"

#include "FreeRTOS.h"
#include "gpio_defs.h"
#include "task.h"

void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName ) {
    while (1);
}

int main(void) {
    uint8_t err;
    if ((err = HAL_Init()) != HAL_OK) {
        while (1);
    }

    // Configure error handler
    ERROR_Init();

    // Configure clocks and power
    if ((err = POWER_Init()) != ERROR_OK) {
        ERROR_TriggerFatal(err);
    }

    // Configure I2C peripheral
    if ((err = I2C_Init()) != ERROR_OK) {
        ERROR_TriggerFatal(err);
    }

    // Configure display
    if ((err = DISPLAY_Init()) != ERROR_OK) {
        ERROR_TriggerFatal(err);
    }

    // Configure encoder
    if ((err = ENCODER_Init()) != ERROR_OK) {
        ERROR_TriggerFatal(err);
    }

    // Start scheduler
    vTaskStartScheduler();
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM22) {
        // Handle encoder event
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xTaskNotifyFromISR(gAppState.Tasks.EncTask, 0, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t Pin) {
    if (Pin == GPIO_PIN_ENC_RES) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        // Signal the task to reset the encoder value
        xTaskNotifyFromISR(gAppState.Tasks.EncTask, 1, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);

        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}