//
// Created by Kok on 6/16/26.
//

#include "gpio_defs.h"
#include "stm32l0xx_hal.h"

void HAL_COMP_MspInit(COMP_HandleTypeDef *hcomp) {
    if (hcomp->Instance == COMP1) {
        // __HAL_RCC_GPIOA_CLK_ENABLE();
        //
        // GPIO_InitTypeDef gpio_config = {
        //     .Mode = GPIO_MODE_ANALOG,
        //     .Pin = GPIO_PIN_COMP_REF,
        //     .Pull = GPIO_NOPULL,
        // };
        // HAL_GPIO_Init(GPIO_PORT_COMP_REF, &gpio_config);
        //
        // gpio_config.Pin = GPIO_PIN_COMP_IN;
        // HAL_GPIO_Init(GPIO_PORT_COMP_IN, &gpio_config);
    }
}