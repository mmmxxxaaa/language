#include "new_great_input.h"
#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "tree_base.h"
#include "DSL.h"
#include "function_parse.h"

static StandardTokenInfo standard_tokens[] = {
    {TOKEN_PLUS,          "+" },
    {TOKEN_MINUS,         "-" },
    {TOKEN_MULTIPLY,      "*" },
    {TOKEN_DIVIDE,        "/" },
    {TOKEN_ASSIGN,        "=" },
    {TOKEN_SEMICOLON,     ";" },
    {TOKEN_LPAREN,        "(" },
    {TOKEN_RPAREN,        ")" },
    {TOKEN_LBRACE,        "{" },
    {TOKEN_RBRACE,        "}" },
    {TOKEN_EQUAL,         "=="},
    {TOKEN_NOT_EQUAL,     "!="},
    {TOKEN_LESS,          "<" },
    {TOKEN_LESS_EQUAL,    "<="},
    {TOKEN_GREATER,       ">" },
    {TOKEN_GREATER_EQUAL, ">="},
    {TOKEN_AND,           "&&"},
    {TOKEN_OR,            "||"},
    {TOKEN_NOT,           "!" },
    {TOKEN_COMMA,         "," },
    {TOKEN_FUNC,          "Freestayla"},
    {TOKEN_RETURN,        "Rakamakafo"},
    {TOKEN_IF,            "Izum_Factor"},
    {TOKEN_WHILE,         "Vova_AIbolit_Lechit_govnocod"}
};

TokenType FindTokenByString(const char* str)
{
    for (size_t i = 0; i < sizeof(standard_tokens) / sizeof(standard_tokens[0]); i++)
    {
        if (strcmp(standard_tokens[i].token_string, str) == 0)
            return standard_tokens[i].token_type;
    }
    return TOKEN_IDENTIFIER; //FIXME или TokenError
}

static StructForCompareTokenAndNode standard_compare_token_and_node[] = {
    {TOKEN_EQUAL,         NODE_EQUAL},
    {TOKEN_NOT_EQUAL,     NODE_NOT_EQUAL},
    {TOKEN_LESS,          NODE_LESS},
    {TOKEN_LESS_EQUAL,    NODE_LESS_EQUAL},
    {TOKEN_GREATER,       NODE_GREATER},
    {TOKEN_GREATER_EQUAL, NODE_GREATER_EQUAL},
    {TOKEN_AND,           NODE_AND},
    {TOKEN_OR,            NODE_OR}
};

NodeType TranslateFromTokenTypeToNodeType(TokenType token_type)
{
    for (size_t i = 0; i < sizeof(standard_compare_token_and_node) / sizeof(standard_compare_token_and_node[0]); ++i)
    {
        if (standard_compare_token_and_node[i].token_type == token_type)
            return standard_compare_token_and_node[i].node_type;
    }

    return NODE_EMPTY;
}

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

        bool token_found = false;

        for (size_t i = 0; i < sizeof(standard_tokens) / sizeof(standard_tokens[0]); i++) //сначала бежим по двухсимвольным
        {
            size_t token_len = strlen(standard_tokens[i].token_string);

            if (token_len == 1)
                continue;

            if (strncmp(s, standard_tokens[i].token_string, token_len) == 0)
            {
                tokens[n++].type = standard_tokens[i].token_type;
                s += token_len;
                token_found = true;
                break;
            }
        }

        if (token_found)
            continue;

        for (size_t i = 0; i < sizeof(standard_tokens) / sizeof(standard_tokens[0]); i++) //потом по односимвольным
        {
            size_t token_len = strlen(standard_tokens[i].token_string);

            if (token_len == 1 && *s == standard_tokens[i].token_string[0])
            {
                tokens[n++].type = standard_tokens[i].token_type;
                s++;
                token_found = true;
                break;
            }
        }

        if (token_found)
            continue;

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
        if (isalpha(*s) || *s == '_')
        {
            char buffer[256] = {0}; //FIXME
            int i = 0;

            buffer[i++] = *s++;

            while (isalnum(*s) || *s == '_')
            {
                if (i < 255) //FIXME
                {
                    buffer[i++] = *s;
                }
                s++;
            }
            buffer[i] = '\0';

            token_found = false;
            for (size_t j = 0; j < sizeof(standard_tokens) / sizeof(standard_tokens[0]); j++)
            {
                if (strcmp(buffer, standard_tokens[j].token_string) == 0)
                {
                    tokens[n++].type = standard_tokens[j].token_type;
                    token_found = true;
                    break;
                }
            }

            if (!token_found)
            {
                tokens[n].type = TOKEN_IDENTIFIER;
                tokens[n].value.identifier = strdup(buffer);
                n++;
            }
            continue;
        }

        fprintf(stderr, "Lexer warning: Unknown character '%c', skipping", *s);
        s++;
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
//FIXME в стдеррорр ошибки
//FIXME подуматьк ак обрабатывать ошибки нормально

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
        fprintf(stderr, "Error: %s (got token type %d at index %d, expected %d)\n",
               error_msg,
               (*index < token_count) ? tokens[*index].type : -1,
               *index,
               type);
        return false;
    }
    (*index)++;
    return true;
}

