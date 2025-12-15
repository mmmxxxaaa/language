#ifndef NEW_GREAT_INPUT_H
#define NEW_GREAT_INPUT_H

#include "tree_base.h"
#include "variable_parse.h"

typedef enum {
    TOKEN_IDENTIFIER = 0,
    TOKEN_NUMBER,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_ASSIGN,        // =
    TOKEN_SEMICOLON,     // ;
    TOKEN_LPAREN,        // (
    TOKEN_RPAREN,        // )
    TOKEN_LBRACE,        // {
    TOKEN_RBRACE,        // }
    TOKEN_EQUAL,         // ==
    TOKEN_NOT_EQUAL,     // !=
    TOKEN_LESS,          // <
    TOKEN_LESS_EQUAL,    // <=
    TOKEN_GREATER,       // >
    TOKEN_GREATER_EQUAL, // >=
    TOKEN_AND,           // &&
    TOKEN_OR,            // ||
    TOKEN_NOT,           // !
    TOKEN_COMMA,         // ,
    TOKEN_FUNC,          // Freestayla
    TOKEN_RETURN,        // Rakamakafo
    TOKEN_IF,            // Изюм-Фактор
    TOKEN_WHILE,         // Вова-АЙболит-Лечит-говнокод
    TOKEN_EOF,           // Конец строки
    TOKEN_ERROR          // Ошибка
} TokenType;

typedef struct {
    TokenType type;
    union {
        char* identifier;  // для TOKEN_IDENTIFIER
        double number;     // для TOKEN_NUMBER
    } value;
} Token;

typedef struct {
    TokenType token_type;
    const char* token_string;
} StandardTokenInfo;

typedef struct {
    TokenType token_type;
    NodeType  node_type;
} StructForCompareTokenAndNode;

TokenType FindTokenByString(const char* str);
NodeType TranslateFromTokenTypeToNodeType(TokenType token_type);
Token* Lexer(const char* s, int* token_count);
void FreeTokens(Token* tokens, int token_count);

Node* ReadProgramFromFile(const char* filename, ParserContext* context);


#endif // NEW_GREAT_INPUT_H
