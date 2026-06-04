//
// Created by Kok on 7/17/25.
//

#include "app_state.h"

#include "stm32l0xx_hal.h"

void TIM21_IRQHandler() {
    HAL_IncTick();
    HAL_TIM_IRQHandler(&gAppState.htim21);
}