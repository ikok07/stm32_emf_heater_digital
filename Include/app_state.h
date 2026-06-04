//
// Created by Kok on 4/7/26.
//

#ifndef STM32_DIGITAL_POTENTIOMETER_V2_APP_STATE_H
#define STM32_DIGITAL_POTENTIOMETER_V2_APP_STATE_H

#include "stm32l0xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

#include "task_scheduler.h"
#include "shared_values.h"
#include "stm32l0xx_hal_i2c.h"

typedef struct {

} App_Tasks;

typedef struct {

} APP_SharedValues;

typedef struct {
    App_Tasks Tasks;
    APP_SharedValues SharedValues;
    I2C_HandleTypeDef hi2c;
    TIM_HandleTypeDef htim21;
} APP_State;

extern APP_State gAppState;

#endif //STM32_DIGITAL_POTENTIOMETER_V2_APP_STATE_H