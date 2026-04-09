//
// Created by Kok on 4/8/26.
//

#include "i2c.h"

#include "app_state.h"

HAL_StatusTypeDef I2C_Init() {
    gAppState.hi2c = (I2C_HandleTypeDef){
        .Instance = I2C1,
        .Mode = HAL_I2C_MODE_MASTER,
        .Init = {
            .AddressingMode = I2C_ADDRESSINGMODE_7BIT,
            .Timing = 0x00300617,                        // Fast Mode - 400 KHz
            .NoStretchMode = I2C_NOSTRETCH_DISABLED,
            .GeneralCallMode = I2C_GENERALCALL_DISABLED,
            .DualAddressMode = I2C_DUALADDRESS_DISABLED
        }
    };

    HAL_StatusTypeDef hal_err = HAL_OK;
    if ((hal_err = HAL_I2C_Init(&gAppState.hi2c)) != HAL_OK) return hal_err;
    return hal_err;
}
