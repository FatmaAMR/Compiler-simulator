#include "Lexer.h"
#include "Parser.h"
#include "helpers.h"
#include "Token.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>
#include <iomanip>
#include "Compiler.h"

using namespace std;

int main() {
    string fileName;
    cout << "Enter the file name: ";
    getline(cin, fileName);

    Compiler myCompiler(fileName);
    myCompiler.compile();
    return 0;
}
