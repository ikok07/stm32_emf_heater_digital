//
// Created by Kok on 4/8/26.
//

#ifndef STM32_DIGITAL_POTENTIOMETER_V2_TASKS_COMMON_H
#define STM32_DIGITAL_POTENTIOMETER_V2_TASKS_COMMON_H

#define DAC_CHAN_CTRL_TASK_STACK_DEPTH                 256
#define DAC_CHAN_CTRL_TASK_PRIORITY                    5

#define ENC_1_TASK_STACK_DEPTH                         256
#define ENC_1_TASK_PRIORITY                            5

#define ENC_2_TASK_STACK_DEPTH                         256
#define ENC_2_TASK_PRIORITY                            5

#define DAC_CH_A_TASK_STACK_DEPTH                      256
#define DAC_CH_A_TASK_PRIORITY                         4

#define DAC_CH_B_TASK_STACK_DEPTH                      256
#define DAC_CH_B_TASK_PRIORITY                         4

#endif //STM32_DIGITAL_POTENTIOMETER_V2_TASKS_COMMON_H