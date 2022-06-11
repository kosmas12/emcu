//
// Created by kosmas on 11/6/2022.
//

#ifndef EMCU_REGISTER_H
#define EMCU_REGISTER_H


#include <cstdint>
#include <vector>

class Register {
public:
    uint8_t size;
    std::vector<uint8_t> bytes;

    Register(uint8_t size);
};


#endif //EMCU_REGISTER_H
