#include "parser.h"
#include <algorithm>
#include <iostream>

Parser::Parser()
    : token_index(0), line_count(1), slow_count(1), error_count(0),
      in_function_scope(false) {}

void Parser::setTokens(const std::vector<Token> &input_tokens) {
  tokens = input_tokens;
  token_index = 0;
  if (!tokens.empty()) {
    current_token = tokens[0];
  }
}

int Parser::getNum(const std::string &s) {
  size_t pos = s.find("Line:");
  if (pos == std::string::npos)
    pos += 5;
  while (pos < s.size() && std::isspace(s[pos]))
    ++pos;
  int n = 0;
  while (pos < s.size() && std::isdigit(s[pos])) {
    n = n * 10 + (s[pos] - '0');
    ++pos;
  }
  return n;
}

void Parser::printParserOutput(std::ofstream &out) {
  std::sort(this->output_lines.begin(), this->output_lines.end(),
            [&](const std::string &a, const std::string &b) {
              return getNum(a) < getNum(b);
            });
  for (auto &&i : this->output_lines) {
    std::cout << i << std::endl;
    out << i << std::endl;
  }
}

int Parser::parse(std::ofstream &out) {
  if (tokens.empty()) {
    output_lines.push_back("No tokens to parse!");

    return 1;
  }
  output_lines.push_back("\nParser Results:\n");
  output_lines.push_back(std::string(50, '-'));

  parseDeclarations(out);
  if (current_token.type != EOF_TOKEN) {
    throwError(out);
  }
  output_lines.push_back("Total NO of errors: " + std::to_string(error_count));
  return error_count == 0 ? 0 : 1;
}

unsigned int Parser::getErrorCount() const { return error_count; }

bool Parser::isDataType(TokenType token) {
  return token == INTEGER || token == SINTEGER || token == CHARACTER ||
         token == STRING || token == FLOAT || token == SFLOAT ||
         token == VOID || token == STRUCT;
}

bool Parser::isStartOfStatement(TokenType type) {
  return type == IDENTIFIER || type == CONSTANT || type == STRING_LITERAL ||
         type == CHARACTER_LITERAL ||
         (type == BRACE &&
          (current_token.text == "(" || current_token.text == "{")) ||
         type == CONDITION || type == LOOP || type == RETURN || type == BREAK;
}

bool Parser::isStartsOfLine(TokenType token) { return token == INCLUSION; }

void Parser::nextToken() {
  if (token_index + 1 < tokens.size()) {
    slow_count +=
        (this->current_token.type == SEMICOLON ||
         this->current_token.type == COMMENT_END ||
         this->current_token.type == SINGLE_LINE_COMMENT_CONTENT ||
         (this->current_token.type == BRACE &&
          (this->current_token.text == "{" || this->current_token.text == "}")))
            ? 1
            : 0;
    token_index++;
    current_token = tokens[token_index];
    line_count = current_token.line;
  } else {
    current_token = Token(line_count, "", EOF_TOKEN, false);
  }
}

void Parser::throwError(std::ofstream &out) {
  error_count++;
  output_lines.push_back("Line : " + std::to_string(slow_count) +
                         " Not Matched Error: Unexpected token '" +
                         current_token.text + "'");
  while (current_token.type != SEMICOLON && current_token.type != BRACE &&
         current_token.type != EOF_TOKEN && token_index < tokens.size()) {
    nextToken();
  }
  if (current_token.type == SEMICOLON && token_index + 1 < tokens.size()) {
    nextToken();
  }
}

void Parser::parseDeclarations(std::ofstream &out) {
  while (isDataType(current_token.type) || current_token.type == INCLUSION ||
         current_token.type == COMMENT_START ||
         current_token.type == SINGLE_LINE_COMMENT_START) {
    if (current_token.type == INCLUSION) {
      parseIncludeCommand(out);
    } else if (current_token.type == COMMENT_START ||
               current_token.type == SINGLE_LINE_COMMENT_START) {
      parseComment(out);
    } else {
      parseDeclaration(out);
    }
  }
}

void Parser::parseDeclarationList(std::ofstream &out) {
  while (isDataType(current_token.type)) {
    parseDeclaration(out);
  }
}

