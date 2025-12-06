#ifndef NEW_GREAT_INPUT_H
#define NEW_GREAT_INPUT_H

#include "tree_base.h"
#include "variable_parse.h"

typedef enum {
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_ASSIGN,    // =
    TOKEN_SEMICOLON, // ;
    TOKEN_LPAREN,    // (
    TOKEN_RPAREN,    // )
    TOKEN_LBRACE,    // {
    TOKEN_RBRACE,    // }
    TOKEN_IF,        // Изюм-Фактор
    TOKEN_WHILE,     // Вова-АЙболит-Лечит-говнокод
    TOKEN_EOF,       // Конец строки
    TOKEN_ERROR      // Ошибка
} TokenType;

typedef struct {
    TokenType type;
    union {
        char* identifier;  // для TOKEN_IDENTIFIER
        double number;     // для TOKEN_NUMBER
    } value;
} Token;

Token* Lexer(const char* s, int* token_count);
void FreeTokens(Token* tokens, int token_count);

Node* ParseProgram(const char* source, VariableTable* var_table);
Node* ReadProgramFromFile(const char* filename, VariableTable* var_table);


#endif // NEW_GREAT_INPUT_H
