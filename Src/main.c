//
// Created by i.stefanov on 2.4.2026 г..
//

#include "app_state.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_gcc.h"
#include "dac.h"
#include "i2c.h"
#include "log.h"
#include "stm32l0xx_hal.h"

void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName ) {
    __disable_irq();
    while (1) {}
}

int main(void) {
    HAL_StatusTypeDef hal_err;
    if ((hal_err = HAL_Init()) != HAL_OK) {
        while (1);
    }

    // Initialize logger
    LOGGER_Init();
    LOGGER_Enable();

    // Setup I2C Bus
    if ((hal_err = I2C_Init()) != HAL_OK) {
        LOGGER_LogBasic();
    }

    vTaskStartScheduler();
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == DAC_CHCTRL_1_PIN || GPIO_Pin == DAC_CHCTRL_2_PIN) {
        DAC_HandleChanCtrlEvent();
    }
}