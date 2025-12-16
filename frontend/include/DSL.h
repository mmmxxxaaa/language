#ifndef DSL_H
#define DSL_H

#include "language_base.h"
#include "tree_common_frontend.h"
#include "tree_common.h"
#include "variable_parse.h"

// ==================== СОЗДАНИЕ УЗЛОВ ====================
#define NUM(val)     CreateNode(NODE_NUM, (ValueOfTreeElement){.num_value = (val)}, NULL, NULL)
#define VAR(var_name) CreateNode(NODE_VAR, (ValueOfTreeElement){.var_definition = \
                            {.hash = ComputeHash(var_name), .name = strdup(var_name)}}, NULL, NULL)
#define EMPTY() CreateNode(NODE_EMPTY, (ValueOfTreeElement){0}, NULL, NULL)


// Бинарные операции
#define ADD(left, right) CreateNode(NODE_OP, (ValueOfTreeElement){.op_value = OP_ADD}, (left), (right))
#define SUB(left, right) CreateNode(NODE_OP, (ValueOfTreeElement){.op_value = OP_SUB}, (left), (right))
#define MUL(left, right) CreateNode(NODE_OP, (ValueOfTreeElement){.op_value = OP_MUL}, (left), (right))
#define DIV(left, right) CreateNode(NODE_OP, (ValueOfTreeElement){.op_value = OP_DIV}, (left), (right))

#define EQ(left, right) CreateNode(NODE_EQUAL, (ValueOfTreeElement){0}, (left), (right))
#define NE(left, right) CreateNode(NODE_NOT_EQUAL, (ValueOfTreeElement){0}, (left), (right))
#define LT(left, right) CreateNode(NODE_LESS, (ValueOfTreeElement){0}, (left), (right))
#define GT(left, right) CreateNode(NODE_GREATER, (ValueOfTreeElement){0}, (left), (right))
#define LE(left, right) CreateNode(NODE_LESS_EQUAL, (ValueOfTreeElement){0}, (left), (right))
#define GE(left, right) CreateNode(NODE_GREATER_EQUAL, (ValueOfTreeElement){0}, (left), (right))

#define AND(left, right) CreateNode(NODE_AND, (ValueOfTreeElement){0}, (left), (right))
#define OR(left, right) CreateNode(NODE_OR, (ValueOfTreeElement){0}, (left), (right))
#define NOT(expr) CreateNode(NODE_NOT, (ValueOfTreeElement){0}, (expr), NULL)

#define IF(cond, body) CreateNode(NODE_IF, (ValueOfTreeElement){0}, (cond), (body))
#define WHILE(cond, body) CreateNode(NODE_WHILE, (ValueOfTreeElement){0}, (cond), (body))
#define ASSIGN(var, expr) CreateNode(NODE_ASSIGN, (ValueOfTreeElement){0}, (var), (expr))
#define RETURN(expr) CreateNode(NODE_RETURN, (ValueOfTreeElement){0}, (expr), NULL)
#define SEQ(first, second) CreateNode(NODE_SEQUENCE, (ValueOfTreeElement){0}, (first), (second))

#define FUNC_DECL(name, params, body) CreateNode(NODE_FUNC_DECL, (ValueOfTreeElement){.func_name = strdup(name)}, (params), (body))
#define FUNC_CALL(name, args) CreateNode(NODE_FUNC_CALL, (ValueOfTreeElement){.func_name = strdup(name)}, (args), NULL)

// ==================== УТИЛИТЫ ====================
#define FREE_NODES(count, ...) \
    do { \
        Node* nodes[] = {__VA_ARGS__}; \
        for (size_t i = 0; i < (count) && i < sizeof(nodes)/sizeof(nodes[0]); i++) \
            if (nodes[i]) FreeSubtree(nodes[i]); \
    } while(0)

#endif // DSL_H
