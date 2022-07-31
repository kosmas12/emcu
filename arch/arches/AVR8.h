//
// Created by kosmas on 13/5/2022.
//

#ifndef EMCU_AVR8_H
#define EMCU_AVR8_H

#include "../../board/mcu/MCU.h"
#include "../Register.h"

#define STATUS_REGISTER 32
#define PROGRAM_COUNTER 33
#define STACK_POINTER 34

#define MAIN_MEMORY 0
#define PROGRAM_MEMORY 1
#define IO_MEMORY 2

#define CARRY_FLAG 0
#define ZERO_FLAG 1
#define NEGATIVE_FLAG 2
#define TWOS_COMPLEMENT_OVERFLOW_FLAG 3
#define SIGN_FLAG 4
#define HALF_CARRY_FLAG 5
#define TRANSFER_BIT 6
#define GLOBAL_INTERRUPT_ENABLE_BIT 7

std::vector<Register> getDefaultAVR8Registers();

void AVR8ExecuteNext(MCU *mcu);

#endif //EMCU_AVR8_H