void Parser::parseDeclaration(std::ofstream &out) {
  int start_line = current_token.line;
  if (isDataType(current_token.type)) {
    bool isStruct = (current_token.type == STRUCT);
    parseTypeSpecifier(out);
    if (current_token.type == IDENTIFIER) {
      parseIdAssign(out);
      if (current_token.type == BRACE && current_token.text == "(") {
        output_lines.push_back("Line : " + std::to_string(slow_count) +
                               " Matched Rule used: Function-declaration");
        in_function_scope = true;
        parseFunDec(out);
        in_function_scope = false;
      } else if (current_token.type == BRACE && current_token.text == "{") {
        output_lines.push_back("Line : " + std::to_string(slow_count) +
                               " Matched Rule used: Struct-declaration");
        parseStructDec(out);
      } else {
        output_lines.push_back("Line : " + std::to_string(slow_count) +
                               " Matched Rule used: Variable-declaration");
        parseVarDec(out, isStruct);
      }
    } else {
      throwError(out);
    }
  } else {
    throwError(out);
  }
}

void Parser::parseStructDec(std::ofstream &out) {
  if (current_token.type == BRACE && current_token.text == "{") {
    nextToken();
    parseLocalDecs(out);
    if (current_token.type == BRACE && current_token.text == "}") {
      nextToken();
      if (current_token.type == SEMICOLON) {
        nextToken();
      } else {
        throwError(out);
      }
    } else {
      throwError(out);
    }
  } else {
    throwError(out);
  }
}

void Parser::parseVarDec(std::ofstream &out, bool isStruct) {
  if (current_token.type == IDENTIFIER) {
    // so i either look back at the type which breaks the rule of top->down and
    // left->right, or i pass in a boo.
    if (isStruct) {
      parseIdAssign(out);
    }
    parseIdAssign(out);
    if (current_token.type == ASSIGNMENT_OP) {
      if (!in_function_scope) {
        output_lines.push_back("Line : " + std::to_string(slow_count) +
                               "ERROR: Variable initialization only allowed "
                               "inside function");
        throwError(out);
      } else {
        nextToken();
        parseExpression(out);
      }
    }
    if (current_token.type == BRACE && current_token.text == "[") {
      nextToken();
      if (current_token.type == CONSTANT) {
        nextToken();
        if (current_token.type == BRACE && current_token.text == "]") {
          nextToken();
        } else {
          throwError(out);
        }
      } else {
        throwError(out);
      }
    }
  } else if (current_token.type == ARITHMETIC_OP && current_token.text == "*") {
    nextToken();
    if (current_token.type == IDENTIFIER) {
      parseIdAssign(out);
    } else {
      throwError(out);
    }
  } else if (current_token.type == SEMICOLON) {
    nextToken();
    return;
  } else {
    throwError(out);
  }
  if (current_token.type == SEMICOLON) {
    nextToken();
  } else {
    throwError(out);
  }
}

void Parser::parseTypeSpecifier(std::ofstream &out) {
  if (isDataType(current_token.type)) {
    nextToken();
  } else {
    throwError(out);
  }
}

void Parser::parseFunDec(std::ofstream &out) {
  if (current_token.type == BRACE && current_token.text == "(") {
    nextToken();
    parseParams(out);
    if (current_token.type == BRACE && current_token.text == ")") {
      nextToken();
      if (current_token.type == BRACE && current_token.text == "{") {
        parseCompoundStmt(out);
      } else {
        throwError(out);
      }
    } else {
      throwError(out);
    }
  } else {
    throwError(out);
  }
}

void Parser::parseParams(std::ofstream &out) {
  if (current_token.type == VOID) {
    nextToken();
    return;
  }
  if (isDataType(current_token.type)) {
    parseParamList(out);
  }
}

void Parser::parseParamList(std::ofstream &out) {
  parseParam(out);
  parsePList(out);
}

void Parser::parsePList(std::ofstream &out) {
  if (current_token.type == COMMA) {
    nextToken();
    parseParam(out);
    parsePList(out);
  }
}

void Parser::parseParam(std::ofstream &out) {
  if (isDataType(current_token.type)) {
    if (current_token.type == STRUCT) {
      nextToken();
    }
    nextToken();
    if (current_token.type == IDENTIFIER) {
      parseIdAssign(out);
    } else {
      throwError(out);
    }
  } else {
    throwError(out);
  }
}

