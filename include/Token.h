#ifndef TOKEN_H
#define TOKEN_H

#include <string>
using namespace std;

enum TokenType {
    CLEAR,
    CONDITION,
    INTEGER,
    SINTEGER,
    CHARACTER,
    STRING,
    FLOAT,
    SFLOAT,
    VOID,
    LOOP,
    RETURN,
    ARITHMETIC_OP,
    LOGIC_OP,
    RELATIONAL_OP,
    ASSIGNMENT_OP,
    ACCESS_OP,
    BRACE,
    CONSTANT,
    QUOTATION_MARK,
    INCLUSION,
    STRUCT,
    BREAK,
    IDENTIFIER,
    COMMA,
    COLON,
    SEMICOLON,
    COMMENT_START,
    COMMENT_CONTENT,
    COMMENT_END,
    SINGLE_LINE_COMMENT_START,
    SINGLE_LINE_COMMENT_CONTENT,
    INVALID,
    INVALID_COMMENT,
    INVALID_INCLUSION,
    STRING_LITERAL,
    CHARACTER_LITERAL,
    UNTERMINATED_STRING,
    UNTERMINATED_CHAR,
    INVALID_IDENTIFIER,
    ADDOP,
    MULOP,
    AMPERSAND,
    UNKNOWN,
    EOF_TOKEN
};

class Token {
public:
    int line;
    std::string text;
    TokenType type;
    bool error;

    Token(int line = 0,
          const std::string& text = "",
          TokenType type = INVALID,
          bool error = false)
        : line(line), text(text), type(type), error(error) {}
};
std::string tokenTypeToString(TokenType t);

#endif
