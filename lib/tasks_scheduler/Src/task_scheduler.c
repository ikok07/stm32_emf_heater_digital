//
// Created by Kok on 2/12/26.
//

#include "task_scheduler.h"

void SCHEDULER_Create(SCHEDULER_TaskTypeDef *Task) {
    xTaskCreate(
        Task->Function,
        Task->Name,
        Task->StackDepth,
        Task->Args,
        Task->Priority,
        &Task->OsTask
    );
    Task->Active = 1;
}

void SCHEDULER_Remove(SCHEDULER_TaskTypeDef *Task) {
    TaskHandle_t os_task = Task->OsTask;
    *Task = (SCHEDULER_TaskTypeDef){0};
    vTaskSuspend(os_task);
}