void Parser::parseCompoundStmt(std::ofstream &out) {
  if (current_token.type == BRACE && current_token.text == "{") {
    nextToken();
    if (current_token.type == COMMENT_START ||
        current_token.type == SINGLE_LINE_COMMENT_START) {
      parseComment(out);
    }
    parseLocalDecs(out);
    parseStmtList(out);
    if (current_token.type == BRACE && current_token.text == "}") {
      nextToken();
    } else {
      throwError(out);
    }
  } else {
    throwError(out);
  }
}

void Parser::parseLocalDecs(std::ofstream &out) {
  while (isDataType(current_token.type)) {
    bool isStruct = current_token.type == STRUCT;
    parseTypeSpecifier(out);
    parseVarDec(out, isStruct);
  }
}

void Parser::parseStmtList(std::ofstream &out) {
  while (isStartOfStatement(current_token.type)) {
    parseStatement(out);
  }
}

void Parser::parseStatement(std::ofstream &out) {
  int start_line = current_token.line;
  switch (current_token.type) {
  case IDENTIFIER:
  case CONSTANT:
  case STRING_LITERAL:
  case CHARACTER_LITERAL:
    output_lines.push_back("Line : " + std::to_string(slow_count) +
                           " Matched Rule used: Expression-statement");
    parseExpressionStmt(out);
    break;
  case BRACE:
    if (current_token.text == "(") {
      output_lines.push_back("Line : " + std::to_string(slow_count) +
                             " Matched Rule used: Expression-statement");
      parseExpressionStmt(out);

    } else if (current_token.text == "{") {
      output_lines.push_back("Line : " + std::to_string(slow_count) +
                             " Matched Rule used: Compound-statement");
      parseCompoundStmt(out);

    } else {
      throwError(out);
    }
    break;
  case CONDITION:
    output_lines.push_back("Line : " + std::to_string(slow_count) +
                           " Matched Rule used: Selection-statement");
    parseSelectionStmt(out);

    break;
  case LOOP:
    output_lines.push_back("Line : " + std::to_string(slow_count) +
                           " Matched Rule used: Iteration-statement");
    parseIterationStmt(out);

    break;
  case RETURN:
  case BREAK:
    output_lines.push_back("Line : " + std::to_string(slow_count) +
                           " Matched Rule used: Jump-statement");
    parseJumpStmt(out);

    break;
  default:
    throwError(out);
  }
}

void Parser::parseExpressionStmt(std::ofstream &out) {
  if (current_token.type == SEMICOLON) {
    nextToken();
    return;
  }
  parseExpression(out);
  if (current_token.type == SEMICOLON) {
    nextToken();
  } else {
    throwError(out);
  }
}

void Parser::parseSelectionStmt(std::ofstream &out) {
  if (current_token.type == CONDITION) {
    nextToken();
    if (current_token.type == BRACE && current_token.text == "(") {
      nextToken();
      parseExpression(out);
      if (current_token.type == BRACE && current_token.text == ")") {
        nextToken();
        parseStatement(out);
        if (current_token.type == CONDITION &&
            current_token.text == "Otherwise") {
          nextToken();
          parseStatement(out);
        }
      } else {
        throwError(out);
      }
    } else {
      throwError(out);
    }
  } else {
    throwError(out);
  }
}

void Parser::parseIterationStmt(std::ofstream &out) {
  if (current_token.type == LOOP) {
    if (current_token.text == "Reiterate") {
      nextToken();
      if (current_token.type == BRACE && current_token.text == "(") {
        nextToken();
        // so in the rules its reiterate (exp;exp;exp) but if it's supposed to
        // be a for loop then the first one is either an expression or vardec. i
        // dunno man.
        if (isDataType(current_token.type)) {
          parseTypeSpecifier(out);
          // vardec consumes the ; from the line while expression does not
          // because it's always wrapped with expression statement.
          parseVarDec(out, false);
          token_index -= 2;
          nextToken();
        } else {
          parseExpression(out);
        }
        if (current_token.type == SEMICOLON) {
          nextToken();
          parseExpression(out);
          if (current_token.type == SEMICOLON) {
            nextToken();
            parseExpression(out);
            if (current_token.type == BRACE && current_token.text == ")") {
              nextToken();
              parseStatement(out);
            } else {
              throwError(out);
            }
          } else {
            throwError(out);
          }
        } else {
          throwError(out);
        }
      } else {
        throwError(out);
      }
    } else {
      nextToken();
      if (current_token.type == BRACE && current_token.text == "(") {
        nextToken();
        parseExpression(out);
        if (current_token.type == BRACE && current_token.text == ")") {
          nextToken();
          parseStatement(out);
        } else {
          throwError(out);
        }
      } else {
        throwError(out);
      }
    }
  } else {
    throwError(out);
  }
}

