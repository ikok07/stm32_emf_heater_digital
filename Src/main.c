//
// Created by i.stefanov on 2.4.2026 г..
//

#include "app_state.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_gcc.h"
#include "dac.h"
#include "encoder.h"
#include "i2c.h"
#include "log.h"
#include "logging.h"
#include "power.h"
#include "stm32l0xx_hal.h"

void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName ) {
    LOGGER_LogBasic(1);
    while (1) {}
}

// TODO: Add encoder reset handlers

int main(void) {
    uint8_t err;
    if ((err = HAL_Init()) != HAL_OK) {
        while (1);
    }

    // Initialize logger
    LOGGING_Init();

    // Setup power and clocks
    if ((err = POWER_Init()) != HAL_OK) {
        LOGGER_Log(LOGGER_LEVEL_FATAL, "Failed to configure MCU's power and clocks!");
    }

    // Setup I2C Bus
    if ((err = I2C_Init()) != 0) {
        LOGGER_Log(LOGGER_LEVEL_FATAL, "Failed to configure MCU's I2C peripheral!");
    }

    // Initialize DAC
    if ((err = DAC_Init()) != 0) {
        LOGGER_Log(LOGGER_LEVEL_FATAL, "Failed to configure the external DAC!");
    }

    // Initialize encoder
    if ((err = ENCODER_Init()) != HAL_OK) {
        LOGGER_Log(LOGGER_LEVEL_FATAL, "Failed to configure the encoder timers!");
    }

    // Start DAC tasks
    DAC_StartTasks();

    // Start encoder tasks
    ENCODER_StartTasks();

    // Start scheduler
    vTaskStartScheduler();
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == DAC_CHCTRL_1_PIN || GPIO_Pin == DAC_CHCTRL_2_PIN) {
        DAC_HandleChanCtrlEvent();
    }
}