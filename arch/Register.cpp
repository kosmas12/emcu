//
// Created by kosmas on 11/6/2022.
//

#include "Register.h"

Register::Register(uint8_t size) {
    this->size = size;

    for (int i = 0; i < size; ++i) {
        this->bytes.push_back(0);
    }

}
