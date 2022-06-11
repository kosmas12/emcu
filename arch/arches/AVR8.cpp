//
// Created by kosmas on 13/5/2022.
//

#include "AVR8.h"

std::vector<uint8_t> getDefaultAVR8Registers() {
    std::vector<uint8_t> registers;
    // 32 general purpose registers seems to be a good default
    // Also PC, SP and Status
    for (int i = 0; i < 35; ++i) {
        registers.push_back(0);
    }
    return registers;
}

void AVRExecuteNext(MCU *mcu) {
    //TODO: Actual execution
}
