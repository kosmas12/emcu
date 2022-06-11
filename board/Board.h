//
// Created by kosmas on 10/5/2022.
//
// This is a class to represent a board that might have things external to the MCU.
// NOTE: This has not been implemented at all yet and has no support for now.

#ifndef EMCU_BOARD_H
#define EMCU_BOARD_H


#include "mcu/MCU.h"

class Board {
    MCU mcu;
    std::vector<std::vector<uint8_t>> externalMemories;
    std::vector<Peripheral> externalPeripherals;
};


#endif //EMCU_BOARD_H
