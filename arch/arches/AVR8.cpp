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
    uint8_t valueForStatus = 0;

    uint8_t dTemp = getBit(RdContents, 3);
    uint8_t rTemp = getBit(RrContents, 3);
    uint8_t RTemp = getBit(result, 3);
    checkPassed = dTemp && rTemp || rTemp && !RTemp || !RTemp && dTemp;
    setBit(&valueForStatus, HALF_CARRY_FLAG, checkPassed);

    dTemp = getBit(RdContents, 7);
    rTemp = getBit(RrContents, 7);
    RTemp = getBit(result, 7);
    checkPassed = dTemp && rTemp && !RTemp || !dTemp && rTemp && RTemp;
    setBit(&valueForStatus, TWOS_COMPLEMENT_OVERFLOW_FLAG, checkPassed);

    checkPassed = RTemp;
    setBit(&valueForStatus, NEGATIVE_FLAG, checkPassed);

    checkPassed = getBit(valueForStatus, NEGATIVE_FLAG) ^ getBit(valueForStatus, TWOS_COMPLEMENT_OVERFLOW_FLAG);
    setBit(&valueForStatus, SIGN_FLAG, checkPassed);

    setBit(&valueForStatus, ZERO_FLAG, result == 0);

    checkPassed = dTemp && rTemp || rTemp && !RTemp || !RTemp && dTemp;
    setBit(&valueForStatus, CARRY_FLAG, checkPassed);

    mcu->writeRegister8bits(STATUS_REGISTER, valueForStatus);

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

    uint8_t valueForStatus = mcu->readRegister8bits(STATUS_REGISTER);

    setBit(&valueForStatus, TWOS_COMPLEMENT_OVERFLOW_FLAG, false);
    setBit(&valueForStatus, NEGATIVE_FLAG, getBit(result, 7));
    setBit(&valueForStatus, ZERO_FLAG, result == 0);
    setBit(&valueForStatus, SIGN_FLAG, (getBit(result, 7) ^ 0));
    mcu->writeRegister8bits(STATUS_REGISTER, valueForStatus);

    uint32_t programCounter = mcu->readRegister32bits(PROGRAM_COUNTER);
    mcu->writeRegister32bits(PROGRAM_COUNTER, programCounter + 1);

}

void compareWithImmediate(uint16_t instruction, MCU *mcu) {
    uint8_t immediate = ((instruction & 0xF00) >> 4) | (instruction & 0xF);
    uint8_t Rd = (instruction & 0xF0) >> 4;
    uint8_t RdContents = mcu->readRegister8bits(Rd);
    uint8_t result = RdContents - immediate;

    uint8_t iTemp = getBit(immediate, 3);
    uint8_t dTemp = getBit(RdContents, 3);
    uint8_t RTemp = getBit(result, 3);

    uint8_t valueForStatus = mcu->readRegister8bits(STATUS_REGISTER);

    uint8_t checkPassed = !dTemp && iTemp || iTemp && RTemp || RTemp && !dTemp;
    setBit(&valueForStatus, HALF_CARRY_FLAG, checkPassed);

    iTemp = getBit(immediate, 7);
    dTemp = getBit(RdContents, 7);
    RTemp = getBit(result, 7);

    checkPassed = dTemp && !iTemp && !RTemp || dTemp && iTemp && RTemp;
    setBit(&valueForStatus, TWOS_COMPLEMENT_OVERFLOW_FLAG, checkPassed);

    setBit(&valueForStatus, NEGATIVE_FLAG, getBit(result, 7));
    setBit(&valueForStatus, ZERO_FLAG, result == 0);

    checkPassed = !dTemp && iTemp || iTemp && RTemp || RTemp && dTemp;
    setBit(&valueForStatus, CARRY_FLAG, checkPassed);

    checkPassed = getBit(valueForStatus, NEGATIVE_FLAG) ^ getBit(valueForStatus, TWOS_COMPLEMENT_OVERFLOW_FLAG);
    setBit(&valueForStatus, SIGN_FLAG, checkPassed);

    mcu->writeRegister8bits(STATUS_REGISTER, valueForStatus);
    uint32_t programCounter = mcu->readRegister32bits(PROGRAM_COUNTER);
    mcu->writeRegister32bits(PROGRAM_COUNTER, programCounter + 1);
}

