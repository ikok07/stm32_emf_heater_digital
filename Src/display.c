//
// Created by Kok on 6/4/26.
//

#include "display.h"

#include <string.h>

#include "app_state.h"
#include "ssd1306.h"

#define SSD1306_HEIGHT                          32

AppErrorTypeDef DISPLAY_Init() {
    uint8_t err;
    if ((err = ssd1306_Init(&gAppState.hi2c)) != 0) {
        return ERROR_DISPLAY_INIT;
    }

    char boot_msg[] = "Booting...";
    uint8_t msg_len = strlen(boot_msg);

    ssd1306_SetCursor((128 - msg_len * 7) / 2, (32 - 10) / 2);
    if ((err = ssd1306_WriteString(boot_msg, Font_7x10, White)) != 0) {
        return ERROR_DISPLAY_BOOT_MSG;
    };

    return ERROR_OK;
}
