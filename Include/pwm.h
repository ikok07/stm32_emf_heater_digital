//
// Created by Kok on 6/9/26.
//

#ifndef STM32_EMF_HEATER_DIGITAL_PWM_H
#define STM32_EMF_HEATER_DIGITAL_PWM_H

#include "error.h"

#define INITIAL_PWM_FREQ_HZ                 120000
#define MIN_PWM_FREQ_HZ                     500
#define MAX_PWM_FREQ_HZ                     120000

AppErrorTypeDef PWM_Init();

#endif //STM32_EMF_HEATER_DIGITAL_PWM_H