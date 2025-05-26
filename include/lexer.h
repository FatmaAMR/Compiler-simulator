#ifndef LEXER_H
#define LEXER_H

#include "Token.h"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Lexer {
public:
  Lexer() = default;
  explicit Lexer(const std::string &sourceCode);
  std::vector<Token> tokenize();

private:
  std::string source;
  size_t pos;
  int line;
  std::vector<Token> tokens;
  std::unordered_map<std::string, std::string> keywords;
  std::unordered_set<std::string> includedFiles;

  char peek(int n = 0);
  char get();
  void skipWhitespace();
  TokenType TokenTypeFromString(const std::string &typeStr);
  Token lexIdentifierOrKeyword();
  Token lexNumber();
  Token lexString();
  Token lexChar();
  Token lexOperatorOrPunctuation();
  std::vector<Token> lexComment();
};

#endif
