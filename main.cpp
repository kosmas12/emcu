#include <iostream>
#include <cstring>
#include <sstream>
#include "arch/arches/AVR8.h"

int main(int argc, char **argv) {
    Architecture arch = Architecture("AVR8");
    std::vector<std::vector<uint8_t>> memories;
    std::vector<uint8_t> programMemory (32000 * 16);
    std::vector<uint8_t> dataMemory (8000);

    memories.push_back(dataMemory);
    memories.push_back(programMemory);
    
    std::vector<Peripheral> peripherals;
    MCU someMCU = MCU(arch, arch.defaultRegisters, memories, peripherals, AVR8ExecuteNext);

    bool interactive = false;
    if (argc > 1) {
        if (strcmp(argv[1], "interactive") == 0) {
            interactive = true;
        }
    }

    if (interactive) {
        uint16_t opcode;
        std::string input;
        std::stringstream stream;
        while (input != "quit") {
            std::getline(std::cin, input);
            stream << input;
            stream >> std::hex >> opcode;
            stream.clear();
            uint16_t programCounter = someMCU.readRegister16bits(PROGRAM_COUNTER);
            someMCU.writeMemory16bits(PROGRAM_MEMORY, programCounter, opcode, someMCU.arch.bigEndian);
            someMCU.executeNext(&someMCU);
        }
    }

    return 0;
}
