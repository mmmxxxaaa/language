#ifndef DSL_H
#define DSL_H

#include "tree_base.h"
#include "operations.h"
#include "tree_common.h"
#include "variable_parse.h"

// ==================== СОЗДАНИЕ УЗЛОВ ====================
#define NUM(val)     CreateNode(NODE_NUM, (ValueOfTreeElement){.num_value = (val)}, NULL, NULL)
#define VAR(var_name) CreateNode(NODE_VAR, (ValueOfTreeElement){.var_definition = \
                            {.hash = ComputeHash(var_name), .name = strdup(var_name)}}, NULL, NULL)

// Бинарные операции
#define ADD(left, right) CreateNode(NODE_OP, (ValueOfTreeElement){.op_value = OP_ADD}, (left), (right))
#define SUB(left, right) CreateNode(NODE_OP, (ValueOfTreeElement){.op_value = OP_SUB}, (left), (right))
#define MUL(left, right) CreateNode(NODE_OP, (ValueOfTreeElement){.op_value = OP_MUL}, (left), (right))
#define DIV(left, right) CreateNode(NODE_OP, (ValueOfTreeElement){.op_value = OP_DIV}, (left), (right))
#define POW(left, right) CreateNode(NODE_OP, (ValueOfTreeElement){.op_value = OP_POW}, (left), (right))

#define LN(arg)     CreateNode(NODE_OP, (ValueOfTreeElement){.op_value = OP_LN},     NULL, (arg))
#define EXP(arg)    CreateNode(NODE_OP, (ValueOfTreeElement){.op_value = OP_EXP},    NULL, (arg))
#define SQRT(x)     CreateNode(NODE_OP, (ValueOfTreeElement){.op_value = OP_POW}, x, NUM(0.5))

#define ASSIGN(var, expr) CreateNode(NODE_ASSIGN, (ValueOfTreeElement){0}, (var), (expr))

// ==================== ДЛЯ ДИФФЕРЕНЦИРОВАНИЯ ====================
#define U  COPY(node->left)
#define V  COPY(node->right)
#define DU DIFF(node->left, variable_name)
#define DV DIFF(node->right, variable_name)

// ==================== УТИЛИТЫ ====================
#define FREE_NODES(count, ...) \
    do { \
        Node* nodes[] = {__VA_ARGS__}; \
        for (size_t i = 0; i < (count) && i < sizeof(nodes)/sizeof(nodes[0]); i++) \
            if (nodes[i]) FreeSubtree(nodes[i]); \
    } while(0)

#endif // DSL_H
