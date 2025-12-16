#include "language_base.h"
#include "reading_and_building.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

static void SkipSpaces(const char** str);
static char* ReadIdentifier(const char** str);
static double ReadNumber(const char** str);
static NodeType ParseNodeTypeFromKeyword(const char* keyword);
static OperationType ParseOpValue(const char* keyword);
static Node* ParseBinaryOp(const char** str, NodeType node_type, OperationType op_value);
static Node* ParseExpression(const char** str);
static Node* ParseAdd(const char** str);
static Node* ParseSub(const char** str);
static Node* ParseMul(const char** str);
static Node* ParseDiv(const char** str);
static Node* ParseEqual(const char** str);
static Node* ParseNotEqual(const char** str);
static Node* ParseLess(const char** str);
static Node* ParseLessEqual(const char** str);
static Node* ParseGreater(const char** str);
static Node* ParseGreaterEqual(const char** str);
static Node* ParseAnd(const char** str);
static Node* ParseOr(const char** str);
static Node* ParseNot(const char** str);
static Node* ParseIf(const char** str);
static Node* ParseWhile(const char** str);
static Node* ParseFunc(const char** str);
static Node* ParseCall(const char** str);
static Node* ParseReturn(const char** str);
static Node* ParseAssign(const char** str);
static Node* ParseSequence(const char** str);
static Node* ParseNumber(const char** str);
static Node* ParseVariable(const char** str);

// =========================================
// Вспомогательные функции
// =========================================

static void SkipSpaces(const char** str)
{
    while (**str && isspace(**str))
        (*str)++;
}

static char* ReadIdentifier(const char** str)
{
    SkipSpaces(str);

    if (!isalpha(**str) && **str != '_')
        return NULL;

    const char* start = *str;
    while (**str && (isalnum(**str) || **str == '_'))
        (*str)++;

    size_t len = *str - start;
    char* ident = (char*)calloc(len + 1, sizeof(char));
    if (!ident) return NULL;

    strncpy(ident, start, len);
    ident[len] = '\0';

    return ident;
}

static double ReadNumber(const char** str)
{
    SkipSpaces(str);

    double result = 0;
    int decimal = 0;
    double fraction = 1.0;
    int negative = 0;

    if (**str == '-')
    {
        negative = 1;
        (*str)++;
        SkipSpaces(str);
    }

    while (**str && (isdigit(**str) || **str == '.'))
    {
        if (**str == '.')
        {
            decimal = 1;
            (*str)++;
            continue;
        }

        if (!decimal)
        {
            result = result * 10 + (**str - '0');
        }
        else
        {
            fraction /= 10;
            result += (**str - '0') * fraction;
        }
        (*str)++;
    }

    return negative ? -result : result;
}

static NodeType ParseNodeTypeFromKeyword(const char* keyword)
{
    if (strcmp(keyword, "ADD") == 0) return NODE_OP;
    if (strcmp(keyword, "SUB") == 0) return NODE_OP;
    if (strcmp(keyword, "MUL") == 0) return NODE_OP;
    if (strcmp(keyword, "DIV") == 0) return NODE_OP;

    if (strcmp(keyword, "EQUAL") == 0) return NODE_EQUAL;
    if (strcmp(keyword, "NOT_EQUAL") == 0) return NODE_NOT_EQUAL;
    if (strcmp(keyword, "LESS") == 0) return NODE_LESS;
    if (strcmp(keyword, "LESS_EQUAL") == 0) return NODE_LESS_EQUAL;
    if (strcmp(keyword, "GREATER") == 0) return NODE_GREATER;
    if (strcmp(keyword, "GREATER_EQUAL") == 0) return NODE_GREATER_EQUAL;

    if (strcmp(keyword, "AND") == 0) return NODE_AND;
    if (strcmp(keyword, "OR") == 0) return NODE_OR;
    if (strcmp(keyword, "NOT") == 0) return NODE_NOT;

    if (strcmp(keyword, "IF") == 0) return NODE_IF;
    if (strcmp(keyword, "WHILE") == 0) return NODE_WHILE;

    if (strcmp(keyword, "FUNC") == 0) return NODE_FUNC_DECL;
    if (strcmp(keyword, "CALL") == 0) return NODE_FUNC_CALL;
    if (strcmp(keyword, "RETURN") == 0) return NODE_RETURN;

    if (strcmp(keyword, "ASSIGN") == 0) return NODE_ASSIGN;
    if (strcmp(keyword, "SEQ") == 0) return NODE_SEQUENCE;

    return NODE_EMPTY;
}

