//
// Created by i.stefanov on 2.4.2026 г..
//

#include "app_state.h"
#include "display.h"
#include "encoder.h"
#include "i2c.h"
#include "power.h"
#include "stm32l0xx_hal.h"

#include "gpio_defs.h"
#include "pwm.h"
#include "task.h"

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
  while (1)
    ;
}

int main(void) {
  uint8_t err;
  if ((err = HAL_Init()) != HAL_OK) {
    while (1)
      ;
  }

  // Configure error handler
  ERROR_Init();

  // Configure clocks and power
  if ((err = POWER_Init()) != ERROR_OK) {
    ERROR_TriggerFatal(err);
  }

  // Configure I2C peripheral
  if ((err = I2C_Init()) != ERROR_OK) {
    ERROR_TriggerFatal(err);
  }

  // Configure display
  // if ((err = DISPLAY_Init()) != ERROR_OK) {
  //   ERROR_TriggerFatal(err);
  // }

  // Configure encoder
  if ((err = ENCODER_Init()) != ERROR_OK) {
    ERROR_TriggerFatal(err);
  }

  // Configure the PWM signals
  if ((err = PWM_Init()) != ERROR_OK) {
    ERROR_TriggerFatal(err);
  }

  // Start scheduler
  vTaskStartScheduler();
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM22) {
    // Handle encoder event
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTaskNotifyFromISR(gAppState.Tasks.EncTask, 0, eSetValueWithOverwrite,
                       &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}

void HAL_GPIO_EXTI_Callback(uint16_t Pin) {
  if (Pin == GPIO_PIN_ENC_PUSH) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTaskNotifyFromISR(gAppState.Tasks.EncTask, RESONANCE_FREQ_MODE_SIGNAL,
                       eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  } else if (Pin == GPIO_PIN_TEST) {
    static uint8_t timestamp_count = 0;
    static uint16_t prev_timestamp = 0;

    if (timestamp_count == 0) {
      prev_timestamp = HAL_LPTIM_ReadCounter(&gAppState.hlptim);
      timestamp_count++;
      return;
    }

    uint16_t current_timestamp = HAL_LPTIM_ReadCounter(&gAppState.hlptim);
    uint32_t delta = (uint16_t)(current_timestamp - prev_timestamp);
    timestamp_count = 0;

    if (delta == 0)
      return;

    // Disable comparator interrupts
    // HAL_NVIC_DisableIRQ(ADC1_COMP_IRQn);
    HAL_NVIC_DisableIRQ(EXTI4_15_IRQn);

    // Sync phases
    __HAL_TIM_DISABLE(&gAppState.htim2);
    __HAL_TIM_SET_COUNTER(&gAppState.htim2,
                          (__HAL_TIM_GET_AUTORELOAD(&gAppState.htim2) * 100) /
                              200);
    __HAL_TIM_ENABLE(&gAppState.htim2);

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTaskNotifyFromISR(gAppState.Tasks.ResonanceTask, delta,
                       eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}
//
// void HAL_COMP_TriggerCallback(COMP_HandleTypeDef *hcomp) {
//     if (hcomp->Instance == COMP1) {
//         static uint8_t timestamp_count = 0;
//         static uint16_t prev_timestamp = 0;
//
//         if (timestamp_count == 0) {
//             prev_timestamp = HAL_LPTIM_ReadCounter(&gAppState.hlptim);
//             timestamp_count++;
//             return;
//         }
//
//         uint16_t current_timestamp =
//         HAL_LPTIM_ReadCounter(&gAppState.hlptim); uint32_t delta =
//         (uint16_t)(current_timestamp - prev_timestamp); timestamp_count = 0;
//
//         if (delta == 0) return;
//
//         // Disable comparator interrupts
//         HAL_NVIC_DisableIRQ(ADC1_COMP_IRQn);
//
//         // Sync phases
//         __HAL_TIM_SET_COUNTER(&gAppState.htim2, 0);
//
//         BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//         xTaskNotifyFromISR(gAppState.Tasks.ResonanceTask, delta,
//         eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
//         portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
//     }
// }