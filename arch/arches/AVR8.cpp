//
// Created by kosmas on 13/5/2022.
//

#include <iostream>
#include "AVR8.h"

/* Flags are calculated as follows (R with no number = Result):
 * Half Carry flag (H): Rd3 AND Rr3 OR Rr3 AND NOT R3 OR NOT R3 AND Rd3
 * Two's Complement Overflow Flag (V): Rd7 AND Rr7 AND NOT R7 OR NOT Rd7 AND NOT Rr7 AND R7
 * Negative flag (N): R7
 * Sign flag: N XOR V
 * Zero flag (Z): NOT R7 AND NOT R6 AND NOT R5 AND NOT R4 AND NOT R3 AND NOT R2 AND NOT R1 AND NOT R0 (We're not really going to do that)
 * Carry flag (C): Rd7 AND Rr7 OR Rr7 AND NOT R7 OR NOT R7 AND Rd7
 */

inline uint8_t getBit(uint8_t value, uint8_t position) {
    return (value & (1 << position)) >> position;
}

inline void setBit(uint8_t *value, uint8_t position, bool set) {
    if (set) {
        *value |= 1 << position;
    }
    else {
        *value &= ~(1 << position);
    }
}

inline uint8_t getStatus(uint8_t flag, MCU *mcu) {
    return getBit(mcu->readRegister8bits(STATUS_REGISTER), flag);
}

void addWithCarry(uint16_t instruction, MCU *mcu) {
    uint8_t Rd = (instruction & 0x1F0) >> 4;
    uint8_t Rr = ((instruction & 0x200) >> 5) | (instruction & 0xF);

    uint8_t RdContents = mcu->readRegister8bits(Rd);
    uint8_t RrContents = mcu->readRegister8bits(Rr);
    uint8_t carry = getStatus(CARRY_FLAG, mcu);

    uint8_t result = RdContents + RrContents + carry;
    mcu->writeRegister8bits(Rd, result);

    uint8_t checkPassed;
    uint8_t finalValue = 0;

    uint8_t dTemp = getBit(RdContents, 3);
    uint8_t rTemp = getBit(RrContents, 3);
    uint8_t RTemp = getBit(result, 3);
    checkPassed = dTemp && rTemp || rTemp && !RTemp || !RTemp && dTemp;
    setBit(&finalValue, HALF_CARRY_FLAG, checkPassed);

    dTemp = getBit(RdContents, 7);
    rTemp = getBit(RrContents, 7);
    RTemp = getBit(result, 7);
    checkPassed = dTemp && rTemp && !RTemp || !dTemp && rTemp && RTemp;
    setBit(&finalValue, TWOS_COMPLEMENT_OVERFLOW_FLAG, checkPassed);

    checkPassed = RTemp;
    setBit(&finalValue, NEGATIVE_FLAG, checkPassed);

    checkPassed = getBit(finalValue, NEGATIVE_FLAG) ^ getBit(finalValue, TWOS_COMPLEMENT_OVERFLOW_FLAG);
    setBit(&finalValue, SIGN_FLAG, checkPassed);

    setBit(&finalValue, ZERO_FLAG, result == 0);

    checkPassed = dTemp && rTemp || rTemp && !RTemp || !RTemp && dTemp;
    setBit(&finalValue, CARRY_FLAG, checkPassed);

    mcu->writeRegister8bits(STATUS_REGISTER, finalValue);

    uint32_t programCounter = mcu->readRegister32bits(PROGRAM_COUNTER);
    mcu->writeRegister32bits(PROGRAM_COUNTER, programCounter + 1);
}

void exclusiveOR(uint16_t instruction, MCU *mcu) {
    uint8_t Rd = (instruction & 0x1F0) >> 4;
    uint8_t Rr = ((instruction & 0x200) >> 5) | (instruction & 0xF);

    uint8_t RdContents = mcu->readRegister8bits(Rd);
    uint8_t RrContents = mcu->readRegister8bits(Rr);

    uint8_t result = RdContents ^ RrContents;
    mcu->writeRegister8bits(Rd, result);

    uint8_t finalValue = mcu->readRegister8bits(STATUS_REGISTER);

    setBit(&finalValue, TWOS_COMPLEMENT_OVERFLOW_FLAG, false);
    setBit(&finalValue, NEGATIVE_FLAG, getBit(result, 7));
    setBit(&finalValue, ZERO_FLAG, result == 0);
    setBit(&finalValue, SIGN_FLAG, (getBit(result, 7) ^ 0));
    mcu->writeRegister8bits(STATUS_REGISTER, finalValue);

    uint32_t programCounter = mcu->readRegister32bits(PROGRAM_COUNTER);
    mcu->writeRegister32bits(PROGRAM_COUNTER, programCounter + 1);

}

void jump(uint16_t instruction, MCU *mcu) {
    uint32_t programCounter = mcu->readRegister32bits(PROGRAM_COUNTER);
    uint16_t secondWord = htobe16(mcu->readMemory16bits(PROGRAM_MEMORY, ++programCounter, false));
    uint32_t fullInstruction = instruction << 16 | secondWord;
    uint32_t addressToJumpTo = (((fullInstruction & 0x1000000) >> 3) | ((fullInstruction & 0xF00000) >> 3) | ((fullInstruction & 0x10000) << 3) | (fullInstruction & 0xFFFF)) << 1;

    mcu->writeRegister32bits(PROGRAM_COUNTER, addressToJumpTo);
}

std::vector<Register> getDefaultAVR8Registers() {
    std::vector<Register> registers;
    // 32 general purpose registers seems to be a good default
    // Also PC, SP and Status

    Register empty8bitRegister = Register(1);
    for (int i = 0; i < 33; ++i) {
        registers.push_back(empty8bitRegister);
    }
    registers.emplace_back(Register(4));
    registers.emplace_back(Register(2));

    return registers;
}

void AVR8ExecuteNext(MCU *mcu) {
    uint32_t programCounter = mcu->readRegister32bits(PROGRAM_COUNTER);
    // The AVR Instruction Manual uses big endian notation for the opcodes, so this is easier
    uint16_t instruction = htobe16(mcu->readMemory16bits(PROGRAM_MEMORY, programCounter, false));
    mcu->writeRegister32bits(PROGRAM_COUNTER, programCounter + 1);

    switch (instruction & 0xFC00) {
        case 0x1C00:
            addWithCarry(instruction, mcu);
            break;
        case 0x2400:
            exclusiveOR(instruction, mcu);
            break;
        case 0x9400:
            jump(instruction, mcu);
            break;
        default:
            std::cerr << "Unimplemented AVR8 instruction " << std::hex << instruction << std::endl;
            exit(2);
            break;
    }
}