void Parser::parseJumpStmt(std::ofstream &out) {
  if (current_token.type == RETURN) {
    nextToken();
    if (current_token.type != SEMICOLON) {
      parseExpression(out);
    }
    if (current_token.type == SEMICOLON) {
      nextToken();
    } else {
      throwError(out);
    }
  } else if (current_token.type == BREAK) {
    nextToken();
    if (current_token.type == SEMICOLON) {
      nextToken();
    } else {
      throwError(out);
    }
  } else {
    throwError(out);
  }
}

void Parser::parseExpression(std::ofstream &out) {
  if (current_token.type == IDENTIFIER) {
    // I'm not sure we can edit the grammar beyond accounting for left recursion
    // so i'll use backtracking here even though i've been avoiding it.
    int id_token = token_index;
    parseIdAssign(out);
    if (current_token.type == ASSIGNMENT_OP) {
      nextToken();
      parseExpression(out);
    } else {
      token_index = id_token - 1;
      nextToken();
      parseSimpleExpression(out);
    }
  } else {
    parseSimpleExpression(out);
  }
}

void Parser::parseIdAssign(std::ofstream &out) {
  if (current_token.type == IDENTIFIER) {
    if (!std::isalpha(current_token.text[0]) && current_token.text[0] != '_') {
      output_lines.push_back("Line : " + std::to_string(current_token.line) +
                             " Not Matched Error: Invalid identifier \"" +
                             current_token.text + "\"");

      throwError(out);
    } else {
      nextToken();
      if (current_token.type == ACCESS_OP) {
        nextToken();
        parseIdAssign(out);
      } else if (current_token.type == BRACE && current_token.text == "[") {
        nextToken();
        if (current_token.type == IDENTIFIER) {
          parseIdAssign(out);
        } else if (current_token.type == CONSTANT) {
          nextToken();
        } else {
          throwError(out);
        }
        if (current_token.type != BRACE || current_token.text != "]") {
          throwError(out);
        } else {
          nextToken();
        }
      }
    }
  } else {
    throwError(out);
  }
}

void Parser::parseSimpleExpression(std::ofstream &out) {
  parseAdditiveExpression(out);
  if (current_token.type == RELATIONAL_OP || current_token.type == LOGIC_OP) {
    parseRelop(out);
    parseAdditiveExpression(out);
  }
}

void Parser::parseRelop(std::ofstream &out) {
  if (current_token.type == RELATIONAL_OP || current_token.type == LOGIC_OP) {
    nextToken();
  } else {
    throwError(out);
  }
}

void Parser::parseAdditiveExpression(std::ofstream &out) {
  parseTerm(out);
  parseAdditiveExpressionPrime(out);
}

void Parser::parseAdditiveExpressionPrime(std::ofstream &out) {
  if (current_token.type == ADDOP) {
    parseAddOp(out);
    parseTerm(out);
    parseAdditiveExpressionPrime(out);
  }
}

void Parser::parseAddOp(std::ofstream &out) {
  if (current_token.type == ADDOP) {
    nextToken();
  } else {
    throwError(out);
  }
}

void Parser::parseTerm(std::ofstream &out) {
  parseFactor(out);
  parseTermPrime(out);
}

void Parser::parseTermPrime(std::ofstream &out) {
  if (current_token.type == MULOP) {
    parseMulOp(out);
    parseFactor(out);
    parseTermPrime(out);
  }
}

void Parser::parseMulOp(std::ofstream &out) {
  if (current_token.type == MULOP) {
    nextToken();
  } else {
    throwError(out);
  }
}

