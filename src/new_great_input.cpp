#include "new_great_input.h"
#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "tree_base.h"
#include "DSL.h"
// #include "logic_functions.h"

Token* Lexer(const char* s, int* token_count)
{
    int capacity = 64;
    int n = 0;
    Token* tokens = (Token*)calloc(capacity, sizeof(Token));

    if (!tokens)
    {
        *token_count = 0;
        return NULL;
    }

    while (*s != '\0')
    {
        //FIXME в отдельную функцию
        if (n >= capacity - 1)
        {
            capacity *= 2;
            Token* new_tokens = (Token*)realloc(tokens, capacity * sizeof(Token));
            if (!new_tokens)
            {
                for (int i = 0; i < n; i++)
                {
                    if (tokens[i].type == TOKEN_IDENTIFIER && tokens[i].value.identifier)
                        free(tokens[i].value.identifier);
                }
                free(tokens);
                *token_count = 0;
                return NULL;
            }
            tokens = new_tokens;
        }

        if (isspace(*s))
        {
            s++;
            continue;
        }
        if (*s == '(')
        {
            tokens[n++].type = TOKEN_LPAREN;
            s++;
            continue;
        }
        if (*s == ')')
        {
            tokens[n++].type = TOKEN_RPAREN;
            s++;
            continue;
        }
        if (*s == '+')
        {
            tokens[n++].type = TOKEN_PLUS;
            s++;
            continue;
        }
        if (*s == '-')
        {
            tokens[n++].type = TOKEN_MINUS;
            s++;
            continue;
        }
        if (*s == '*')
        {
            tokens[n++].type = TOKEN_MULTIPLY;
            s++;
            continue;
        }
        if (*s == '/')
        {
            tokens[n++].type = TOKEN_DIVIDE;
            s++;
            continue;
        }
        if (*s == '=')
        {
            tokens[n++].type = TOKEN_ASSIGN;
            s++;
            continue;
        }
        if (*s == ';')
        {
            tokens[n++].type = TOKEN_SEMICOLON;
            s++;
            continue;
        }
        if (*s == '{')
        {
            tokens[n++].type = TOKEN_LBRACE;
            s++;
            continue;
        }
        if (*s == '}')
        {
            tokens[n++].type = TOKEN_RBRACE;
            s++;
            continue;
        }
        if (isdigit(*s))
        {
            double val = 0;
            while (isdigit(*s))
            {
                val = val * 10 + (*s - '0');
                s++;
            }
            tokens[n].type = TOKEN_NUMBER;
            tokens[n].value.number = val;
            n++;
            continue;
        }

        if (isalpha(*s))
        {
            char buffer[256] = {0}; //FIXME
            int i = 0;

            // первый символ обязательно буква
            buffer[i++] = *s++;

            while (isalnum(*s) || *s == '_')
            {
                if (i < 255)
                {
                    buffer[i++] = *s;
                }
                s++;
            }
            buffer[i] = '\0';

            if (strcmp(buffer, "Izum_Factor") == 0)
            {
                tokens[n++].type = TOKEN_IF;
                continue;
            }
            else if (strcmp(buffer, "Vova_AIbolit_Lechit_govnocod") == 0)
            {
                tokens[n++].type = TOKEN_WHILE;
                continue;
            }

            tokens[n].type = TOKEN_IDENTIFIER;
            tokens[n].value.identifier = strdup(buffer);
            n++;
            continue;
        }

        tokens[n++].type = TOKEN_ERROR;
        break;
    }

    tokens[n].type = TOKEN_EOF;
    tokens[n].value.identifier = NULL;
    n++;

    if (n < capacity)
    {
        Token* resized_tokens = (Token*)realloc(tokens, n * sizeof(Token));
        if (resized_tokens)
            tokens = resized_tokens;
    }

    *token_count = n;
    return tokens;
}

void FreeTokens(Token* tokens, int token_count)
{
    if (!tokens) return;

    for (int i = 0; i < token_count; i++)
    {
        if (tokens[i].type == TOKEN_IDENTIFIER && tokens[i].value.identifier)
        {
            free(tokens[i].value.identifier);
        }
    }
    free(tokens);
}

static bool CheckToken(Token* tokens, int index, int token_count, TokenType type)
{
    return (index < token_count && tokens[index].type == type);
}

