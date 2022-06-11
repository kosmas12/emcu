#include <iostream>
#include "arch/arches/AVR8.h"

int main(int argc, char **argv) {
    Architecture arch = Architecture("AVR8");
    std::vector<Register> registers;
    std::vector<std::vector<uint8_t>> memories;
    std::vector<uint8_t> programMemory (32000 * 16);
    std::vector<uint8_t> dataMemory (8000);

    memories.push_back(dataMemory);
    memories.push_back(programMemory);
    
    std::vector<Peripheral> peripherals;
    MCU someMCU = MCU(arch, arch.defaultRegisters, memories, peripherals, AVR8ExecuteNext);

    someMCU.executeNext(&someMCU);

    return 0;
}
