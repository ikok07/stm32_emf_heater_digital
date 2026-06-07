//
// Created by Kok on 6/4/26.
//

#include "error.h"

#include <string.h>

#include "stm32l0xx_hal.h"
#include "gpio_defs.h"

#include "FreeRTOS.h"
#include "task.h"

#define ERROR_LOG_MAGIC                 0x23547567

// __attribute__((section(".noinit"))) AppErrorLogTypeDef gErrorLog;
AppErrorLogTypeDef gErrorLog;

void log_error(AppErrorTypeDef error);

void ERROR_Init() {
    GPIO_InitTypeDef gpio_config = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pin = GPIO_PIN_ERROR_LED,
        .Pull = GPIO_PULLDOWN,
        .Speed = GPIO_SPEED_FREQ_LOW,
    };

    HAL_GPIO_Init(GPIO_PORT_ERROR_LED, &gpio_config);

    if (gErrorLog.Magic != ERROR_LOG_MAGIC) {
        // First time initializing the logging region
        memset(&gErrorLog, 0, sizeof(gErrorLog));
        gErrorLog.Magic = ERROR_LOG_MAGIC;
    } else {
        // A reset occurred
        gErrorLog.ResetCount++;
    }
}

AppErrorLogTypeDef *ERROR_DumpLog() {
    if (gErrorLog.Magic != ERROR_LOG_MAGIC) return NULL;
    return &gErrorLog;
}

void ERROR_Trigger(AppErrorTypeDef Error) {
    log_error(Error);
    HAL_GPIO_WritePin(GPIO_PORT_ERROR_LED, GPIO_PIN_ERROR_LED, GPIO_PIN_SET);
}

void ERROR_TriggerFatal(AppErrorTypeDef Error) {
    ERROR_Trigger(Error);

    if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
        vTaskSuspendAll();
    }

    __disable_irq();
    while (1);
}

void log_error(AppErrorTypeDef Error) {
    gErrorLog.errors[gErrorLog.Cursor++] = Error;

    if (gErrorLog.Cursor >= sizeof(gErrorLog.errors) / sizeof(gErrorLog.errors[0])) {
        gErrorLog.Cursor = 0;
    }
}