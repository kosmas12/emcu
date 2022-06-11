cmake_minimum_required(VERSION 3.22)
project(emcu)

set(CMAKE_CXX_STANDARD 20)

add_executable(emcu main.cpp board/mcu/MCU.cpp
        board/mcu/MCU.h
        board/Board.cpp
        board/Board.h
        arch/Architecture.cpp
        arch/Architecture.h
        peripheral/Peripheral.cpp
        peripheral/Peripheral.h
        peripheral/Terminal.cpp
        peripheral/Terminal.h
        arch/arches/AVR8.cpp arch/Register.h arch/Register.cpp)
