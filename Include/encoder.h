//
// Created by Kok on 4/9/26.
//

#ifndef STM32_DIGITAL_POTENTIOMETER_V2_ENCODER_H
#define STM32_DIGITAL_POTENTIOMETER_V2_ENCODER_H

#include "stm32l0xx_hal.h"

HAL_StatusTypeDef ENCODER_Init();
void ENCODER_StartTasks();

#endif //STM32_DIGITAL_POTENTIOMETER_V2_ENCODER_H