// ============================================================================
// функции для парсера
// ============================================================================

static Node* GetExpression         (Token* tokens, int* index, int token_count, ParserContext* context);
static Node* GetLogicOr            (Token* tokens, int* index, int token_count, ParserContext* context);
static Node* GetLogicAnd           (Token* tokens, int* index, int token_count, ParserContext* context);
static Node* GetLogicEqual         (Token* tokens, int* index, int token_count, ParserContext* context);
static Node* GetLogicCompare       (Token* tokens, int* index, int token_count, ParserContext* context);
static Node* GetAddOrSub           (Token* tokens, int* index, int token_count, ParserContext* context);
static Node* GetTerm               (Token* tokens, int* index, int token_count, ParserContext* context);
static Node* GetFactor             (Token* tokens, int* index, int token_count, ParserContext* context);
static Node* GetFuncCall           (Token* tokens, int* index, int token_count, ParserContext* context);
static Node* GetIfStatement        (Token* tokens, int* index, int token_count, ParserContext* context);
static Node* GetWhileStatement     (Token* tokens, int* index, int token_count, ParserContext* context);
static Node* GetBlock              (Token* tokens, int* index, int token_count, ParserContext* context);
static Node* GetFunc               (Token* tokens, int* index, int token_count, ParserContext* context);
static Node* GetReturn             (Token* tokens, int* index, int token_count, ParserContext* context);
static Node* GetAssignmentStatement(Token* tokens, int* index, int token_count, ParserContext* context);

// General ::= { Func }*
Node* GetProgram(const char* source, ParserContext* context)
{
    int token_count = 0;
    Token* tokens = Lexer(source, &token_count);
    if (!tokens)
        return NULL;

    int index = 0;
    Node* program = NULL;
    Node* last_func = NULL;

    while (index < token_count && tokens[index].type != TOKEN_EOF)
    {
        int current_index = index;

        Node* func = GetFunc(tokens, &index, token_count, context);
        if (!func)
        {
            FreeSubtree(program);
            FreeTokens(tokens, token_count);
            return NULL;
        }

        if (!program)
        {
            program = func;
            last_func = func;
        }
        else
        {
            Node* seq = SEQ(last_func, func);
            if (program == last_func)
                program = seq;
            last_func = seq;
        }
    }

    FreeTokens(tokens, token_count);
    return program;
}

