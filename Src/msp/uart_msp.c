//
// Created by Kok on 4/9/26.
//

#include "stm32l0xx_hal.h"

#define UART_PORT                               GPIOA
#define UART_TX_PIN                             2

void HAL_UART_MspInit(UART_HandleTypeDef *huart) {
    __HAL_RCC_LPUART1_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_Config = {
        .Mode = GPIO_MODE_AF_PP,
        .Pin = UART_TX_PIN,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FAST,
        .Alternate = GPIO_AF6_LPUART1
    };

    HAL_GPIO_Init(UART_PORT, &GPIO_Config);
}