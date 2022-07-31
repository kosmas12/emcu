//
// Created by kosmas on 10/5/2022.
//
// This is a class that represents a Micro Controller Unit (MCU)

#ifndef EMCU_MCU_H
#define EMCU_MCU_H

#include <vector>
#include <cstdint>
#include <functional>
#include "../../arch/Architecture.h"
#include "../../peripheral/Peripheral.h"

class MCU {
public:
    std::function<void(MCU *)> executeNext;
    Architecture arch;
    std::vector<Register> registers;
    std::vector<std::vector<uint8_t>> memories;
    std::vector<Peripheral> peripherals;
    std::string name;

    uint8_t readRegister8bits(uint8_t registerNumber);
    uint16_t readRegister16bits(uint8_t registerNumber);

    void writeRegister8bits(uint8_t registerNumber, uint8_t value);
    void writeRegister16bits(uint8_t registerNumber, uint16_t value);

    uint32_t readRegister32bits(uint8_t registerNumber);
    void writeRegister32bits(uint8_t registerNumber, uint32_t value);

    uint8_t readMemory8bits(uint8_t type, uint32_t location);
    uint16_t readMemory16bits(uint8_t type, uint32_t location, bool bigEndian);

    void writeMemory8bits(uint8_t type, uint32_t location, uint8_t value);
    void writeMemory16bits(uint8_t type, uint32_t location, uint16_t value, bool bigEndian);

    MCU();
    MCU(Architecture architecture);
    MCU(Architecture arch, std::vector<Register> registers);
    MCU(Architecture arch, std::vector<Register> registers, std::vector<std::vector<uint8_t>> memories);
    MCU(Architecture arch, std::vector<Register> registers, std::vector<std::vector<uint8_t>> memories,
        std::vector<Peripheral> peripherals);
    MCU(Architecture arch, std::vector<Register> registers, std::vector<std::vector<uint8_t>> memories,
        std::vector<Peripheral> peripherals, const std::function<void(MCU *)>& executeNext);

};


#endif //EMCU_MCU_H
