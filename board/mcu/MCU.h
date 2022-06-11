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
    std::vector<uint8_t> registers;
    std::vector<std::vector<uint8_t>> memories;
    std::vector<Peripheral> peripherals;
    std::string name;

    MCU();
    MCU(Architecture architecture);
    MCU(Architecture arch, std::vector<uint8_t> registers);
    MCU(Architecture arch, std::vector<uint8_t> registers, std::vector<std::vector<uint8_t>> memories);
    MCU(Architecture arch, std::vector<uint8_t> registers, std::vector<std::vector<uint8_t>> memories,
        std::vector<Peripheral> peripherals);
    MCU(Architecture arch, std::vector<uint8_t> registers, std::vector<std::vector<uint8_t>> memories,
        std::vector<Peripheral> peripherals, const std::function<void(MCU *)>& executeNext);
};


#endif //EMCU_MCU_H
