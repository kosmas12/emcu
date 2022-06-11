//
// Created by kosmas on 10/5/2022.
//

#include <iostream>
#include <utility>
#include "MCU.h"
#include "../../arch/arches/AVR8.h"

void dummyExecuteNext(MCU *mcu) {
    std::cout << "executeNext function not available on this MCU" << std::endl;
}

MCU::MCU() {
    this->arch = Architecture("AVR8");
    this->registers = this->arch.defaultRegisters;
    this->executeNext = AVRExecuteNext;
}

MCU::MCU(Architecture architecture) {
    this->arch = architecture;
    this->registers = this->arch.defaultRegisters;
    this->executeNext = dummyExecuteNext;
}

MCU::MCU(Architecture arch, std::vector<uint8_t> registers) {
    this->arch = arch;
    this->registers = std::move(registers);
    this->executeNext = dummyExecuteNext;
}

MCU::MCU(Architecture arch, std::vector<uint8_t> registers, std::vector<std::vector<uint8_t>> memories) {
    this->arch = arch;
    this->registers = registers;
    this->memories = memories;
    this->executeNext = dummyExecuteNext;
}

MCU::MCU(Architecture arch, std::vector<uint8_t> registers, std::vector<std::vector<uint8_t>> memories,
         std::vector<Peripheral> peripherals) {
    this->arch = arch;
    this->registers = registers;
    this->memories = memories;
    this->peripherals = peripherals;
    this->executeNext = dummyExecuteNext;
}

MCU::MCU(Architecture arch, std::vector<uint8_t> registers, std::vector<std::vector<uint8_t>> memories,
         std::vector<Peripheral> peripherals, const std::function<void(MCU *)>& executeNext) {
    this->arch = arch;
    this->registers = registers;
    this->memories = memories;
    this->peripherals = peripherals;

    if (!executeNext) {
        std::cout << "NULL pointer passed as executeNext function, defaulting to dummy" << std::endl;
        this->executeNext = dummyExecuteNext;
    }
    else {
        this->executeNext = executeNext;
    }
}
