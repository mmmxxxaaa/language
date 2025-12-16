#include "language_base.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
{
    Node* node = (Node*)calloc(1, sizeof(Node));
    if (!node)
        return NULL;

    node->type = type;
    node->left = left;
    node->right = right;
    node->parent = NULL;

    node->data = (ValueOfTreeElement){0};

    switch (type)
    {
        case NODE_NUM:
            node->data.num_value = data.num_value;
            node->priority = 0;
            break;

        case NODE_OP:
            node->data.op_value = data.op_value;
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

        case NODE_VAR:
            if (data.var_definition.name != NULL)
            {
                node->data.var_definition.name = strdup(data.var_definition.name);
                if (!node->data.var_definition.name)
                {
                    free(node);
                    return NULL;
                }
                node->data.var_definition.hash = ComputeHash(data.var_definition.name);
            }
            node->priority = 0;
            break;

        case NODE_FUNC_DECL:
            if (data.func_name != NULL)
            {
                node->data.func_name = strdup(data.func_name);
                if (!node->data.func_name)
                {
                    free(node);
                    return NULL;
                }
            }
            node->priority = 0;
            break;

        case NODE_FUNC_CALL:
            if (data.func_call.name != NULL)
            {
                node->data.func_call.name = strdup(data.func_call.name);
                if (!node->data.func_call.name)
                {
                    free(node);
                    return NULL;
                }
            }
            node->priority = 0;
            break;

        default:
            node->priority = 0;
            node->data = data;  /
            break;
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

unsigned int ComputeHash(const char* str) //djb2
{
    unsigned int hash = 5381;
    int c = 0;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + (unsigned char)c; //умножаем на 33 без умножения
    return hash;
}
