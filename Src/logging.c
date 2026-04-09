//
// Created by Kok on 4/9/26.
//

#include "FreeRTOS.h"
#include "task.h"
#include "logging.h"
#include "log.h"
#include "stm32l0xx_hal_gpio.h"
#include "stm32l0xx_hal_rcc.h"

#define LOGGING_ERR_LED_GPIOEN                  __HAL_RCC_GPIOB_CLK_ENABLE()
#define LOGGING_ERR_LED_PORT                    GPIOB
#define LOGGING_ERR_LED_PIN                     7

void init_logger();
void fatal_err_cb();

void LOGGING_Init() {
    LOGGER_CallbacksTypeDef callbacks = {
        .on_init_basic = init_logger,
        .on_fatal_basic = fatal_err_cb
    };

    LOGGER_RegisterCB(&callbacks);
    LOGGER_Init();
    LOGGER_Enable();
}

void init_logger() {
    LOGGING_ERR_LED_GPIOEN;

    GPIO_InitTypeDef GPIO_Config = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pin = LOGGING_ERR_LED_PIN,
        .Pull = GPIO_PULLDOWN,
        .Speed = GPIO_SPEED_LOW
    };
    HAL_GPIO_Init(LOGGING_ERR_LED_PORT, &GPIO_Config);
}

void fatal_err_cb() {
    HAL_GPIO_WritePin(LOGGING_ERR_LED_PORT, LOGGING_ERR_LED_PIN, GPIO_PIN_SET);
    vTaskEndScheduler();
    __disable_irq();
    while (1);
}
