//
// Created by Kok on 4/9/26.
//

#include "power.h"

HAL_StatusTypeDef POWER_Init() {
    HAL_StatusTypeDef hal_err = HAL_OK;

    // Only MSI is running
    RCC_OscInitTypeDef OSC_Config = {
        .OscillatorType = RCC_OSCILLATORTYPE_MSI,
        .MSIState = RCC_MSI_ON,
        .MSIClockRange = RCC_MSIRANGE_6,         // 4.194 MHz
        .MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT
    };

    if ((hal_err = HAL_RCC_OscConfig(&OSC_Config)) != HAL_OK) return hal_err;

    // All clock prescalers are set to 1
    RCC_ClkInitTypeDef CLK_Config = {
        .ClockType      = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                          RCC_CLOCKTYPE_PCLK1  | RCC_CLOCKTYPE_PCLK2,
        .SYSCLKSource   = RCC_SYSCLKSOURCE_MSI,
        .AHBCLKDivider  = RCC_SYSCLK_DIV1,
        .APB1CLKDivider = RCC_HCLK_DIV1,
        .APB2CLKDivider = RCC_HCLK_DIV1
    };

    if ((hal_err = HAL_RCC_ClockConfig(&CLK_Config, FLASH_LATENCY_0)) != HAL_OK) return hal_err;

    return hal_err;
}
