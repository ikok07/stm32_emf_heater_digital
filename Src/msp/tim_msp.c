//
// Created by Kok on 4/9/26.
//

#include "stm32l0xx_hal.h"

#define ENC1_PORT                   GPIOA
#define ENC1_A_PIN                  0
#define ENC1_B_PIN                  1

#define ENC2_PORT                   GPIOA
#define ENC2_A_PIN                  6
#define ENC2_B_PIN                  7

void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef *htim) {
    __HAL_RCC_TIM2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_Config = {
        .Mode = GPIO_MODE_AF_PP,
        .Pin = ENC1_A_PIN,
        .Pull = GPIO_PULLUP,
        .Speed = GPIO_SPEED_LOW
    };
    HAL_GPIO_Init(ENC1_PORT, &GPIO_Config);

    GPIO_Config.Pin = ENC1_B_PIN;
    HAL_GPIO_Init(ENC1_PORT, &GPIO_Config);

    GPIO_Config.Pin = ENC2_A_PIN;
    HAL_GPIO_Init(ENC2_PORT, &GPIO_Config);

    GPIO_Config.Pin = ENC2_B_PIN;
    HAL_GPIO_Init(ENC2_PORT, &GPIO_Config);
}