static OperationType ParseOpValue(const char* keyword)
{
    if (strcmp(keyword, "ADD") == 0) return OP_ADD;
    if (strcmp(keyword, "SUB") == 0) return OP_SUB;
    if (strcmp(keyword, "MUL") == 0) return OP_MUL;
    if (strcmp(keyword, "DIV") == 0) return OP_DIV;
    return OP_ADD;
}

// =========================================
// Парсеры для бинарных операций
// =========================================

static Node* ParseBinaryOp(const char** str, NodeType node_type, OperationType op_value)
{
    Node* left = ParseExpression(str);
    if (!left) return NULL;

    SkipSpaces(str);

    Node* right = ParseExpression(str);
    if (!right)
    {
        FreeSubtree(left);
        return NULL;
    }

    SkipSpaces(str);
    if (**str != ')')
    {
        FreeSubtree(left);
        FreeSubtree(right);
        return NULL;
    }

    (*str)++;

    if (node_type == NODE_OP)
        return CreateNode(node_type, (ValueOfTreeElement){.op_value = op_value}, left, right);
    else
        return CreateNode(node_type, (ValueOfTreeElement){0}, left, right);
}

static Node* ParseAdd(const char** str) { return ParseBinaryOp(str, NODE_OP, OP_ADD); }
static Node* ParseSub(const char** str) { return ParseBinaryOp(str, NODE_OP, OP_SUB); }
static Node* ParseMul(const char** str) { return ParseBinaryOp(str, NODE_OP, OP_MUL); }
static Node* ParseDiv(const char** str) { return ParseBinaryOp(str, NODE_OP, OP_DIV); }
static Node* ParseEqual(const char** str) { return ParseBinaryOp(str, NODE_EQUAL, OP_ADD); }
static Node* ParseNotEqual(const char** str) { return ParseBinaryOp(str, NODE_NOT_EQUAL, OP_ADD); }
static Node* ParseLess(const char** str) { return ParseBinaryOp(str, NODE_LESS, OP_ADD); }
static Node* ParseLessEqual(const char** str) { return ParseBinaryOp(str, NODE_LESS_EQUAL, OP_ADD); }
static Node* ParseGreater(const char** str) { return ParseBinaryOp(str, NODE_GREATER, OP_ADD); }
static Node* ParseGreaterEqual(const char** str) { return ParseBinaryOp(str, NODE_GREATER_EQUAL, OP_ADD); }
static Node* ParseAnd(const char** str) { return ParseBinaryOp(str, NODE_AND, OP_ADD); }
static Node* ParseOr(const char** str) { return ParseBinaryOp(str, NODE_OR, OP_ADD); }

// =========================================
// Парсеры для унарных операций
// =========================================

static Node* ParseNot(const char** str)
{
    Node* operand = ParseExpression(str);
    if (!operand) return NULL;

    SkipSpaces(str);
    if (**str != ')')
    {
        FreeSubtree(operand);
        return NULL;
    }

    (*str)++;

    return CreateNode(NODE_NOT, (ValueOfTreeElement){0}, operand, NULL);
}

// =========================================
// Парсеры для управляющих конструкций
// =========================================

static Node* ParseIf(const char** str)
{
    Node* condition = ParseExpression(str);
    if (!condition) return NULL;

    SkipSpaces(str);

    Node* then_branch = ParseExpression(str);
    if (!then_branch)
    {
        FreeSubtree(condition);
        return NULL;
    }

    SkipSpaces(str);
    if (**str != ')')
    {
        FreeSubtree(condition);
        FreeSubtree(then_branch);
        return NULL;
    }

    (*str)++;

    return CreateNode(NODE_IF, (ValueOfTreeElement){0}, condition, then_branch);
}

static Node* ParseWhile(const char** str)
{
    Node* condition = ParseExpression(str);
    if (!condition) return NULL;

    SkipSpaces(str);

    Node* body = ParseExpression(str);
    if (!body)
    {
        FreeSubtree(condition);
        return NULL;
    }

    SkipSpaces(str);
    if (**str != ')')
    {
        FreeSubtree(condition);
        FreeSubtree(body);
        return NULL;
    }

    (*str)++;

    return CreateNode(NODE_WHILE, (ValueOfTreeElement){0}, condition, body);
}

