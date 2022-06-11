#include <iostream>
#include "arch/arches/AVR8.h"

int main() {
    Architecture arch = Architecture("AVR8");
    std::vector<uint8_t> registers;
    std::vector<std::vector<uint8_t>> memories;
    std::vector<Peripheral> peripherals;
    MCU someMCU = MCU(arch, registers, memories, peripherals, AVRExecuteNext);

    return 0;
}
