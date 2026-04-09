//
// Created by Kok on 2/12/26.
//

#ifndef TASK_SCHEDULER_H
#define TASK_SCHEDULER_H

#include "FreeRTOS.h"
#include "task.h"

typedef struct {
    volatile uint8_t Active;
    TaskHandle_t OsTask;
    TaskFunction_t Function;
    char *Name;
    void *Args;
    uint8_t Priority;
    uint32_t StackDepth;
} SCHEDULER_TaskTypeDef;

void SCHEDULER_Create(SCHEDULER_TaskTypeDef *Task);
void SCHEDULER_Remove(SCHEDULER_TaskTypeDef *Task);

#endif //TASK_SCHEDULER_H
