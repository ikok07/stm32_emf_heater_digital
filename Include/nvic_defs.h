//
// Created by Kok on 6/12/26.
//

#ifndef STM32_EMF_HEATER_DIGITAL_NVIC_DEFS_H
#define STM32_EMF_HEATER_DIGITAL_NVIC_DEFS_H

#define NVIC_PRIORITY_TIM22                         6               // Encoder
#define NVIC_PRIORITY_LPTIM1                        5               // Comparator

/* ------ NON-FREERTOS ------ */

#define NVIC_PRIORITY_TIM21                         4               // HAL Tick

#endif //STM32_EMF_HEATER_DIGITAL_NVIC_DEFS_H