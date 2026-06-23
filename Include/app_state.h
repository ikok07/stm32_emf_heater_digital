//
// Created by Kok on 4/7/26.
//

#ifndef STM32_DIGITAL_POTENTIOMETER_V2_APP_STATE_H
#define STM32_DIGITAL_POTENTIOMETER_V2_APP_STATE_H

#include "stm32l0xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

#include "shared_values.h"

typedef struct
{
    TaskHandle_t EncTask;
    TaskHandle_t DisplayTask;
    TaskHandle_t PWMTask;
    TaskHandle_t ResonanceTask;
} App_Tasks;

typedef struct
{
    SHVAL_HandleTypeDef EncValue;
} APP_SharedValues;

typedef struct
{
    App_Tasks Tasks;
    APP_SharedValues SharedValues;
    I2C_HandleTypeDef hi2c;
    TIM_HandleTypeDef htim21;
    TIM_HandleTypeDef htim22;
    TIM_HandleTypeDef htim2;
    LPTIM_HandleTypeDef hlptim;
    COMP_HandleTypeDef hcomp1;
} APP_State;

extern APP_State gAppState;

#endif // STM32_DIGITAL_POTENTIOMETER_V2_APP_STATE_H