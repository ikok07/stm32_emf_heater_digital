//
// Created by Kok on 6/4/26.
//

#include "power.h"

#include "stm32l0xx_hal.h"

AppErrorTypeDef POWER_Init() {
  // Config for 32MHz
  RCC_OscInitTypeDef osc_config = {
      .OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSE,
      .HSIState = RCC_HSI_ON,
      .HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT,
      .HSEState = RCC_HSE_OFF,
      .PLL = {.PLLState = RCC_PLL_ON,
              .PLLSource = RCC_PLLSOURCE_HSI,
              .PLLDIV = RCC_PLL_DIV2,
              .PLLMUL = RCC_PLLMUL_4}};

  HAL_StatusTypeDef hal_err;
  if ((hal_err = HAL_RCC_OscConfig(&osc_config)) != HAL_OK) {
    return ERROR_PWR_OSC;
  }

  RCC_ClkInitTypeDef clk_config = {
      .ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                   RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2,
      .SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK,
      .AHBCLKDivider = RCC_SYSCLK_DIV1,
      .APB1CLKDivider = RCC_HCLK_DIV1,
      .APB2CLKDivider = RCC_HCLK_DIV1};

  if ((hal_err = HAL_RCC_ClockConfig(&clk_config, FLASH_LATENCY_1)) != HAL_OK) {
    return ERROR_PWR_CLK;
  }

  // Disable other clocks
  osc_config = (RCC_OscInitTypeDef){.OscillatorType = RCC_OSCILLATORTYPE_MSI |
                                                      RCC_OSCILLATORTYPE_LSI |
                                                      RCC_OSCILLATORTYPE_LSE,
                                    .MSIState = RCC_MSI_OFF,
                                    .LSIState = RCC_LSI_OFF,
                                    .LSEState = RCC_LSE_OFF};

  if ((hal_err = HAL_RCC_OscConfig(&osc_config)) != HAL_OK) {
    return ERROR_PWR_OSC;
  }

  return ERROR_OK;
}
