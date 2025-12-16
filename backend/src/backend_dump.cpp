#include "backend_dump.h"
#include <stdio.h>
#include <string.h>
#include "tree_common.h"
#include "language_base.h"

static void PrintNode(Node* node, int depth, FILE* file)
{
    if (!node) return;

    for (int i = 0; i < depth; i++) fprintf(file, "  ");

    switch (node->type)
    {
        case NODE_OP:
            fprintf(file, "OP:");
            switch (node->data.op_value)
            {
                case OP_ADD: fprintf(file, "ADD"); break;
                case OP_SUB: fprintf(file, "SUB"); break;
                case OP_MUL: fprintf(file, "MUL"); break;
                case OP_DIV: fprintf(file, "DIV"); break;
                default: fprintf(file, "?");
            }
            break;

        case NODE_NUM:
            fprintf(file, "NUM:%.2f", node->data.num_value);
            break;

        case NODE_VAR:
            fprintf(file, "VAR:%s", node->data.var_definition.name);
            break;

        case NODE_FUNC_DECL:
            fprintf(file, "FUNC_DECL:%s", node->data.func_name);
            break;

        case NODE_FUNC_CALL:
            fprintf(file, "FUNC_CALL:%s", node->data.func_call.name);
            break;

        case NODE_IF: fprintf(file, "IF"); break;
        case NODE_WHILE: fprintf(file, "WHILE"); break;
        case NODE_SEQUENCE: fprintf(file, "SEQ"); break;
        case NODE_ASSIGN: fprintf(file, "ASSIGN"); break;
        case NODE_EQUAL: fprintf(file, "EQUAL"); break;
        case NODE_GREATER: fprintf(file, "GREATER"); break;
        case NODE_RETURN: fprintf(file, "RETURN"); break;

        default:
            fprintf(file, "UNKNOWN:%d", node->type);
    }

    fprintf(file, " (addr:%p)\n", (void*)node);

    // Рекурсивно
    PrintNode(node->left, depth + 1, file);
    PrintNode(node->right, depth + 1, file);
}

void BackendTreeDump(Tree* tree, const char* filename)
{
    if (!tree || !filename) return;

    FILE* file = fopen(filename, "w");
    if (!file) return;

    fprintf(file, "=== TREE DUMP ===\n");
    fprintf(file, "Total nodes: %zu\n", tree->size);
    fprintf(file, "Root: %p\n\n", (void*)tree->root);

    if (tree->root)
    {
        PrintNode(tree->root, 0, file);
    }
    else
    {
        fprintf(file, "Empty tree\n");
    }

    fclose(file);
    printf("Tree dumped to %s\n", filename);
}