// Func ::= 'Freestayla' Identifier '(' Parameters ')' Block
static Node* GetFunc(Token* tokens, int* index, int token_count, ParserContext* context)
{
    if (!ExpectToken(tokens, index, token_count, TOKEN_FUNC, "Expected 'Freestayla'"))
        return NULL;

    if (*index >= token_count)
    {
        fprintf(stderr, "Error: Expected function name after 'Freestayla'\n");
        return NULL;
    }

    if (tokens[*index].type != TOKEN_IDENTIFIER)
    {
        fprintf(stderr, "Error: Expected function name, got token type %d\n", tokens[*index].type);
        return NULL;
    }

    char* func_name = strdup(tokens[*index].value.identifier);
    (*index)++;

    if (!ExpectToken(tokens, index, token_count, TOKEN_LPAREN, "Expected '(' after function name"))
    {
        free(func_name);
        return NULL;
    }

    // Parameters ::= ε | Parameter { ',' Parameter }*
    Node* params = NULL;
    Node* last_param = NULL;

    if (*index < token_count && tokens[*index].type != TOKEN_RPAREN)
    {

        // Parameter ::= Identifier
        if (*index >= token_count || tokens[*index].type != TOKEN_IDENTIFIER)
        {
            fprintf(stderr, "Error: Expected parameter name\n");
            free(func_name);
            return NULL;
        }

        char* param_name = strdup(tokens[*index].value.identifier);
        (*index)++;

        TreeErrorType error = AddVariable(&(context->var_table), param_name);
        if (error != TREE_ERROR_NO && error != TREE_ERROR_VARIABLE_ALREADY_EXISTS &&
            error != TREE_ERROR_REDEFINITION_VARIABLE)
        {
            fprintf(stderr, "Error adding parameter to table: %d\n", error);
            free(func_name);
            free(param_name);
            return NULL;
        }

        params = VAR(param_name);
        last_param = params;
        free(param_name);

        while (*index < token_count && tokens[*index].type == TOKEN_COMMA)
        {
            (*index)++; // пропускаем ','

            if (*index >= token_count || tokens[*index].type != TOKEN_IDENTIFIER)
            {
                printf("Error: Expected parameter name after comma\n");
                FreeSubtree(params);
                free(func_name);
                return NULL;
            }

            char* param_name2 = strdup(tokens[*index].value.identifier);
            (*index)++;

            error = AddVariable(&(context->var_table), param_name2);
            if (error != TREE_ERROR_NO && error != TREE_ERROR_VARIABLE_ALREADY_EXISTS &&
                error != TREE_ERROR_REDEFINITION_VARIABLE)
            {
                fprintf(stderr, "Error adding parameter to table: %d\n", error);
                FreeSubtree(params);
                free(func_name);
                free(param_name2);
                return NULL;
            }

            Node* param_node = VAR(param_name2);
            free(param_name2);

            if (last_param)
            {
                Node* seq = SEQ(last_param, param_node);
                if (!params) params = seq;
                last_param = seq;
            }
        }
    }

    if (!ExpectToken(tokens, index, token_count, TOKEN_RPAREN, "Expected ')' after parameters"))
    {
        FreeSubtree(params);
        free(func_name);
        return NULL;
    }

    Node* body = GetBlock(tokens, index, token_count, context);
    if (!body)
    {
        FreeSubtree(params);
        free(func_name);
        return NULL;
    }

    char **param_names = NULL;
    int param_count = 0;

    if (params)
    {
        Node* current = params;
        while (current)
        {
            param_count++;
            if (current->type == NODE_SEQUENCE)
                current = current->right;
            else
                break;
        }
        param_names = (char**)calloc(param_count, sizeof(char*));
        current = params;
        int i = 0;
        while (current && i < param_count)
        {
            if (current->type == NODE_SEQUENCE)
            {
                param_names[i] = strdup(current->left->data.var_definition.name);
                current = current->right;
            }
            else
            {
                param_names[i] = strdup(current->data.var_definition.name);
                current = NULL;
            }
            i++;
        }
    }

    TreeErrorType error = AddFunction(&(context->func_table), func_name, param_count, param_names, body);

    if (param_names)
    {
        for (int i = 0; i < param_count; i++)
        {
            free(param_names[i]);
        }
        free(param_names);
    }

    if (error != TREE_ERROR_NO && error != TREE_ERROR_FUNCTION_REDEFINITION)
    {
        fprintf(stderr, "Error adding function to table: %d\n", error);
        FreeSubtree(params);
        FreeSubtree(body);
        free(func_name);
        return NULL;
    }

    Node* func_node = FUNC_DECL(func_name, params, body);
    free(func_name);

    return func_node;
}

