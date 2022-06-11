//
// Created by kosmas on 13/5/2022.
//

#include <iostream>
#include "AVR8.h"

std::vector<Register> getDefaultAVR8Registers() {
    std::vector<Register> registers;
    // 32 general purpose registers seems to be a good default
    // Also PC, SP and Status
    registers.emplace_back(Register(2));
    registers.emplace_back(Register(2));


    Register empty8bitRegister = Register(1);
    for (int i = 2; i < 35; ++i) {
        registers.push_back(empty8bitRegister);
    }

    return registers;
}

void AVR8ExecuteNext(MCU *mcu) {
    uint16_t programCounter = mcu->readRegister16bits(PROGRAM_COUNTER);
    uint16_t instruction = mcu->readMemory16bits(PROGRAM_MEMORY, programCounter);

    switch (instruction) {
        default:
            std::cerr << "Unimplemented AVR8 instruction " << std::hex << instruction << std::endl;
            break;
    }

}
