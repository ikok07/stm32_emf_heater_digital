//
// Created by Kok on 4/9/26.
//

#include "logging.h"

#include "log.h"

void init_logger();

void LOGGING_Init() {
    LOGGER_CallbacksTypeDef callbacks = {
        .on_init = init_logger
    };
    LOGGER_RegisterCB(&callbacks);
    LOGGER_Init();
}

void init_logger() {

}