void orImmediate(uint16_t instruction, MCU *mcu) {
    uint8_t Rd = (instruction & 0xF0) >> 4;
    uint8_t RdContents = mcu->readRegister8bits(Rd);
    uint8_t immediate = ((instruction & 0xF00) >> 4) | (instruction & 0xF);

    uint8_t result = RdContents | immediate;
    mcu->writeRegister8bits(Rd, result);

    uint8_t status = mcu->readRegister8bits(STATUS_REGISTER);
    setBit(&status, TWOS_COMPLEMENT_OVERFLOW_FLAG, false);

    uint8_t RTemp = getBit(result, 7);
    setBit(&status, NEGATIVE_FLAG, RTemp);
    setBit(&status, ZERO_FLAG, result == 0);
    setBit(&status, SIGN_FLAG, RTemp ^ 0);
    mcu->writeRegister8bits(STATUS_REGISTER, status);

    uint32_t programCounter = mcu->readRegister32bits(PROGRAM_COUNTER);
    mcu->writeRegister32bits(PROGRAM_COUNTER, programCounter + 1);
}

void lds(uint16_t instruction, MCU *mcu) {
    uint8_t Rd = (instruction & 0x1F0) >> 4;

    uint32_t programCounter = mcu->readRegister32bits(PROGRAM_COUNTER);
    uint16_t dataSpaceAddress = htobe16(mcu->readMemory16bits(PROGRAM_MEMORY, ++programCounter, false));

    uint8_t value = mcu->readMemory8bits(MAIN_MEMORY, dataSpaceAddress);
    mcu->writeRegister8bits(Rd, value);

    mcu->writeRegister32bits(PROGRAM_COUNTER, programCounter + 2);
}

void sts(uint16_t instruction, MCU *mcu) {
    uint8_t Rd = (instruction & 0x1F0) >> 4;
    uint8_t RdContents = mcu->readRegister8bits(Rd);

    uint32_t programCounter = mcu->readRegister32bits(PROGRAM_COUNTER);
    uint16_t dataSpaceAddress = htobe16(mcu->readMemory16bits(PROGRAM_MEMORY, ++programCounter, false));

    uint8_t value = mcu->readMemory8bits(MAIN_MEMORY, dataSpaceAddress);
    mcu->writeMemory8bits(MAIN_MEMORY, dataSpaceAddress, RdContents);

    mcu->writeRegister32bits(PROGRAM_COUNTER, programCounter + 2);
}

void compareWithCarry(uint16_t instruction, MCU *mcu) {
    uint8_t Rd = (instruction & 0x1F0) >> 4;
    uint8_t Rr = ((instruction & 0x200) >> 5) | (instruction & 0xF);

    uint8_t RdContents = mcu->readRegister8bits(Rd);
    uint8_t RrContents = mcu->readRegister8bits(Rr);
    uint8_t status = mcu->readRegister8bits(STATUS_REGISTER);
    uint8_t carryFlag = status & 1;

    uint8_t result = RdContents - RrContents - carryFlag;

    uint8_t rTemp = getBit(RrContents, 3);
    uint8_t dTemp = getBit(RdContents, 3);
    uint8_t RTemp = getBit(result, 3);

    uint8_t checkPassed = !dTemp && rTemp || rTemp && RTemp || RTemp && !dTemp;
    setBit(&status, HALF_CARRY_FLAG, checkPassed);

    rTemp = getBit(Rr, 7);
    dTemp = getBit(Rd, 7);
    RTemp = getBit(result, 7);
    checkPassed = dTemp && !rTemp && !RTemp || !dTemp && rTemp && RTemp;
    setBit(&status, TWOS_COMPLEMENT_OVERFLOW_FLAG, checkPassed);

    setBit(&status, NEGATIVE_FLAG, getBit(result, 7));
    setBit(&status, ZERO_FLAG, result == 0);

    checkPassed = getBit(status, TWOS_COMPLEMENT_OVERFLOW_FLAG) ^ getBit(status, NEGATIVE_FLAG);
    setBit(&status, SIGN_FLAG, checkPassed);

    checkPassed = !dTemp && rTemp || rTemp && RTemp || RTemp && dTemp;
    setBit(&status, CARRY_FLAG, checkPassed);

    mcu->writeRegister8bits(STATUS_REGISTER, status);
    uint32_t programCounter = mcu->readRegister32bits(PROGRAM_COUNTER);
    mcu->writeRegister32bits(PROGRAM_COUNTER, programCounter + 1);
}

void jump(uint16_t instruction, MCU *mcu) {
    uint32_t programCounter = mcu->readRegister32bits(PROGRAM_COUNTER);
    uint16_t secondWord = htobe16(mcu->readMemory16bits(PROGRAM_MEMORY, ++programCounter, false));
    uint32_t addressToJumpTo = (((instruction & 0x1F0) << 16) | (instruction & 0x1 << 19) | (secondWord << 3) >> 3) << 1;

    mcu->writeRegister32bits(PROGRAM_COUNTER, addressToJumpTo);
}

