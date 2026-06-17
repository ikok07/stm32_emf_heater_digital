//
// Created by Kok on 7/17/25.
//

#include "app_state.h"
#include "gpio_defs.h"

#include "stm32l0xx_hal.h"

void TIM2_IRQHandler() {
    HAL_TIM_IRQHandler(&gAppState.htim2);
}

void TIM21_IRQHandler() {
    HAL_IncTick();
    HAL_TIM_IRQHandler(&gAppState.htim21);
}

void TIM22_IRQHandler() {
    HAL_TIM_IRQHandler(&gAppState.htim22);
}

void EXTI0_1_IRQHandler() {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_ENC_PUSH);
}

void EXTI4_15_IRQHandler() {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_TEST);
}

void ADC1_COMP_IRQHandler() {
    HAL_COMP_IRQHandler(&gAppState.hcomp1);
}