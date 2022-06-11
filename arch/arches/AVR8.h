//
// Created by kosmas on 13/5/2022.
//

#ifndef EMCU_AVR8_H
#define EMCU_AVR8_H

#include "../../board/mcu/MCU.h"

#define STATUS_REGISTER 0
#define PROGRAM_COUNTER 1
#define STACK_POINTER 2

std::vector<uint8_t> getDefaultAVR8Registers();

void AVRExecuteNext(MCU *mcu);

#endif //EMCU_AVR8_H