// Return ::= 'Rakamakafo' [ Expression ] ';'
static Node* GetReturn(Token* tokens, int* index, int token_count, ParserContext* context)
{
    if (!ExpectToken(tokens, index, token_count, TOKEN_RETURN, "Expected 'Rakamakafo'"))
        return NULL;

    Node* return_expr = NULL;
    if (tokens[*index].type != TOKEN_SEMICOLON)
    {
        return_expr = GetExpression(tokens, index, token_count, context);
        if (!return_expr) return NULL;
    }

    if (!ExpectToken(tokens, index, token_count, TOKEN_SEMICOLON, "Expected ';' after return"))
    {
        FreeSubtree(return_expr);
        return NULL;
    }

    return RETURN(return_expr);
}

// FunCall  ::= Identifier '(' Arguments ')'
// Arguments ::= ε | Expression { ',' Expression }*
static Node* GetFuncCall(Token* tokens, int* index, int token_count, ParserContext* context)
{
    DEBUG_PRINT("\n index: %d\n token_count: %d\n tokens[*index].type: %d\n", *index, token_count, tokens[*index].type);

    if (*index >= token_count || tokens[*index].type != TOKEN_IDENTIFIER)
    {
        printf("Error: Expected function name\n");
        DEBUG_PRINT("at %d index", *index);
        return NULL;
    }

    char* func_name = strdup(tokens[*index].value.identifier);
    (*index)++;

    if (*index >= token_count || tokens[*index].type != TOKEN_LPAREN)
    {
        free(func_name);
        (*index)--;
        return NULL;
    }

    FunctionInfo* func_info = FindFunction(&(context->func_table), func_name);
    if (!func_info)
    {
        fprintf(stderr, "Error: Function '%s' not declared\n", func_name);
        free(func_name);
        (*index)--;
        return NULL;
    }

    // пропускаем '('
    (*index)++;

    Node* args = NULL;
    Node* last_arg = NULL;
    int arg_count = 0;

    if (tokens[*index].type != TOKEN_RPAREN)
    {
        Node* arg = GetExpression(tokens, index, token_count, context);
        if (!arg)
        {
            free(func_name);
            return NULL;
        }
        args = arg;
        last_arg = arg;
        arg_count++;

        while (*index < token_count && tokens[*index].type == TOKEN_COMMA)
        {
            (*index)++;
            arg = GetExpression(tokens, index, token_count, context);
            if (!arg)
            {
                FreeSubtree(args);
                free(func_name);
                return NULL;
            }
            arg_count++;

            Node* seq = SEQ(last_arg, arg);
            last_arg = seq;
        }
    }

    if (arg_count != func_info->param_count) //проверяем количество аргументов
    {
        fprintf(stderr, "Error: Function '%s' expects %d arguments, but %d provided\n",
               func_name, func_info->param_count, arg_count);
        FreeSubtree(args);
        free(func_name);
        return NULL;
    }

    if (!ExpectToken(tokens, index, token_count, TOKEN_RPAREN, "Expected ')' after arguments"))
    {
        FreeSubtree(args);
        free(func_name);
        return NULL;
    }

    Node* call_node = FUNC_CALL(func_name, args);
    free(func_name);

    return call_node;
}
//===============================

static Node* GetStatement(Token* tokens, int* index, int token_count, ParserContext* context)
{
    if (*index >= token_count)
    {
        return NULL;
    }

    TokenType type = tokens[*index].type;

    if (type == TOKEN_LBRACE)
    {
        return GetBlock(tokens, index, token_count, context);
    }
    else if (type == TOKEN_IF)
    {
        return GetIfStatement(tokens, index, token_count, context);
    }
    else if (type == TOKEN_WHILE)
    {
        return GetWhileStatement(tokens, index, token_count, context);
    }
    else if (type == TOKEN_RETURN)
    {
        return GetReturn(tokens, index, token_count, context);
    }
    else if (type == TOKEN_IDENTIFIER)
    {
        int saved_index = *index;

        char* ident_name = strdup(tokens[*index].value.identifier);
        (*index)++;

        if (*index < token_count && tokens[*index].type == TOKEN_LPAREN)
        {
            (*index) = saved_index;
            free(ident_name);

            Node* func_call = GetFuncCall(tokens, index, token_count, context);
            if (func_call)
            {
                if (tokens[*index].type == TOKEN_SEMICOLON)
                {
                    (*index)++;
                    return func_call;
                }
                else
                {
                    FreeSubtree(func_call);
                    (*index) = saved_index;
                    return GetAssignmentStatement(tokens, index, token_count, context);
                }
            }
            else
            {
                (*index) = saved_index;
                return GetAssignmentStatement(tokens, index, token_count, context);
            }
        }
        else
        {
            (*index) = saved_index;
            free(ident_name);

            return GetAssignmentStatement(tokens, index, token_count, context);
        }
    }

    fprintf(stderr, "Error: Unexpected token in statement\n");
    return NULL;
}

