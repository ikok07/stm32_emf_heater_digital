//
// Created by i.stefanov on 2.4.2026 г..
//

#include "app_state.h"
#include "display.h"
#include "FreeRTOS.h"
#include "i2c.h"
#include "power.h"
#include "task.h"
#include "stm32l0xx_hal.h"

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

    // Start scheduler
    vTaskStartScheduler();
}