static bool ExpectToken(Token* tokens, int* index, int token_count, TokenType type, const char* error_msg)
{
    if (*index >= token_count || tokens[*index].type != type)
    {
        printf("Error: %s\n", error_msg);
        return false;
    }
    (*index)++;
    return true;
}

// ============================================================================
// функции для парсера
// ============================================================================

static Node* GetExpression(Token* tokens, int* index, int token_count, VariableTable* var_table);
static Node* GetTerm(Token* tokens, int* index, int token_count, VariableTable* var_table);
static Node* GetFactor(Token* tokens, int* index, int token_count, VariableTable* var_table);
static Node* ParseBlock(Token* tokens, int* index, int token_count, VariableTable* var_table);
static Node* GetAssignment(Token* tokens, int* index, int token_count, VariableTable* var_table);

static Node* GetIF(Token* tokens, int* index, int token_count, VariableTable* var_table)
{
    (*index)++; //пропустили if

    if (!ExpectToken(tokens, index, token_count, TOKEN_LPAREN, "Expected '(' after 'if'"))
        return NULL;

    Node* condition = GetExpression(tokens, index, token_count, var_table);
    if (!condition)
        return NULL;

    if (!ExpectToken(tokens, index, token_count, TOKEN_RPAREN, "Expected ')' after condition"))
    {
        FreeSubtree(condition);
        return NULL;
    }

    Node* body = ParseBlock(tokens, index, token_count, var_table);
    if (!body)
    {
        FreeSubtree(condition);
        return NULL;
    }

    ValueOfTreeElement data = {};

    return CreateNode(NODE_IF, data, condition, body);
}

static Node* GetST(Token* tokens, int* index, int token_count, VariableTable* var_table)
{
    if (CheckToken(tokens, *index, token_count, TOKEN_IF))
    {
        return GetIF(tokens, index, token_count, var_table);
    }

    if (CheckToken(tokens, *index, token_count, TOKEN_LBRACE))
    {
        return ParseBlock(tokens, index, token_count, var_table);
    }

    return GetAssignment(tokens, index, token_count, var_table);
}

static Node* ParseBlock(Token* tokens, int* index, int token_count, VariableTable* var_table)
{
    if (!ExpectToken(tokens, index, token_count, TOKEN_LBRACE, "Expected '{'"))
        return NULL;

    if (CheckToken(tokens, *index, token_count, TOKEN_RBRACE))
    {
        (*index)++;
        ValueOfTreeElement data = {};
        return CreateNode(NODE_EMPTY, data, NULL, NULL);
    }

    Node* first_stmt = GetST(tokens, index, token_count, var_table);
    if (!first_stmt)
        return NULL;

    Node* current = first_stmt;

    while (!CheckToken(tokens, *index, token_count, TOKEN_RBRACE))
    {
        Node* next_stmt = GetST(tokens, index, token_count, var_table);
        if (!next_stmt)
        {
            FreeSubtree(current);
            return NULL;
        }

        Node* seq_node = CreateNode(NODE_SEQUENCE, (ValueOfTreeElement){0}, current, next_stmt);
        current = seq_node;
    }

    if (!ExpectToken(tokens, index, token_count, TOKEN_RBRACE, "Expected '}'"))
    {
        FreeSubtree(current);
        return NULL;
    }

    return first_stmt ? current : CreateNode(NODE_EMPTY, (ValueOfTreeElement){0}, NULL, NULL);
}


//СИГМА СКИБИДИ

// E ::= T {('+' | '-') T}*
static Node* GetExpression(Token* tokens, int* index, int token_count, VariableTable* var_table)
{
    Node* left = GetTerm(tokens, index, token_count, var_table);
    if (!left)
        return NULL;

    while (*index < token_count)
    {
        TokenType type = tokens[*index].type;

        if (type != TOKEN_PLUS && type != TOKEN_MINUS)
            break;

        (*index)++;

        Node* right = GetTerm(tokens, index, token_count, var_table);
        if (!right)
        {
            FreeSubtree(left);
            return NULL;
        }

        if (type == TOKEN_PLUS)
            left = ADD(left, right);
        else
            left = SUB(left, right);
    }

    return left;
}

