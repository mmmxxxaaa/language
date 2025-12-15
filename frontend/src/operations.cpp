#include "operations.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tree_common.h"
#include "variable_parse.h"
#include "tree_base.h"
#include "DSL.h"

void FreeSubtree(Node* node)
{
    if (node == NULL)
        return;

    FreeSubtree(node->left);
    FreeSubtree(node->right);

    switch (node->type)
    {
        case NODE_VAR:
            if (node->data.var_definition.name != NULL)
                free(node->data.var_definition.name);
            break;

        case NODE_FUNC_DECL:
            if (node->data.func_name != NULL)
                free(node->data.func_name);
            break;

        case NODE_FUNC_CALL:
            if (node->data.func_call.name != NULL)
                free(node->data.func_call.name);
            break;

        default:
            break;
    }

    free(node);
}

Node* CreateNode(NodeType type, ValueOfTreeElement data, Node* left, Node* right)
{ //FIXME ассерты
    Node* node = (Node*)calloc(1, sizeof(Node));
    if (!node)
        return NULL;

    node->type = type;
    node->left = left;
    node->right = right;
    node->parent = NULL;
    node->data = data;

    switch (type)
    {
        case NODE_NUM:
        case NODE_VAR:
            node->priority = 0;
            break;

        case NODE_OP:
            switch (data.op_value)
            {
                case OP_ADD:
                case OP_SUB:
                    node->priority = 1;
                    break;

                case OP_MUL:
                case OP_DIV:
                    node->priority = 2;
                    break;

                default:
                    node->priority = 0;
                    break;
            }
            break;

        default:
            node->priority = 0;
            break;
    }

    if (type == NODE_VAR && data.var_definition.name != NULL)
    {
        node->data.var_definition.name = data.var_definition.name;
        node->data.var_definition.hash = ComputeHash(data.var_definition.name);
    }

    if (left)
        left->parent = node;
    if (right)
        right->parent = node;

    return node;
}

size_t CountTreeNodes(Node* node)
{
    if (node == NULL)
        return 0;

    return 1 + CountTreeNodes(node->left) + CountTreeNodes(node->right);
}

#include "DSL_undef.h"