// =========================================
// Парсеры для функций
// =========================================

static Node* ParseFunc(const char** str)
{
    char* func_name = ReadIdentifier(str);
    if (!func_name) return NULL;

    SkipSpaces(str);

    // читаем список параметров
    Node* params = NULL;
    Node* last_param = NULL;

    while (**str != '\0' && **str != ')' && (isalpha(**str) || **str == '_'))
    {
        char* param_name = ReadIdentifier(str);
        if (!param_name)
        {
            free(func_name);
            FreeSubtree(params);
            return NULL;
        }

        unsigned int param_hash = ComputeHash(param_name);
        Node* param_node = CreateNode(NODE_VAR,
            (ValueOfTreeElement){.var_definition = {.hash = param_hash, .name = param_name}},
            NULL, NULL);
        free(param_name);

        if (!param_node)
        {
            free(func_name);
            FreeSubtree(params);
            return NULL;
        }

        if (!params)
        {
            params = param_node;
            last_param = param_node;
        }
        else
        {
            Node* seq = CreateNode(NODE_SEQUENCE, (ValueOfTreeElement){0}, last_param, param_node);
            last_param = seq;
            if (params == last_param->left)
                params = seq;
        }

        SkipSpaces(str);
    }

    // читаем тело функции
    SkipSpaces(str);
    Node* body = ParseExpression(str);
    if (!body)
    {
        free(func_name);
        FreeSubtree(params);
        return NULL;
    }

    SkipSpaces(str);
    if (**str != ')')
    {
        free(func_name);
        FreeSubtree(params);
        FreeSubtree(body);
        return NULL;
    }

    (*str)++;

    Node* func_node = CreateNode(NODE_FUNC_DECL,
        (ValueOfTreeElement){.func_name = func_name}, params, body);

    return func_node;
}

static Node* ParseCall(const char** str)
{
    char* func_name = ReadIdentifier(str);
    if (!func_name) return NULL;

    SkipSpaces(str);

    Node* args = NULL;
    Node* last_arg = NULL;

    while (**str != '\0' && **str != ')')
    {
        Node* arg = ParseExpression(str);
        if (!arg)
        {
            free(func_name);
            FreeSubtree(args);
            return NULL;
        }

        if (!args)
        {
            args = arg;
            last_arg = arg;
        }
        else
        {
            Node* seq = CreateNode(NODE_SEQUENCE, (ValueOfTreeElement){0}, last_arg, arg);
            last_arg = seq;
            if (args == last_arg->left)
                args = seq;
        }

        SkipSpaces(str);
    }

    if (**str != ')')
    {
        free(func_name);
        FreeSubtree(args);
        return NULL;
    }

    (*str)++;

    Node* call_node = CreateNode(NODE_FUNC_CALL,
        (ValueOfTreeElement){.func_call = {.name = func_name}}, args, NULL);

    return call_node;
}

static Node* ParseReturn(const char** str)
{
    SkipSpaces(str);

    if (**str == ')')
    {
        (*str)++;
        return CreateNode(NODE_RETURN, (ValueOfTreeElement){0}, NULL, NULL);
    }

    Node* expr = ParseExpression(str);
    if (!expr) return NULL;

    SkipSpaces(str);
    if (**str != ')')
    {
        FreeSubtree(expr);
        return NULL;
    }

    (*str)++;

    return CreateNode(NODE_RETURN, (ValueOfTreeElement){0}, expr, NULL);
}

// =========================================
// Парсеры для базовых конструкций
// =========================================

static Node* ParseAssign(const char** str)
{
    char* var_name = ReadIdentifier(str);
    if (!var_name) return NULL;

    unsigned int var_hash = ComputeHash(var_name);
    Node* left = CreateNode(NODE_VAR,
        (ValueOfTreeElement){.var_definition = {.hash = var_hash, .name = var_name}}, NULL, NULL);
    free(var_name);

    if (!left) return NULL;

    SkipSpaces(str);

    Node* right = ParseExpression(str);
    if (!right)
    {
        FreeSubtree(left);
        return NULL;
    }

    SkipSpaces(str);
    if (**str != ')')
    {
        FreeSubtree(left);
        FreeSubtree(right);
        return NULL;
    }

    (*str)++;

    return CreateNode(NODE_ASSIGN, (ValueOfTreeElement){0}, left, right);
}

