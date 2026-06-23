//
// Created by Kok on 6/4/26.
//

#include "i2c.h"

#include "app_state.h"

AppErrorTypeDef I2C_Init()
{
    gAppState.hi2c = (I2C_HandleTypeDef){
        .Instance = I2C1,
        .Init = {
            .AddressingMode = I2C_ADDRESSINGMODE_7BIT,
            .Timing = 0x00301C79 // Fast Mode - 100KHz
        }};

    HAL_StatusTypeDef hal_err;
    if ((hal_err = HAL_I2C_Init(&gAppState.hi2c)) != HAL_OK)
    {
        return ERROR_I2C_INIT;
    }

    return ERROR_OK;
}
