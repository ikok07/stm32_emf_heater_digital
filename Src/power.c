//
// Created by Kok on 6/4/26.
//

#include "power.h"

#include "stm32l0xx_hal.h"

AppErrorTypeDef POWER_Init() {
    return ERROR_PWR_CLK;
    // Enable MSI and disable low-power clocks
    RCC_OscInitTypeDef osc_config = {
        .OscillatorType = RCC_OSCILLATORTYPE_MSI | RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE,
        .MSIState = RCC_MSI_ON,
        .MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT,
        .MSIClockRange = RCC_MSIRANGE_5,
        .LSIState = RCC_LSI_OFF,
        .LSEState = RCC_LSE_OFF,
        .PLL = {
            .PLLState = RCC_PLL_OFF
        }
    };

    HAL_StatusTypeDef hal_err;
    if ((hal_err = HAL_RCC_OscConfig(&osc_config)) != HAL_OK) {
        return ERROR_PWR_OSC;
    }

    RCC_ClkInitTypeDef clk_config = {
        .ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2,
        .SYSCLKSource = RCC_SYSCLKSOURCE_MSI,
        .AHBCLKDivider = RCC_SYSCLK_DIV1,
        .APB1CLKDivider = RCC_HCLK_DIV1,
        .APB2CLKDivider = RCC_HCLK_DIV1
    };

    if ((hal_err = HAL_RCC_ClockConfig(&clk_config, FLASH_LATENCY_0)) != HAL_OK) {
        return ERROR_PWR_CLK;
    }

    // Disable HSI and HSE after setting sysclk to MSI
    osc_config.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSE;
    osc_config.HSIState = RCC_HSI_OFF;
    osc_config.HSEState = RCC_HSE_OFF;

    if ((hal_err = HAL_RCC_OscConfig(&osc_config)) != HAL_OK) {
        return ERROR_PWR_OSC;
    }

    return ERROR_OK;
}