// T ::= F {('*' | '/') F}*
static Node* GetTerm(Token* tokens, int* index, int token_count, VariableTable* var_table)
{
    Node* left = GetFactor(tokens, index, token_count, var_table);
    if (!left) return NULL;

    while (*index < token_count)
    {
        TokenType type = tokens[*index].type;

        if (type != TOKEN_MULTIPLY && type != TOKEN_DIVIDE)
            break;

        (*index)++;

        Node* right = GetFactor(tokens, index, token_count, var_table);
        if (!right)
        {
            FreeSubtree(left);
            return NULL;
        }

        if (type == TOKEN_MULTIPLY)
            left = MUL(left, right);
        else
            left = DIV(left, right);
    }

    return left;
}

// F ::= '(' E ')' | Identifier | Number
static Node* GetFactor(Token* tokens, int* index, int token_count, VariableTable* var_table)
{
    if (*index >= token_count)
        return NULL;

    Token token = tokens[*index];

    // Ебашим скобки
    if (token.type == TOKEN_LPAREN)
    {
        (*index)++; // пропускаем '('

        Node* expr = GetExpression(tokens, index, token_count, var_table);
        if (!expr) return NULL;

        if (*index >= token_count || tokens[*index].type != TOKEN_RPAREN)
        {
            FreeSubtree(expr);
            printf("Error: Expected ')'\n");
            return NULL;
        }

        (*index)++; // пропускаем ')'

        return expr;
    }

    if (token.type == TOKEN_NUMBER)
    {
        (*index)++;
        return NUM(token.value.number);
    }

    if (token.type == TOKEN_IDENTIFIER)
    {
        (*index)++;

        TreeErrorType error = AddVariable(var_table, token.value.identifier);
        if (error != TREE_ERROR_NO && error != TREE_ERROR_VARIABLE_ALREADY_EXISTS &&
            error != TREE_ERROR_REDEFINITION_VARIABLE)
        {
            printf("Error adding variable to table: %d\n", error);
            return NULL;
        }

        return VAR(token.value.identifier);
    }

    printf("Error: Expected number, variable, or '(' in expression\n");
    return NULL;
}

// ============================================================================
// A ::= Identifier '=' E ';'
// ============================================================================

static Node* GetAssignment(Token* tokens, int* index, int token_count, VariableTable* var_table)
{
    if (*index >= token_count || tokens[*index].type != TOKEN_IDENTIFIER)
    {
        return NULL;
    }

    char* var_name = strdup(tokens[*index].value.identifier);
    (*index)++;

    // чекаем на '='
    if (!ExpectToken(tokens, index, token_count, TOKEN_ASSIGN, "Expected '=' after identifier"))
    {
        free(var_name);
        return NULL;
    }

    Node* expr = GetExpression(tokens, index, token_count, var_table);
    if (!expr)
    {
        free(var_name);
        return NULL;
    }

    if (!ExpectToken(tokens, index, token_count, TOKEN_SEMICOLON, "Expected ';' after expression"))
    {
        free(var_name);
        FreeSubtree(expr);
        return NULL;
    }

    Node* var_node = VAR(var_name);
    free(var_name);

    ValueOfTreeElement data = {};

    return CreateNode(NODE_ASSIGN, data, var_node, expr);
}

Node* ParseProgram(const char* source, VariableTable* var_table)
{
    int token_count = 0;
    Token* tokens = Lexer(source, &token_count);
    if (!tokens)
        return NULL;

    int index = 0;

    Node* program = GetST(tokens, &index, token_count, var_table);

    if (program && index < token_count && tokens[index].type != TOKEN_EOF)
    {
        printf("Error: Unexpected tokens at the end of program\n");
        FreeSubtree(program);
        program = NULL;
    }

    FreeTokens(tokens, token_count);
    return program;
}


//FIXME ассерты

static long GetFileSize(FILE* file)
{
    if (!file)
        return -1;

    long current_pos = ftell(file);
    if (current_pos == -1)
        return -1;

    if (fseek(file, 0, SEEK_END) != 0)
        return -1;

    long size = ftell(file);

    fseek(file, current_pos, SEEK_SET);

    return size;
}

Node* ReadProgramFromFile(const char* filename, VariableTable* var_table)
{
    FILE* file = fopen(filename, "r");
    if (!file)
    {
        printf("Cannot open file: %s\n", filename);
        return NULL;
    }

    long size = GetFileSize(file);

    char* buffer = (char*) calloc(size + 1, sizeof(char));
    if (!buffer)
    {
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, size, file);
    buffer[size] = '\0';
    fclose(file);

    Node* result = ParseProgram(buffer, var_table);

    free(buffer);
    return result;
}
