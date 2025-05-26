#include "Compiler.h"
#include <iomanip>
#include <iostream>

Compiler::Compiler(std::string filename, std::string resultsname) {
  this->sourceCode = readFile(filename);
  this->out = ofstream(resultsname);
  this->lexer = Lexer(sourceCode);
  this->parser = Parser();
}

int Compiler::calcLexerErrorCount() {
  int err_count = 0;
  for (auto &&i : this->tokens) {
    err_count += (i.error) ? 1 : 0;
  }
  return err_count;
}

void Compiler::printLexerTokens() {
  std::cout << left << std::setw(8) << "Line" << "| " << std::setw(15)
            << "Lexeme" << "| "
            << "Token Type\n";
  std::cout << string(50, '-') << "\n";
  this->out << left << std::setw(8) << "Line" << "| " << std::setw(15)
            << "Lexeme" << "| "
            << "Token Type\n";
  this->out << string(50, '-') << "\n";

  int errorCount = 0;
  for (const auto &token : tokens) {
    string errorNote = token.error ? " (Error)" : "";
    std::cout << left << std::setw(8) << token.line << "| " << std::setw(15)
              << token.text << "| " << tokenTypeToString(token.type)
              << errorNote << "\n";
    out << left << std::setw(8) << token.line << "| " << std::setw(15)
        << token.text << "| " << tokenTypeToString(token.type) << errorNote
        << "\n";
    if (token.error)
      errorCount++;
  }

  cout << "\nTotal Number of lexical errors: " << errorCount << "\n";
  out << "\nTotal Number of lexical errors: " << errorCount << "\n";
}

bool Compiler::compile() {
  this->tokens = this->lexer.tokenize();
  this->printLexerTokens();
  this->parser.setTokens(this->tokens);
  this->parser.parse(this->out);
  this->parser.printParserOutput(this->out);
  this->out.close();
  return (this->parser.getErrorCount() == 0 &&
          this->calcLexerErrorCount() == 0);
}