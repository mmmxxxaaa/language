#include "output_infix.h"
#include <stdio.h>
#include "tree_common_frontend.h"

static ASTMapping ast_mappings[] = {
    {NODE_OP, "OP"},
    {NODE_EQUAL, "EQUAL"},
    {NODE_NOT_EQUAL, "NOT_EQUAL"},
    {NODE_LESS, "LESS"},
    {NODE_LESS_EQUAL, "LESS_EQUAL"},
    {NODE_GREATER, "GREATER"},
    {NODE_GREATER_EQUAL, "GREATER_EQUAL"},
    {NODE_AND, "AND"},
    {NODE_OR, "OR"},
    {NODE_NOT, "NOT"},
    {NODE_IF, "IF"},
    {NODE_WHILE, "WHILE"},
    {NODE_FUNC_DECL, "FUNC"},
    {NODE_FUNC_CALL, "CALL"},
    {NODE_RETURN, "RETURN"},
    {NODE_ASSIGN, "ASSIGN"},
    {NODE_SEQUENCE, "SEQ"},
};

static const char* GetOpStringFromOpType(OperationType op_type)
{
    switch (op_type)
    {
        case OP_ADD: return "ADD";
        case OP_SUB: return "SUB";
        case OP_MUL: return "MUL";
        case OP_DIV: return "DIV";
        default: return "UNKNOWN_OP";
    }
}

static const char* GetASTStringFromNode(Node* node)
{
    for (size_t i = 0; i < sizeof(ast_mappings) / sizeof(ast_mappings[0]); i++)
    {
        if (ast_mappings[i].node_type == node->type)
        {
            if (node->type == NODE_OP)
            {
                return GetOpStringFromOpType(node->data.op_value);
            }
            return ast_mappings[i].ast_str;
        }
    }
    return "UNKNOWN";
}

static void PrintIndent(FILE* file, int depth)
{
    for (int i = 0; i < depth; i++)
        fprintf(file, "  ");
}

static void PrintASTNode(Node* node, FILE* file, int depth)
{
    if (!node)
    {
        PrintIndent(file, depth);
        fprintf(file, "nil\n");
        return;
    }

    switch (node->type)
    {
        case NODE_NUM:
            PrintIndent(file, depth);
            fprintf(file, "%g\n", node->data.num_value);
            break;

        case NODE_VAR:
            PrintIndent(file, depth);
            if (node->data.var_definition.name)
                fprintf(file, "%s\n", node->data.var_definition.name);
            else
                fprintf(file, "nil\n");
            break;

        case NODE_EMPTY:
            PrintIndent(file, depth);
            fprintf(file, "nil\n");
            break;

        case NODE_OP:
        case NODE_EQUAL:
        case NODE_NOT_EQUAL:
        case NODE_LESS:
        case NODE_LESS_EQUAL:
        case NODE_GREATER:
        case NODE_GREATER_EQUAL:
        case NODE_AND:
        case NODE_OR:
        case NODE_NOT:
        case NODE_IF:
        case NODE_WHILE:
        case NODE_RETURN:
        case NODE_ASSIGN:
            PrintIndent(file, depth);
            fprintf(file, "(%s\n", GetASTStringFromNode(node));

            if (node->left)
                PrintASTNode(node->left, file, depth + 1);
            else
            {
                PrintIndent(file, depth + 1);
                fprintf(file, "nil\n");
            }

            if (node->right && node->type != NODE_NOT && node->type != NODE_RETURN)
                PrintASTNode(node->right, file, depth + 1);
            else if (node->type != NODE_NOT && node->type != NODE_RETURN)
            {
                PrintIndent(file, depth + 1);
                fprintf(file, "nil\n");
            }

            PrintIndent(file, depth);
            fprintf(file, ")\n");
            break;

        case NODE_FUNC_DECL:
            PrintIndent(file, depth);
            fprintf(file, "(FUNC\n");

            PrintIndent(file, depth + 1);
            if (node->data.func_name)
                fprintf(file, "%s\n", node->data.func_name);
            else
                fprintf(file, "nil\n");

            if (node->left)
                PrintASTNode(node->left, file, depth + 1);
            else
            {
                PrintIndent(file, depth + 1);
                fprintf(file, "nil\n");
            }

            if (node->right)
                PrintASTNode(node->right, file, depth + 1);
            else
            {
                PrintIndent(file, depth + 1);
                fprintf(file, "nil\n");
            }

            PrintIndent(file, depth);
            fprintf(file, ")\n");
            break;

        case NODE_FUNC_CALL:
            PrintIndent(file, depth);
            fprintf(file, "(CALL\n");

            PrintIndent(file, depth + 1);
            if (node->data.func_call.name)
                fprintf(file, "%s\n", node->data.func_call.name);
            else
                fprintf(file, "nil\n");

            if (node->left)
                PrintASTNode(node->left, file, depth + 1);
            else
            {
                PrintIndent(file, depth + 1);
                fprintf(file, "nil\n");
            }

            PrintIndent(file, depth);
            fprintf(file, ")\n");
            break;

        case NODE_SEQUENCE:
            PrintIndent(file, depth);
            fprintf(file, "(SEQ\n");

            if (node->left)
                PrintASTNode(node->left, file, depth + 1);
            else
            {
                PrintIndent(file, depth + 1);
                fprintf(file, "nil\n");
            }

            if (node->right)
                PrintASTNode(node->right, file, depth + 1);
            else
            {
                PrintIndent(file, depth + 1);
                fprintf(file, "nil\n");
            }

            PrintIndent(file, depth);
            fprintf(file, ")\n");
            break;

        default:
            PrintIndent(file, depth);
            fprintf(file, "UNKNOWN_NODE_%d\n", node->type);
            break;
    }
}

void PrintASTToFile(Node* node, FILE* file)
{
    if (!file || !node)
        return;

    PrintASTNode(node, file, 0);
}
