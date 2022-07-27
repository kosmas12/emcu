//
// Created by kosmas on 12/5/2022.
//

#ifndef EMCU_ARCHITECTURE_H
#define EMCU_ARCHITECTURE_H


#include <string>
#include <vector>
#include "Register.h"

class Architecture {
private:
    std::string name;
    void getDefaultRegisters();
public:
    std::vector<Register> defaultRegisters;
    bool bigEndian;
    Architecture(std::string name = "AVR8");
};


#endif //EMCU_ARCHITECTURE_H
