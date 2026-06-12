//
// Created by Kok on 6/4/26.
//

#include "display.h"

#include <stdio.h>
#include <string.h>

#include "app_state.h"
#include "ssd1306.h"
#include "svhal_evt_bits_def.h"
#include "tasks_common.h"

#define DISPLAY_FONT                    Font_7x10
#define DISPLAY_FONT_W                  7
#define DISPLAY_FONT_H                  10

void display_freq(uint8_t frequency);

void display_task(void *arg);

AppErrorTypeDef DISPLAY_Init() {
    uint8_t err;
    if ((err = ssd1306_Init(&gAppState.hi2c)) != 0) {
        return ERROR_DISPLAY_INIT;
    }

    char boot_msg[] = "Booting...";

    // Write text in the middle of the screen
    ssd1306_SetCursor((128 - strlen(boot_msg) * DISPLAY_FONT_W) / 2, (64 - DISPLAY_FONT_H) / 2);
    if (ssd1306_WriteString(boot_msg, DISPLAY_FONT, White) != '\0') {
        return ERROR_DISPLAY_BOOT_MSG;
    }

    ssd1306_UpdateScreen(&gAppState.hi2c);

    xTaskCreate(
        display_task,
        "Display task",
        DISPLAY_TASK_STACK_DEPTH,
        NULL,
        DISPLAY_TASK_PRIORITY,
        &gAppState.Tasks.DisplayTask
    );

    return ERROR_OK;
}

void display_freq(uint8_t frequency) {
    char buffer[10];
    ssd1306_Fill(Black);

    snprintf(buffer, sizeof(buffer), "Frequency");
    ssd1306_SetCursor((128 - strlen(buffer) * DISPLAY_FONT_W) / 2, 7);
    ssd1306_WriteString(buffer, DISPLAY_FONT, White);

    snprintf(buffer, sizeof(buffer), "%02d KHz", frequency);
    ssd1306_SetCursor((128 - strlen(buffer) * DISPLAY_FONT_W) / 2, 39);
    ssd1306_WriteString(buffer, DISPLAY_FONT, White);

    ssd1306_UpdateScreen(&gAppState.hi2c);
}

void display_task(void *arg) {
    SHVAL_ErrorTypeDef shval_err;
    uint32_t enc_value;
    if ((shval_err = SHVAL_GetValue(&gAppState.SharedValues.EncValue, &enc_value, 1000)) == SHVAL_ERROR_OK) {
        display_freq(enc_value);
    } else {
        ERROR_Trigger(ERROR_ENCODER_SHVAL_READ);
    }

    while (1) {
        if (xEventGroupWaitBits(gAppState.SharedValues.EncValue.EventGroup, SHVAL_EVT_BITS_ENC_VALUE_DISPLAY, pdTRUE, pdFALSE, portMAX_DELAY)) {
            if ((shval_err = SHVAL_GetValue(&gAppState.SharedValues.EncValue, &enc_value, 1000)) == SHVAL_ERROR_OK) {
                display_freq(enc_value);
            } else {
                ERROR_Trigger(ERROR_ENCODER_SHVAL_READ);
            }
        }
    }
}