static Node* GetBlock(Token* tokens, int* index, int token_count, ParserContext* context)
{
    if (!ExpectToken(tokens, index, token_count, TOKEN_LBRACE, "Expected '{'"))
    {
        return NULL;
    }

    if (CheckToken(tokens, *index, token_count, TOKEN_RBRACE))
    {
        (*index)++;
        return EMPTY();
    }

    Node* first_stmt = GetStatement(tokens, index, token_count, context);
    if (!first_stmt)
    {
        return NULL;
    }

    Node* current = first_stmt;

    while (!CheckToken(tokens, *index, token_count, TOKEN_RBRACE))
    {
        Node* next_stmt = GetStatement(tokens, index, token_count, context);
        if (!next_stmt)
        {
            FreeSubtree(current);
            return NULL;
        }

        current = SEQ(current, next_stmt);
    }

    if (!ExpectToken(tokens, index, token_count, TOKEN_RBRACE, "Expected '}'"))
    {
        FreeSubtree(current);
        return NULL;
    }

    return current; //FIXME
}

// IfStatement ::= 'Izum-Factor' '(' Expression ')' Block
static Node* GetIfStatement(Token* tokens, int* index, int token_count, ParserContext* context)
{
    if (!ExpectToken(tokens, index, token_count, TOKEN_IF, "Expected 'Izum-Factor'"))
        return NULL;

    if (!ExpectToken(tokens, index, token_count, TOKEN_LPAREN, "Expected '(' after 'if'"))
        return NULL;

    Node* condition = GetExpression(tokens, index, token_count, context);
    if (!condition) return NULL;

    if (!ExpectToken(tokens, index, token_count, TOKEN_RPAREN, "Expected ')' after condition"))
    {
        FreeSubtree(condition);
        return NULL;
    }

    Node* body = GetBlock(tokens, index, token_count, context);
    if (!body)
    {
        FreeSubtree(condition);
        return NULL;
    }

    return IF(condition, body);
}

// WhileStatement ::= 'Vova-Arbolit-Lechit-govnocod' '(' Expression ')' Block
static Node* GetWhileStatement(Token* tokens, int* index, int token_count, ParserContext* context)
{
    if (!ExpectToken(tokens, index, token_count, TOKEN_WHILE, "Expected 'Vova-Arbolit-Lechit-govnocod'"))
        return NULL;

    if (!ExpectToken(tokens, index, token_count, TOKEN_LPAREN, "Expected '(' after while"))
        return NULL;

    Node* condition = GetExpression(tokens, index, token_count, context);
    if (!condition) return NULL;

    if (!ExpectToken(tokens, index, token_count, TOKEN_RPAREN, "Expected ')' after condition"))
    {
        FreeSubtree(condition);
        return NULL;
    }

    Node* body = GetBlock(tokens, index, token_count, context);
    if (!body)
    {
        FreeSubtree(condition);
        return NULL;
    }

    return WHILE(condition, body);
}

