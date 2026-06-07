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

void display_power(uint8_t percentage);

void display_task(void *arg);

AppErrorTypeDef DISPLAY_Init() {
    uint8_t err;
    if ((err = ssd1306_Init(&gAppState.hi2c)) != 0) {
        return ERROR_DISPLAY_INIT;
    }

    char boot_msg[] = "Booting...";

    // Write text in the middle of the screen
    ssd1306_SetCursor((128 - strlen(boot_msg) * 11) / 2, (64 - 18) / 2);
    if (ssd1306_WriteString(boot_msg, Font_11x18, White) != '\0') {
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

void display_power(uint8_t percentage) {
    char buffer[7];
    ssd1306_Fill(Black);

    snprintf(buffer, sizeof(buffer), "Power");
    ssd1306_SetCursor((128 - strlen(buffer) * 11) / 2, 7);
    ssd1306_WriteString(buffer, Font_11x18, White);

    snprintf(buffer, sizeof(buffer), "%02d %%", percentage);
    ssd1306_SetCursor((128 - strlen(buffer) * 11) / 2, 39);
    ssd1306_WriteString(buffer, Font_11x18, White);

    ssd1306_UpdateScreen(&gAppState.hi2c);
}

void display_task(void *arg) {
    SHVAL_ErrorTypeDef shval_err;
    uint32_t enc_value;
    if ((shval_err = SHVAL_GetValue(&gAppState.SharedValues.EncValue, &enc_value, pdMS_TO_TICKS(1000))) == SHVAL_ERROR_OK) {
        // display_power(enc_value > 100 ? 100 : enc_value);
    } else {
        ERROR_Trigger(ERROR_ENCODER_SHVAL_READ);
    }

    while (1) {
        if (xEventGroupWaitBits(gAppState.SharedValues.EncValue.EventGroup, SHVAL_EVT_BITS_ENC_VALUE_DISPLAY, pdTRUE, pdFALSE, portMAX_DELAY)) {
            if ((shval_err = SHVAL_GetValue(&gAppState.SharedValues.EncValue, &enc_value, pdMS_TO_TICKS(1000))) == SHVAL_ERROR_OK) {
                display_power(enc_value > 100 ? 100 : enc_value);
            } else {
                ERROR_Trigger(ERROR_ENCODER_SHVAL_READ);
            }
        }
    }
}