void call(uint16_t instruction, MCU *mcu) {
    uint32_t programCounter = mcu->readRegister32bits(PROGRAM_COUNTER);
    uint16_t secondWord = htobe16(mcu->readMemory16bits(PROGRAM_MEMORY, ++programCounter, false));
    uint32_t callAddress = (((instruction & 0x1F0) << 16) | (instruction & 0x1 << 19) | (secondWord << 3) >> 3) << 1;

    uint32_t stackPointer = mcu->readRegister16bits(STACK_POINTER);
    // For devices with a 22-bit PC (instead of 16), more data is pushed to the stack
    if (mcu->registers[PROGRAM_COUNTER].size > 2) {
        uint32_t value = programCounter + 2;
        mcu->writeMemory8bits(MAIN_MEMORY, stackPointer, value & 0xFF);
        mcu->writeMemory8bits(MAIN_MEMORY, stackPointer - 1, (value & 0xFF00) >> 8);
        mcu->writeMemory8bits(MAIN_MEMORY, stackPointer - 2, (value & 0x3F0000) >> 16);
        mcu->writeRegister16bits(STACK_POINTER, stackPointer - 3);
    }
    else {
        mcu->writeMemory16bits(MAIN_MEMORY, stackPointer, (uint16_t)(programCounter + 2), false);
        mcu->writeRegister16bits(STACK_POINTER, stackPointer - 2);
    }

    mcu->writeRegister32bits(PROGRAM_COUNTER, callAddress);
}

void sbiw(uint16_t instruction, MCU *mcu) {
    uint8_t immediate = (instruction & 0xF) | ((instruction & 0xC00) >> 4);
    uint8_t pairBeginnings[4] = {24, 26, 28, 30};
    uint8_t pair = (instruction & 0x30) >> 4;

    uint8_t firstRegister = pairBeginnings[pair];
    uint8_t secondRegister = firstRegister + 1;

    // AVR is little endian, so the lower register holds the lower byte
    uint16_t value = mcu->readRegister8bits(firstRegister);
    value = (value | (mcu->readRegister8bits(secondRegister) << 8));

    uint16_t result = value - immediate;

    mcu->writeRegister8bits(firstRegister, (result & 0xFF));
    mcu->writeRegister8bits(secondRegister, result >> 8);

    uint32_t programCounter = mcu->readRegister32bits(PROGRAM_COUNTER);
    mcu->writeRegister32bits(PROGRAM_COUNTER, programCounter + 1);

}

void sei(uint16_t instruction, MCU *mcu) {
    uint32_t programCounter = mcu->readRegister32bits(PROGRAM_COUNTER);

    uint8_t status = mcu->readRegister8bits(STATUS_REGISTER);
    setBit(&status, GLOBAL_INTERRUPT_ENABLE_BIT, true);
    mcu->writeRegister8bits(STATUS_REGISTER, status);

    mcu->writeRegister32bits(PROGRAM_COUNTER, programCounter + 1);
}

void in(uint16_t instruction, MCU *mcu) {
    uint8_t Rd = (instruction & 0x1F0) >> 4;
    uint8_t ioSpaceAddress = ((instruction & 0x600) >> 5) | (instruction & 0xF);

    mcu->writeRegister8bits(Rd, mcu->readMemory8bits(IO_MEMORY, ioSpaceAddress));

    uint32_t programCounter = mcu->readRegister32bits(PROGRAM_COUNTER);
    mcu->writeRegister32bits(PROGRAM_COUNTER, programCounter + 1);
}

void out(uint16_t instruction, MCU *mcu) {
    uint8_t ioSpaceAddress = ((instruction & 0x600) >> 5) | (instruction & 0xF);
    uint8_t Rr = (instruction & 0x1F0) >> 4;

    uint8_t RrContents = mcu->readRegister8bits(Rr);
    mcu->writeMemory8bits(IO_MEMORY, ioSpaceAddress, RrContents);

    uint32_t programCounter = mcu->readRegister32bits(PROGRAM_COUNTER);
    mcu->writeRegister32bits(PROGRAM_COUNTER, programCounter + 1);
}

void relativeJump(uint16_t instruction, MCU *mcu) {
    int16_t offset = (int16_t) (instruction & 0xFFF);

    uint32_t programCounter = mcu->readRegister32bits(PROGRAM_COUNTER);
    mcu->writeRegister32bits(PROGRAM_COUNTER, programCounter + ((int16_t)offset * 2) + 1);
}

