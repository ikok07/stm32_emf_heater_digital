//
// Created by Kok on 4/9/26.
//

#include "app_state.h"
#include "gpio_defs.h"
#include "nvic_defs.h"
#include "stm32l0xx_hal.h"

#define ENC1_PORT                   GPIOA
#define ENC1_A_PIN                  0
#define ENC1_B_PIN                  1

#define ENC2_PORT                   GPIOA
#define ENC2_A_PIN                  6
#define ENC2_B_PIN                  7


HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority) {
    HAL_StatusTypeDef hal_err = HAL_OK;

    gAppState.htim21 = (TIM_HandleTypeDef){
        .Instance = TIM21,
        .Init = {
            .ClockDivision = TIM_CLOCKDIVISION_DIV1,
            .AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE,
            .CounterMode = TIM_COUNTERMODE_UP,
            .Prescaler = (SystemCoreClock / 1000000) - 1,
            .Period =  (1000000U / configTICK_RATE_HZ) - 1
        }
    };

    if ((hal_err = HAL_TIM_Base_Init(&gAppState.htim21)) != HAL_OK) return hal_err;
    if ((hal_err = HAL_TIM_Base_Start_IT(&gAppState.htim21)) != HAL_OK) return hal_err;
    return hal_err;
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM21) {
        __HAL_RCC_TIM21_CLK_ENABLE();
        HAL_NVIC_SetPriority(TIM21_IRQn, NVIC_PRIORITY_TIM21, 0);
        HAL_NVIC_EnableIRQ(TIM21_IRQn);
    }
}

void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM22) {
        __HAL_RCC_TIM22_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        GPIO_InitTypeDef gpio_conf = {
            .Mode = GPIO_MODE_AF_PP,
            .Alternate = GPIO_AF5_TIM22,
            .Pull = GPIO_PULLUP,
            .Speed = GPIO_SPEED_FREQ_MEDIUM
        };

        gpio_conf.Pin = GPIO_PIN_ENC_CH1;
        HAL_GPIO_Init(GPIO_PORT_ENC_CH1, &gpio_conf);

        gpio_conf.Pin = GPIO_PIN_ENC_CH2;
        HAL_GPIO_Init(GPIO_PORT_ENC_CH2, &gpio_conf);

        HAL_NVIC_SetPriority(TIM22_IRQn, NVIC_PRIORITY_TIM22, 0);
        HAL_NVIC_EnableIRQ(TIM22_IRQn);
    }
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_TIM2_CLK_ENABLE();

        GPIO_InitTypeDef gpio_conf = {
            .Mode = GPIO_MODE_AF_PP,
            .Pull = GPIO_NOPULL,
            .Speed = GPIO_SPEED_FREQ_VERY_HIGH,
            .Alternate = GPIO_AF2_TIM2,
        };

        gpio_conf.Pin = GPIO_PIN_PWM_A;
        HAL_GPIO_Init(GPIO_PORT_PWM_A, &gpio_conf);

        gpio_conf.Pin = GPIO_PIN_PWM_B;
        HAL_GPIO_Init(GPIO_PORT_PWM_B, &gpio_conf);
    }
}

void HAL_LPTIM_MspInit(LPTIM_HandleTypeDef *hlptim) {
    __HAL_RCC_LPTIM1_CLK_ENABLE();
    __HAL_RCC_LPTIM1_CONFIG(RCC_LPTIM1CLKSOURCE_PCLK1);

    /* ------ TEMP ------ */
    GPIO_InitTypeDef gpio_conf = {
        .Mode = GPIO_MODE_IT_FALLING,
        .Pin = GPIO_PIN_TEST,
        .Pull = GPIO_PULLDOWN,
        .Speed = GPIO_SPEED_FREQ_VERY_HIGH
    };
    HAL_GPIO_Init(GPIO_PORT_TEST, &gpio_conf);
    HAL_NVIC_SetPriority(EXTI4_15_IRQn, NVIC_PRIORITY_COMP, 0);
    /* ------ TEMP ------ */
}