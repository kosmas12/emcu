//
// Created by kosmas on 10/5/2022.
//

#include <iostream>
#include <utility>
#include "MCU.h"
#include "../../arch/arches/AVR8.h"
#include <byteswap.h>
#include <cstring>

void dummyExecuteNext(MCU *mcu) {
    std::cout << "executeNext function not available on this MCU" << std::endl;
}

MCU::MCU() {
    this->arch = Architecture("AVR8");
    this->registers = this->arch.defaultRegisters;
    this->executeNext = AVR8ExecuteNext;
}

MCU::MCU(Architecture architecture) {
    this->arch = architecture;
    this->registers = this->arch.defaultRegisters;
    this->executeNext = dummyExecuteNext;
}

MCU::MCU(Architecture arch, std::vector<Register> registers) {
    this->arch = arch;
    this->registers = std::move(registers);
    this->executeNext = dummyExecuteNext;
}

MCU::MCU(Architecture arch, std::vector<Register> registers, std::vector<std::vector<uint8_t>> memories) {
    this->arch = arch;
    this->registers = registers;
    this->memories = memories;
    this->executeNext = dummyExecuteNext;
}

MCU::MCU(Architecture arch, std::vector<Register> registers, std::vector<std::vector<uint8_t>> memories,
         std::vector<Peripheral> peripherals) {
    this->arch = arch;
    this->registers = registers;
    this->memories = memories;
    this->peripherals = peripherals;
    this->executeNext = dummyExecuteNext;
}

MCU::MCU(Architecture arch, std::vector<Register> registers, std::vector<std::vector<uint8_t>> memories,
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

void MCU::writeRegister8bits(uint8_t registerNumber, uint8_t value) {
    if (this->registers.size() >= registerNumber + 1) {
        Register *registerToWrite = &this->registers[registerNumber];
        if (registerToWrite->size >= 1) {
            registerToWrite->bytes[0] = value;
        }
    }
}

uint8_t MCU::readRegister8bits(uint8_t registerNumber) {
    if (this->registers.size() >= registerNumber + 1) {
        Register registerToRead = this->registers[registerNumber];
        if (registerToRead.size >= 1) {
            return registerToRead.bytes[0];
        }
    }
    return 0;
}

uint16_t MCU::readRegister16bits(uint8_t registerNumber) {
    uint16_t word;
    if (this->registers.size() >= registerNumber + 1) {
        if (this->registers[registerNumber].size >= 2) {
            word = this->registers[registerNumber].bytes[0];
            word = (word << 8) | this->registers[registerNumber].bytes[1];
            return word;
        }
    }
    return 0;
}

void MCU::writeRegister16bits(uint8_t registerNumber, uint16_t value) {
    if (this->registers.size() >= registerNumber + 1) {
        Register *registerToWrite = &this->registers[registerNumber];
        if (registerToWrite->size >= 2) {
            registerToWrite->bytes[0] = value >> 8;
            registerToWrite->bytes[1] = value & 0xFF;
        }
    }
}

uint8_t MCU::readMemory8bits(uint8_t type, uint16_t location) {
    if (this->memories.size() >= type + 1) {
        return this->memories[type][location];
    }
    return 0;
}

void MCU::writeMemory8bits(uint8_t type, uint16_t location, uint8_t value) {
    if (this->memories.size() >= type + 1) {
        this->memories[type][location] = value;
    }
}

uint16_t MCU::readMemory16bits(uint8_t type, uint32_t location, bool bigEndian) {
    if (this->memories.size() >= type + 1) {
        uint16_t word = this->memories[type][location++];
        word = (word << 8) | this->memories[type][location];
        if (bigEndian) {
            return htobe16(word);
        }
        else {
            return htole16(word);
        }
    }
    return 0;
}

void MCU::writeMemory16bits(uint8_t type, uint32_t location, uint16_t value, bool bigEndian) {
    if (this->memories.size() >= type + 1) {
        if (this->memories[type].size() >= location + 1) {
            if (bigEndian) {
                value = htobe16(value);
            }
            else {
                value = htole16(value);
            }
            memcpy(&(this->memories[type][location]), &value, sizeof(value));
        }
    }
}
