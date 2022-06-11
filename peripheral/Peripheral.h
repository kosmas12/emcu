//
// Created by kosmas on 12/5/2022.
//
// This is a class to represent peripherals. Peripherals can be any device, really

#ifndef EMCU_PERIPHERAL_H
#define EMCU_PERIPHERAL_H


#include <string>
#include <vector>
#include "Terminal.h"
#include <functional>

class Peripheral {
    // Name, for convenience
    std::string name;
    // The function that actually makes it work
    std::function<void(void)> tickFunction;
    // Self-explanatory
    std::vector<Terminal> inputs;
    std::vector<Terminal> outputs;
};


#endif //EMCU_PERIPHERAL_H
