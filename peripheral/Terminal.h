//
// Created by kosmas on 12/5/2022.
//
// This is a class to represent a simple terminal

#ifndef EMCU_TERMINAL_H
#define EMCU_TERMINAL_H


#include <cstdint>
#include <string>

typedef enum {
    TERMINALTYPE_POWER,
    TERMINALTYPE_ANALOG,
    TERMINALTYPE_DIGITAL
}TerminalType;

class Terminal {
    TerminalType type;
    // For convenience purposes
    std::string name;
    // Current value
    uint8_t value;
};


#endif //EMCU_TERMINAL_H
