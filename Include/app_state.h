//
// Created by Kok on 4/7/26.
//

#ifndef STM32_DIGITAL_POTENTIOMETER_V2_APP_STATE_H
#define STM32_DIGITAL_POTENTIOMETER_V2_APP_STATE_H

#include "dac_x050x.h"
#include "FreeRTOS.h"
#include "task.h"

#include "task_scheduler.h"
#include "shared_values.h"
#include "stm32l0xx_hal_i2c.h"

typedef struct {
    SCHEDULER_TaskTypeDef DacATask;
    SCHEDULER_TaskTypeDef DacBTask;
    SCHEDULER_TaskTypeDef DacChanCtrlTask;
} App_Tasks;

typedef struct {
    SHVAL_HandleTypeDef DacAValue;
    SHVAL_HandleTypeDef DacBValue;
} APP_SharedValues;

typedef struct {
    App_Tasks Tasks;
    APP_SharedValues SharedValues;
    I2C_HandleTypeDef hi2c;
    DACx050x_HandleTypeDef hdac;
} APP_State;

extern APP_State gAppState;

#endif //STM32_DIGITAL_POTENTIOMETER_V2_APP_STATE_H