void Parser::parseFactor(std::ofstream &out) {
  switch (current_token.type) {
  case BRACE:
    if (current_token.text == "(") {
      nextToken();
      parseExpression(out);
      if (current_token.type == BRACE && current_token.text == ")") {
        nextToken();
      } else {
        throwError(out);
      }
    } else {
      throwError(out);
    }
    break;
  case IDENTIFIER: {
    parseIdAssign(out);
    if (current_token.type == BRACE && current_token.text == "(") {
      parseCall(out);
    } else if (current_token.type == ACCESS_OP) {
      nextToken();
      parseIdAssign(out);
    }
  } break;
  case CONSTANT:
  case STRING_LITERAL:
  case CHARACTER_LITERAL:
    nextToken();
    break;
  case ADDOP:
    parseSignedNum(out);
    break;
  case ARITHMETIC_OP:
    if (current_token.text == "*") {
      nextToken();
      parseFactor(out);
    } else {
      throwError(out);
    }
    break;
  default:
    throwError(out);
  }
}

void Parser::parseCall(std::ofstream &out) {
  if (current_token.type == BRACE && current_token.text == "(") {
    nextToken();
    parseArgs(out);
    if (current_token.type == BRACE && current_token.text == ")") {
      nextToken();
    } else {
      throwError(out);
    }
  } else {
    throwError(out);
  }
}

void Parser::parseArgs(std::ofstream &out) {
  if (current_token.type != BRACE || current_token.text != ")") {
    parseArgList(out);
  }
}

void Parser::parseArgList(std::ofstream &out) {
  parseExpression(out);
  parseAList(out);
}

void Parser::parseAList(std::ofstream &out) {
  if (current_token.type == COMMA) {
    nextToken();
    parseExpression(out);
    parseAList(out);
  }
}

void Parser::parseNum(std::ofstream &out) {
  if (current_token.type == ADDOP) {
    parseSignedNum(out);
  } else if (current_token.type == CONSTANT) {
    parseUnsignedNum(out);
  } else {
    throwError(out);
  }
}

void Parser::parseSignedNum(std::ofstream &out) {
  if (current_token.type == ADDOP) {
    if (current_token.text == "+") {
      parsePosNum(out);
    } else if (current_token.text == "-") {
      parseNegNum(out);
    } else {
      throwError(out);
    }
  } else {
    throwError(out);
  }
}

void Parser::parseUnsignedNum(std::ofstream &out) { parseValue(out); }

void Parser::parsePosNum(std::ofstream &out) {
  if (current_token.type == ADDOP && current_token.text == "+") {
    nextToken();
    parseValue(out);
  } else {
    throwError(out);
  }
}

void Parser::parseNegNum(std::ofstream &out) {
  if (current_token.type == ADDOP && current_token.text == "-") {
    nextToken();
    parseValue(out);
  } else {
    throwError(out);
  }
}

void Parser::parseValue(std::ofstream &out) {
  if (current_token.type == CONSTANT) {
    nextToken();
  } else {
    throwError(out);
  }
}

void Parser::parseComment(std::ofstream &out) {
  int start_line = current_token.line;
  if (current_token.type == COMMENT_START) {
    nextToken();
    if (current_token.type == COMMENT_CONTENT) {
      nextToken();
    }
    if (current_token.type == COMMENT_END ||
        current_token.type == INVALID_COMMENT) {
      nextToken();
    } else {
      throwError(out);
    }
  } else if (current_token.type == SINGLE_LINE_COMMENT_START) {
    nextToken();
    if (current_token.type == SINGLE_LINE_COMMENT_CONTENT) {
      nextToken();
    }
  output_lines.push_back("Line : " + std::to_string(slow_count) +
                         " Matched Rule used: Comment");
  } else {
    throwError(out);
  }
}

void Parser::parseIncludeCommand(std::ofstream &out) {
  int start_line = current_token.line;
  if (current_token.type == INCLUSION) {
    nextToken();
    if (current_token.type == STRING_LITERAL ||
        current_token.type == INVALID_INCLUSION) {
      parseFName(out);
      if (current_token.type == SEMICOLON) {
        output_lines.push_back("Line : " + std::to_string(slow_count) +
                               " Matched Rule used: Include-command");
        nextToken();
      } else {
        throwError(out);
      }
    } else {
      throwError(out);
    }
  } else {
    throwError(out);
  }
}

void Parser::parseFName(std::ofstream &out) {
  if (current_token.type == STRING_LITERAL ||
      current_token.type == INVALID_INCLUSION) {
    nextToken();
  } else {
    throwError(out);
  }
}
