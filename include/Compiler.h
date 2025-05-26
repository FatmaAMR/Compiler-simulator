#pragma once
#include "lexer.h"
#include "parser.h"
#include "helpers.h"

class Compiler {
private:
  vector<Token> tokens;
  Lexer lexer;
  Parser parser;
  std::string sourceCode;
  std::ofstream out;

public:
  Compiler(std::string filename, std::string resultsname = "result.txt");
  int calcLexerErrorCount();
  void printLexerTokens();
  bool compile();
};
