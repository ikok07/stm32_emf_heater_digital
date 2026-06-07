//
// Created by Kok on 6/4/26.
//

#ifndef STM32_EMF_HEATER_DIGITAL_GPIO_DEFS_H
#define STM32_EMF_HEATER_DIGITAL_GPIO_DEFS_H

#include "stm32l0xx_hal.h"

#define GPIO_PORT_ERROR_LED                     GPIOB
#define GPIO_PIN_ERROR_LED                      GPIO_PIN_7

#define GPIO_PORT_I2C_SCL                       GPIOA
#define GPIO_PIN_I2C_SCL                        GPIO_PIN_9

#define GPIO_PORT_I2C_SDA                       GPIOA
#define GPIO_PIN_I2C_SDA                        GPIO_PIN_10

#define GPIO_PORT_ENC_CH1                       GPIOA
#define GPIO_PIN_ENC_CH1                        GPIO_PIN_6

#define GPIO_PORT_ENC_CH2                       GPIOA
#define GPIO_PIN_ENC_CH2                        GPIO_PIN_7

#define GPIO_PORT_ENC_RES                       GPIOA
#define GPIO_PIN_ENC_RES                        GPIO_PIN_0

#endif //STM32_EMF_HEATER_DIGITAL_GPIO_DEFS_H