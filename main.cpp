#include <iostream>
#include <cstring>
#include <sstream>
#include <fstream>
#include <iterator>
#include "arch/arches/AVR8.h"

void loadIHEX(MCU *mcu, std::ifstream file) {
    std::string line;

    while (std::getline(file, line)) {
        std::stringstream stream;
        uint8_t numBytes;
        stream << line[1] << line[2];
        numBytes = std::stoi(stream.str(), nullptr, 16);
        stream.str(std::string());

        uint16_t address;
        stream << line[3] << line[4] << line[5] << line[6];
        address = std::stoi(stream.str(), nullptr, 16);
        stream.str(std::string());

        uint8_t recordType;
        stream << line[7] << line[8];
        recordType = std::stoi(stream.str(), nullptr, 16);
        stream.str(std::string());

        std::vector<uint8_t> data;
        for (int i = 0; i < numBytes * 2; i += 2) {
            stream << line[9 + i] << line[9 + i + 1];
            std::string byteData = stream.str();
            uint8_t byte = std::stoi(byteData, nullptr, 16);
            stream.str(std::string());
            data.emplace_back(byte);
        }

        uint8_t checksum;
        stream << line[9 + (numBytes * 2)] << line[9 + (numBytes * 2) + 1];
        checksum = std::stoi(stream.str(), nullptr, 16);
        stream.str(std::string());

        // Checksum is the bytes of the line added together, and then the two's complement of that
        uint8_t calculatedChecksum = 0;
        calculatedChecksum += numBytes;
        uint8_t *addressBytes = (uint8_t *)(&address);
        calculatedChecksum += addressBytes[0] + addressBytes[1];
        calculatedChecksum += recordType;
        for (auto byte : data) {
            calculatedChecksum += byte;
        }
        calculatedChecksum = (~calculatedChecksum) + 1;
        if (calculatedChecksum != checksum) {
            std::cerr << "Invalid record in Intel HEX file. Please make sure the data is correct." << std::endl;
            exit(1);
        }
        else {
            for (auto byte : data) {
                mcu->writeMemory8bits(PROGRAM_MEMORY, address++, byte);
            }
        }
    }
}

int main(int argc, char **argv) {
    Architecture arch = Architecture("AVR8");
    std::vector<std::vector<uint8_t>> memories;
    std::vector<uint8_t> mainMemory (8000);
    std::vector<uint8_t> programMemory (32000 * 16);
    std::vector<uint8_t> ioMemory(64);

    memories.push_back(mainMemory);
    memories.push_back(programMemory);
    memories.push_back(ioMemory);
    
    std::vector<Peripheral> peripherals;
    MCU someMCU = MCU(arch, arch.defaultRegisters, memories, peripherals, AVR8ExecuteNext);

    bool interactive = false;
    if (argc > 1) {
        if (strcmp(argv[1], "interactive") == 0) {
            interactive = true;
        }
        else {
            std::ifstream program(argv[1], std::ios::in | std::ios::binary);
            std::string firstLine;
            std::getline(program, firstLine);
            // Check if the first line starts with a HEX record start character
            if (!firstLine.empty() && firstLine[0] == ':') {
                // If yes, this is a HEX file
                program.seekg(std::ios::beg);
                loadIHEX(&someMCU, std::move(program));
            }
            else {
                // TODO: Maybe binary? More research for commonly used formats needed
            }
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
            uint32_t programCounter = someMCU.readRegister32bits(PROGRAM_COUNTER);
            someMCU.writeMemory16bits(PROGRAM_MEMORY, programCounter, opcode, someMCU.arch.bigEndian);
            someMCU.executeNext(&someMCU);
        }
    }
    else {
        while (true) {
            someMCU.executeNext(&someMCU);
        }
    }

    return 0;
}