static Node* GetAssignmentStatement(Token* tokens, int* index, int token_count, ParserContext* context)
{
    if (*index >= token_count || tokens[*index].type != TOKEN_IDENTIFIER)
    {
        fprintf(stderr, "Error: Expected identifier in assignment\n");
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

    Node* expr = GetExpression(tokens, index, token_count, context);
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

    return ASSIGN(var_node, expr);
}

// =========================================
// Выражения
// =========================================

// Expression ::= LogicOr
static Node* GetExpression(Token* tokens, int* index, int token_count, ParserContext* context)
{
    return GetLogicOr(tokens, index, token_count, context);
}

static Node* GetLogicOr(Token* tokens, int* index, int token_count, ParserContext* context)
{
    Node* left = GetLogicAnd(tokens, index, token_count, context);
    if (!left) return NULL;

    while (*index < token_count && tokens[*index].type == TOKEN_OR)
    {
        (*index)++; //пропускаем '||'
        Node* right = GetLogicAnd(tokens, index, token_count, context);
        if (!right)
        {
            FreeSubtree(left);
            return NULL;
        }
        left = OR(left, right);
    }
    return left;
}

// LogicAnd ::= LogicEqual {'&&' LogicEqual}*
static Node* GetLogicAnd(Token* tokens, int* index, int token_count, ParserContext* context)
{
    Node* left = GetLogicEqual(tokens, index, token_count, context);
    if (!left) return NULL;

    while (*index < token_count && tokens[*index].type == TOKEN_AND)
    {
        (*index)++; // пропускаем '&&'
        Node* right = GetLogicEqual(tokens, index, token_count, context);
        if (!right)
        {
            FreeSubtree(left);
            return NULL;
        }
        left = AND(left, right);
    }
    return left;
}

// LogicEqual ::= LogicCompare {('=='|'!=') LogicCompare}*
static Node* GetLogicEqual(Token* tokens, int* index, int token_count, ParserContext* context)
{
    Node* left = GetLogicCompare(tokens, index, token_count, context);
    if (!left) return NULL;

    while (*index < token_count)
    {
        TokenType type = tokens[*index].type;
        if (type != TOKEN_EQUAL && type != TOKEN_NOT_EQUAL)
            break;

        (*index)++;
        Node* right = GetLogicCompare(tokens, index, token_count, context);
        if (!right)
        {
            FreeSubtree(left);
            return NULL;
        }

        if (type == TOKEN_EQUAL)
            left = EQ(left, right);
        else
            left = NE(left, right);
    }
    return left;
}

//switch сделать, который переводит один енам в другой
//дслку нахуй

// LogicCompare ::= AddOrSub {('<'|'>'|'<='|'>=') AddOrSub}*
//FIXME взорвать нахуй это место, навалил говна дико, завтра приберу
static Node* GetLogicCompare(Token* tokens, int* index, int token_count, ParserContext* context)
{
    Node* left = GetAddOrSub(tokens, index, token_count, context);
    if (!left) return NULL;

    while (*index < token_count)
    {
        TokenType token_type = tokens[*index].type;

        if (token_type != TOKEN_LESS       && token_type != TOKEN_GREATER &&
            token_type != TOKEN_LESS_EQUAL && token_type != TOKEN_GREATER_EQUAL)
        {
            break;
        }

        (*index)++;

        Node* right = GetAddOrSub(tokens, index, token_count, context);
        if (!right)
        {
            FreeSubtree(left);
            return NULL;
        }
        NodeType node_type = TranslateFromTokenTypeToNodeType(token_type);
        left = CreateNode(node_type, (ValueOfTreeElement){0}, (left), (right));
    }
    return left;
    //     // if (type == TOKEN_LESS)
    //     {
    //         (*index)++;
    //         Node* right = GetAddOrSub(tokens, index, token_count, context);
    //         if (!right)
    //         {
    //             FreeSubtree(left);
    //             return NULL;
    //         }
    //         left = LT(left, right);
    //     }
    //     else if (type == TOKEN_GREATER)
    //     {
    //         (*index)++;
    //         Node* right = GetAddOrSub(tokens, index, token_count, context);
    //         if (!right)
    //         {
    //             FreeSubtree(left);
    //             return NULL;
    //         }
    //         left = GT(left, right);
    //     }
    //     else if (type == TOKEN_LESS_EQUAL)
    //     {
    //         (*index)++;
    //         Node* right = GetAddOrSub(tokens, index, token_count, context);
    //         if (!right)
    //         {
    //             FreeSubtree(left);
    //             return NULL;
    //         }
    //         left = LE(left, right);
    //     }
    //     else if (type == TOKEN_GREATER_EQUAL)
    //     {
    //         (*index)++;
    //         Node* right = GetAddOrSub(tokens, index, token_count, context);
    //         if (!right)
    //         {
    //             FreeSubtree(left);
    //             return NULL;
    //         }
    //         left = GE(left, right);
    //     }
    //     else
    //     {
    //         break;
    //     }
    // }
    // return left;
}

// AddOrSub ::= Term {('+'|'-') Term}
static Node* GetAddOrSub(Token* tokens, int* index, int token_count, ParserContext* context)
{
    Node* left = GetTerm(tokens, index, token_count, context);
    if (!left) return NULL;

    while (*index < token_count)
    {
        TokenType type = tokens[*index].type;
        if (type != TOKEN_PLUS && type != TOKEN_MINUS)
            break;

        (*index)++;

        Node* right = GetTerm(tokens, index, token_count, context);
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
static Node* GetTerm(Token* tokens, int* index, int token_count, ParserContext* context)
{
    Node* left = GetFactor(tokens, index, token_count, context);
    if (!left) return NULL;

    while (*index < token_count)
    {
        TokenType type = tokens[*index].type;

        if (type != TOKEN_MULTIPLY && type != TOKEN_DIVIDE)
            break;

        (*index)++;

        Node* right = GetFactor(tokens, index, token_count, context);
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
static Node* GetFactor(Token* tokens, int* index, int token_count, ParserContext* context)
{
    if (*index >= token_count)
        return NULL;

    if (tokens[*index].type == TOKEN_NOT)
    {
        (*index)++;
        Node* operand = GetFactor(tokens, index, token_count, context);
        if (!operand)
            return NULL;

        Node* not_node = NOT(operand);
        if (!not_node)
        {
            FreeSubtree(operand);
            return NULL;
        }
        return not_node;
    }

    if (tokens[*index].type == TOKEN_LPAREN)
    {
        (*index)++; // пропускаем '('

        Node* expr = GetExpression(tokens, index, token_count, context);
        if (!expr) return NULL;

        if (*index >= token_count || tokens[*index].type != TOKEN_RPAREN)
        {
            FreeSubtree(expr);
            fprintf(stderr, "Error: Expected ')'\n");
            return NULL;
        }

        (*index)++; // пропускаем ')'
        return expr;
    }

    if (tokens[*index].type == TOKEN_NUMBER)
    {
        double val = tokens[*index].value.number;
        (*index)++;

        Node* num_node = NUM(val);
        if (!num_node)
        {
            fprintf(stderr, "Error creating number node\n");
            return NULL;
        }
        return num_node;
    }

    if (tokens[*index].type == TOKEN_IDENTIFIER)
    {
        int saved_index = *index;
        char* name = strdup(tokens[*index].value.identifier);
        (*index)++;

        if (*index < token_count && tokens[*index].type == TOKEN_LPAREN) //проверка на вызов функции
        {
            *index = saved_index;
            free(name);
            return GetFuncCall(tokens, index, token_count, context);
        }
        else
        {
            TreeErrorType error = AddVariable(&(context->var_table), name);
            if (error != TREE_ERROR_NO && error != TREE_ERROR_VARIABLE_ALREADY_EXISTS &&
                error != TREE_ERROR_REDEFINITION_VARIABLE)
            {
                fprintf(stderr, "Error adding variable to table: %d\n", error);
                free(name);
                return NULL;
            }

            Node* var_node = VAR(name);
            free(name);

            if (!var_node)
            {
                fprintf(stderr, "Error creating variable node\n");
                return NULL;
            }

            return var_node;
        }
    }

    fprintf(stderr, "Error: Expected number, variable, or '(' in expression\n");
    return NULL;
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

Node* ReadProgramFromFile(const char* filename, ParserContext* context)
{
    FILE* file = fopen(filename, "r");
    if (!file)
    {
        fprintf(stderr, "Cannot open file: %s\n", filename);
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

    Node* result = GetProgram(buffer, context);

    free(buffer);
    return result;
}
