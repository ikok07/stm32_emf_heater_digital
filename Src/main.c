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
#include "logging.h"
#include "stm32l0xx_hal.h"

void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName ) {
    LOGGER_LogBasic(1);
    while (1) {}
}

int main(void) {
    uint8_t err;
    if ((err = HAL_Init()) != 0) {
        while (1);
    }

    // Initialize logger
    LOGGING_Init();

    // Setup clocks
    // TODO: ...

    // Setup I2C Bus
    if ((err = I2C_Init()) != 0) {
        LOGGER_LogBasic(1);
        while (1);
    }

    // Initialize DAC
    if ((err = DAC_Init()) != 0) {
        LOGGER_LogBasic(1);
        while (1);
    }

    // Initialize encoder
    // TODO: ...

    // Start DAC Tasks
    DAC_StartTasks();

    vTaskStartScheduler();
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == DAC_CHCTRL_1_PIN || GPIO_Pin == DAC_CHCTRL_2_PIN) {
        DAC_HandleChanCtrlEvent();
    }
}