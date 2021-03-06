//
// Created by kosmas on 13/5/2022.
//

#ifndef EMCU_AVR8_H
#define EMCU_AVR8_H

#include "../../board/mcu/MCU.h"
#include "../Register.h"

#define STATUS_REGISTER 0
#define PROGRAM_COUNTER 1
#define STACK_POINTER 2

#define MAIN_MEMORY 0
#define PROGRAM_MEMORY 1
#define EEPROM_MEMORY 2
#define IO_MEMORY 3

std::vector<Register> getDefaultAVR8Registers();

void AVR8ExecuteNext(MCU *mcu);

#endif //EMCU_AVR8_H
