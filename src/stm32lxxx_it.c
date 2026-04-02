//
// Created by Kok on 7/17/25.
//

#include "system_config.h"

void SysTick_Handler() {
    HAL_IncTick();
    HAL_SYSTICK_IRQHandler();
}
