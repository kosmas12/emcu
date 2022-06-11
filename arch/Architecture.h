//
// Created by kosmas on 12/5/2022.
//

#ifndef EMCU_ARCHITECTURE_H
#define EMCU_ARCHITECTURE_H


#include <string>
#include <vector>

class Architecture {
private:
    std::string name;
    void getDefaultRegisters();
public:
    std::vector<uint8_t> defaultRegisters;
    Architecture(std::string name = "AVR8");
};


#endif //EMCU_ARCHITECTURE_H
