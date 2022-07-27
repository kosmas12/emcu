//
// Created by kosmas on 12/5/2022.
//

#include <iostream>
#include "Architecture.h"
#include "arches/AVR8.h"

void Architecture::getDefaultRegisters() {
    if (this->name == "AVR8") {
        this->defaultRegisters = getDefaultAVR8Registers();
    }
}

Architecture::Architecture(std::string name) {
    this->name = name;
    this->getDefaultRegisters();
    if (name == "AVR8") {
        this->bigEndian = false;
    }
}