void ldi(uint16_t instruction, MCU *mcu) {
    uint8_t Rd = (instruction & 0xF0) >> 4;
    uint8_t immediate = ((instruction & 0xF00) >> 4) | (instruction & 0xF);

    mcu->writeRegister8bits(Rd, immediate);

    uint32_t programCounter = mcu->readRegister32bits(PROGRAM_COUNTER);
    mcu->writeRegister32bits(PROGRAM_COUNTER, programCounter + 1);
}

void branchIfNotEqual(uint16_t instruction, MCU *mcu) {
    uint8_t status = mcu->readRegister8bits(STATUS_REGISTER);
    uint8_t zeroFlag = getBit(status, ZERO_FLAG);

    uint32_t programCounter = mcu->readRegister32bits(PROGRAM_COUNTER);
    uint32_t newPC = programCounter + 1;

    if (!zeroFlag) {
        int8_t offset = (int8_t)((instruction >> 3) * 2);
        newPC += offset;
    }
    mcu->writeRegister32bits(PROGRAM_COUNTER, newPC);
}

void branchIfEqual(uint16_t instruction, MCU *mcu) {
    uint8_t status = mcu->readRegister8bits(STATUS_REGISTER);
    uint8_t zeroFlag = getBit(status, ZERO_FLAG);

    uint32_t programCounter = mcu->readRegister32bits(PROGRAM_COUNTER);
    uint32_t newPC = programCounter + 1;

    if (zeroFlag) {
        int8_t offset = (int8_t)((instruction >> 3) * 2);
        newPC += offset;
    }
    mcu->writeRegister32bits(PROGRAM_COUNTER, newPC);
}

std::vector<Register> getDefaultAVR8Registers() {
    std::vector<Register> registers;
    // 32 general purpose registers seems to be a good default
    // Also Status (8 bits), PC (16 bits) anf SP (16 bits)

    Register empty8bitRegister = Register(1);
    registers.reserve(33);
    for (int i = 0; i < 33; ++i) {
        registers.push_back(empty8bitRegister);
    }
    registers.emplace_back(2);
    registers.emplace_back(2);

    return registers;
}

void AVR8ExecuteNext(MCU *mcu) {
    static uint32_t currentFrameClockCycles;
    uint32_t programCounter = mcu->readRegister32bits(PROGRAM_COUNTER);
    // The AVR Instruction Manual uses big endian notation for the opcodes, so this is easier
    uint16_t instruction = htobe16(mcu->readMemory16bits(PROGRAM_MEMORY, programCounter, false));
    mcu->writeRegister32bits(PROGRAM_COUNTER, programCounter + 1);

    switch (instruction & 0xFC00) {
	    case 0x400:
	        compareWithCarry(instruction, mcu);
	        break;
        case 0x1C00:
            addWithCarry(instruction, mcu);
            break;
        case 0x2400:
            exclusiveOR(instruction, mcu);
            break;
        case 0x3000:
        case 0x3400:
        case 0x3800:
        case 0x3C00:
            compareWithImmediate(instruction, mcu);
            break;
        case 0x6000:
        case 0x6400:
        case 0x6800:
        case 0x6C00:
            orImmediate(instruction, mcu);
            break;
        case 0x9000:
            if(((instruction & 0x200) >> 8) == 0) {
                lds(instruction, mcu);
            }
            else {
                sts(instruction, mcu);
            }
            break;
        case 0x9400:
            if (((instruction & 0x300) >> 8) != 0x3) {
                if ((instruction & 0b1110) == 0b1000) {
                    sei(instruction, mcu);
                }
                else if ((instruction & 0b1110) == 0b1100) {
                    jump(instruction, mcu);
                }
                else if ((instruction & 0b1110) == 0b1110) {
                    call(instruction, mcu);
                }
            }
            else {
                sbiw(instruction, mcu);
            }
            break;
        case 0xB000:
        case 0xB100:
        case 0xB200:
        case 0xB300:
        case 0xB400:
        case 0xB500:
        case 0xB600:
        case 0xB700:
            in(instruction, mcu);
            break;
        case 0xB800:
        case 0xBC00:
            out(instruction, mcu);
            break;
        case 0xC000:
        case 0xC400:
        case 0xC800:
        case 0xCC00:
            relativeJump(instruction, mcu);
            break;
        case 0xE000:
        case 0xE400:
        case 0xE800:
        case 0xEC00:
            ldi(instruction, mcu);
            break;
        case 0xF000:
            if ((instruction & 0b001) == 1) {
                branchIfEqual(instruction, mcu);
            }
            break;
        case 0xF400:
            branchIfNotEqual(instruction, mcu);
            break;
        default:
            std::cerr << "Unimplemented AVR8 instruction 0x" << std::hex << instruction << " at PC 0x" << std::hex << programCounter << std::endl;
            exit(2);
            break;
    }
}
