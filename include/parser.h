#pragma once

#include <string>
#include <vector>
#include <fstream>
#include "Token.h"

class Parser {
private:
    std::vector<Token> tokens;
    Token current_token;
    unsigned int token_index;
    unsigned int line_count;
    unsigned int slow_count;
    unsigned int error_count;
    bool in_function_scope;
    std::vector<string> output_lines;

    bool isDataType(TokenType token);
    bool isStartOfStatement(TokenType type);
    bool isStartsOfLine(TokenType token);
    void nextToken();
    void throwError(std::ofstream& out);

    void parseDeclarations(std::ofstream& out);
    void parseDeclarationList(std::ofstream& out);
    void parseDeclaration(std::ofstream& out);
    void parseStructDec(std::ofstream& out);
    void parseVarDec(std::ofstream& out, bool isStruct);
    void parseTypeSpecifier(std::ofstream& out);
    void parseFunDec(std::ofstream& out);
    void parseParams(std::ofstream& out);
    void parseParamList(std::ofstream& out);
    void parsePList(std::ofstream& out);
    void parseParam(std::ofstream& out);

    void parseCompoundStmt(std::ofstream& out);
    void parseLocalDecs(std::ofstream& out);
    void parseStmtList(std::ofstream& out);
    void parseStatement(std::ofstream& out);
    void parseExpressionStmt(std::ofstream& out);
    void parseSelectionStmt(std::ofstream& out);
    void parseIterationStmt(std::ofstream& out);
    void parseJumpStmt(std::ofstream& out);

    void parseExpression(std::ofstream& out);
    void parseIdAssign(std::ofstream& out);
    void parseSimpleExpression(std::ofstream& out);
    void parseRelop(std::ofstream& out);
    void parseAdditiveExpression(std::ofstream& out);
    void parseAdditiveExpressionPrime(std::ofstream& out);
    void parseAddOp(std::ofstream& out);
    void parseTerm(std::ofstream& out);
    void parseMulOp(std::ofstream& out);
    void parseTermPrime(std::ofstream& out);
    void parseFactor(std::ofstream& out);
    void parseCall(std::ofstream& out);
    void parseArgs(std::ofstream& out);
    void parseArgList(std::ofstream& out);
    void parseAList(std::ofstream& out);

    void parseNum(std::ofstream& out);
    void parseUnsignedNum(std::ofstream& out);
    void parseSignedNum(std::ofstream& out);
    void parsePosNum(std::ofstream& out);
    void parseNegNum(std::ofstream& out);
    void parseValue(std::ofstream& out);

    void parseComment(std::ofstream& out);
    void parseIncludeCommand(std::ofstream& out);
    void parseFName(std::ofstream& out);
  int getNum(const std::string &s);

public:
    Parser();
  void setTokens(const std::vector<Token> &input_tokens);
    void printParserOutput(std::ofstream &out);
    int parse(std::ofstream& out);
    unsigned int getErrorCount() const;
};
