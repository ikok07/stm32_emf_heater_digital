//
// Created by Kok on 6/4/26.
//

#include "gpio_defs.h"
#include "stm32l0xx_hal.h"

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c) {
    __HAL_RCC_I2C1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef gpio_config = {
        .Mode = GPIO_MODE_AF_OD,
        .Alternate = GPIO_AF1_I2C1,
        .Pull = GPIO_PULLUP
    };

    gpio_config.Pin = GPIO_PIN_I2C_SCL;
    HAL_GPIO_Init(GPIO_PORT_I2C_SCL, &gpio_config);

    gpio_config.Pin = GPIO_PIN_I2C_SDA;
    HAL_GPIO_Init(GPIO_PORT_I2C_SDA, &gpio_config);
}