#include "base64.hpp"


string base64encode(string input) {
    uint32_t* buffer = new uint32_t(0);
    int cycles = input.size() / 3;  // počet cyklů / trojic bytů zpracovávaných do byse64 čtveřice
    int padding = input.size() % 3;     // počet = na konci
    if (padding != 0) cycles++;
    const char* inp = input.c_str();
    int index;
    string output;
    for (int i = 0; i < cycles; i++) {
        *buffer = inp[i*3];
        *buffer = *buffer << 8;
        if (i*3+1 < input.size())
            *buffer = (*buffer | inp[i*3+1]) << 8;
        else
            *buffer = *buffer << 8;
        if (i*3+2 < input.size())
            *buffer = (*buffer | inp[i*3+2]) << 6;
        else
            *buffer = *buffer << 6;
        index = ((uint8_t*)buffer)[3];
        index = index & 0x3F;
        output += symboltable[index];
        for (int j = 0; j < 3; j++) {
            *buffer = *buffer << 6;
            index = ((uint8_t*)buffer)[3];
            index = index & 0x3F;
            if (i == cycles-1 && j >= 3-padding && index == 0) {
                output += "=";
            } else {
                output += symboltable[index];
            }
        }
        *buffer = 0;
    }
    return output;
}