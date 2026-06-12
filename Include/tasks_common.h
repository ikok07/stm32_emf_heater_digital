//
// Created by Kok on 4/8/26.
//

#ifndef STM32_DIGITAL_POTENTIOMETER_V2_TASKS_COMMON_H
#define STM32_DIGITAL_POTENTIOMETER_V2_TASKS_COMMON_H

#define ENC_TASK_STACK_DEPTH                         256
#define ENC_TASK_PRIORITY                            5

#define DISPLAY_TASK_STACK_DEPTH                     256
#define DISPLAY_TASK_PRIORITY                        5

#define PWM_TASK_STACK_DEPTH                         128
#define PWM_TASK_PRIORITY                            4

#define RESONANCE_TASK_STACK_DEPTH                   256
#define RESONANCE_TASK_PRIORITY                      4

#endif //STM32_DIGITAL_POTENTIOMETER_V2_TASKS_COMMON_H