static Node* ParseSequence(const char** str)
{
    SkipSpaces(str);

    if (**str == ')')
    {
        (*str)++;
        return NULL;
    }

    Node* first = ParseExpression(str);
    if (!first) return NULL;

    SkipSpaces(str);

    if (**str == ')')
    {
        (*str)++;
        return first;
    }

    Node* rest = ParseSequence(str);
    if (!rest)
    {
        FreeSubtree(first);
        return NULL;
    }

    return CreateNode(NODE_SEQUENCE, (ValueOfTreeElement){0}, first, rest);
}

static Node* ParseNumber(const char** str)
{
    double value = ReadNumber(str);
    return CreateNode(NODE_NUM, (ValueOfTreeElement){.num_value = value}, NULL, NULL);
}

static Node* ParseVariable(const char** str)
{
    char* var_name = ReadIdentifier(str);
    if (!var_name) return NULL;

    unsigned int var_hash = ComputeHash(var_name);
    Node* node = CreateNode(NODE_VAR,
        (ValueOfTreeElement){.var_definition = {.hash = var_hash, .name = var_name}}, NULL, NULL);
    free(var_name);

    return node;
}

// =========================================
// Главный парсер выражений
// =========================================

static Node* ParseExpression(const char** str)
{
    SkipSpaces(str);

    if (**str == '(')
    {
        (*str)++;
        SkipSpaces(str);

        const char* saved = *str;
        char* keyword = ReadIdentifier(str);
        if (!keyword)
        {
            *str = saved;
            Node* nested = ParseExpression(str);
            if (nested)
            {
                SkipSpaces(str);
                if (**str == ')') (*str)++;
            }
            return nested;
        }

        Node* result = NULL;
        NodeType node_type = ParseNodeTypeFromKeyword(keyword);

        switch (node_type)
        {
            case NODE_OP:
                if      (strcmp(keyword, "ADD") == 0) result = ParseAdd(str);
                else if (strcmp(keyword, "SUB") == 0) result = ParseSub(str);
                else if (strcmp(keyword, "MUL") == 0) result = ParseMul(str);
                else if (strcmp(keyword, "DIV") == 0) result = ParseDiv(str);
                break;

            case NODE_EQUAL: result = ParseEqual(str); break;
            case NODE_NOT_EQUAL: result = ParseNotEqual(str); break;
            case NODE_LESS: result = ParseLess(str); break;
            case NODE_LESS_EQUAL: result = ParseLessEqual(str); break;
            case NODE_GREATER: result = ParseGreater(str); break;
            case NODE_GREATER_EQUAL: result = ParseGreaterEqual(str); break;
            case NODE_AND: result = ParseAnd(str); break;
            case NODE_OR: result = ParseOr(str); break;
            case NODE_NOT: result = ParseNot(str); break;
            case NODE_IF: result = ParseIf(str); break;
            case NODE_WHILE: result = ParseWhile(str); break;
            case NODE_FUNC_DECL: result = ParseFunc(str); break;
            case NODE_FUNC_CALL: result = ParseCall(str); break;
            case NODE_RETURN: result = ParseReturn(str); break;
            case NODE_ASSIGN: result = ParseAssign(str); break;
            case NODE_SEQUENCE: result = ParseSequence(str); break;
            default:
                // неизвестное ключевое слово, пробуем как вложенное выражение
                *str = saved;
                free(keyword);
                result = ParseExpression(str);
                if (result)
                {
                    SkipSpaces(str);
                    if (**str == ')') (*str)++;
                }
                break;
        }

        free(keyword);
        return result;
    }
    else if (isdigit(**str) || **str == '.' || **str == '-')
    {
        return ParseNumber(str);
    }
    else if (isalpha(**str) || **str == '_')
    {
        return ParseVariable(str);
    }

    return NULL;
}

// =========================================
// Основная функция бэкенда
// =========================================

Tree* BuildTreeFromExpression(const char* expression)
{
    if (!expression) return NULL;

    const char* ptr = expression;
    Node* root = ParseExpression(&ptr);
    if (!root) return NULL;

    SkipSpaces(&ptr);
    if (*ptr != '\0')
    {
        FreeSubtree(root);
        return NULL;
    }

    Tree* tree = (Tree*)malloc(sizeof(Tree));
    if (!tree)
    {
        FreeSubtree(root);
        return NULL;
    }

    tree->root = root;
    tree->size = CountTreeNodes(root);
    tree->file_buffer = NULL;

    return tree;
}
