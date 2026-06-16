//
// Created by Kok on 6/4/26.
//

#ifndef STM32_EMF_HEATER_DIGITAL_ERROR_H
#define STM32_EMF_HEATER_DIGITAL_ERROR_H

#include <stdint.h>

typedef enum {
    ERROR_OK,
    ERROR_PWR_OSC,
    ERROR_PWR_CLK,
    ERROR_I2C_INIT,
    ERROR_DISPLAY_INIT,
    ERROR_DISPLAY_BOOT_MSG,
    ERROR_ENCODER_INIT,
    ERROR_ENCODER_START,
    ERROR_ENCODER_SHVAL_READ,
    ERROR_ENCODER_SHVAL_WRITE,
    ERROR_PWM_TIM_INIT,
    ERROR_PWM_LPTIM_INIT,
    ERROR_PWM_COMP_INIT,
    ERROR_PWM_TIM_OC_CONF,
    ERROR_IC_TIM_CONF,
    ERROR_PWM_TIM_START,
    ERROR_PWM_LPTIM_START,
    ERROR_PWM_COMP_START,
    ERROR_PWM_TIM_STOP,
    ERROR_PWM_COMP_OUT_TIMEOUT
} AppErrorTypeDef;

typedef struct {
    uint32_t Magic;
    uint8_t ResetCount;
    uint32_t Cursor;
    AppErrorTypeDef errors[2];
} AppErrorLogTypeDef;

void ERROR_Init();

AppErrorLogTypeDef *ERROR_DumpLog();

void ERROR_Trigger(AppErrorTypeDef Error);
void ERROR_TriggerFatal(AppErrorTypeDef Error);

#endif //STM32_EMF_HEATER_DIGITAL_ERROR_H