#include "helpers.h"
#include <fstream>
#include <sstream>
#include <iostream>

std::string readFile(const std::string &fileName) {
    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file \"" << fileName << "\"" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
