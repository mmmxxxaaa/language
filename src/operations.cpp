#include "operations.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include "tree_error_types.h"
#include "tree_common.h"
#include "variable_parse.h"
#include "tree_base.h"
#include "DSL.h"
#include "logic_functions.h"

// ==================== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ====================

void FreeSubtree(Node* node)
{
    if (node == NULL)
        return;

    FreeSubtree(node->left);
    FreeSubtree(node->right);

    if (node->type == NODE_VAR && node->data.var_definition.name != NULL)
        free(node->data.var_definition.name);

    free(node);
}

static TreeErrorType EvaluateTreeRecursive(Node* node, VariableTable* var_table, double* result, int depth)
{
    if (node == NULL)
        return TREE_ERROR_NULL_PTR;

    if (result == NULL)
        return TREE_ERROR_NULL_PTR;

    switch (node->type)
    {
        case NODE_NUM:
            *result = node->data.num_value;
            return TREE_ERROR_NO;

        case NODE_OP:
            {
                double left_result = 0, right_result = 0;
                TreeErrorType error = TREE_ERROR_NO;

                if (node->right == NULL)
                    return TREE_ERROR_NULL_PTR;

                if (is_binary(node->data.op_value))
                {
                    if (node->left == NULL)
                        return TREE_ERROR_NULL_PTR;

                    error = EvaluateTreeRecursive(node->left, var_table, &left_result, depth + 1);
                    if (error != TREE_ERROR_NO)
                        return error;
                }

                error = EvaluateTreeRecursive(node->right, var_table, &right_result, depth + 1);
                if (error != TREE_ERROR_NO)
                    return error;

                switch (node->data.op_value)
                {
                    case OP_ADD:
                        *result = left_result + right_result;
                        break;
                    case OP_SUB:
                        *result = left_result - right_result;
                        break;
                    case OP_MUL:
                        *result = left_result * right_result;
                        break;
                    case OP_DIV:
                        if (is_zero(right_result))
                            return TREE_ERROR_DIVISION_BY_ZERO;
                        *result = left_result / right_result;
                        break;
                    case OP_POW:
                        *result = pow(left_result, right_result);
                        break;
                    case OP_LN:
                        if (right_result <= 0)
                            return TREE_ERROR_MATH_DOMAIN;
                        *result = log(right_result);
                        break;
                    case OP_EXP:
                        *result = exp(right_result);
                        break;
                    default:
                        return TREE_ERROR_UNKNOWN_OPERATION;
                }
                return TREE_ERROR_NO;
            }

        case NODE_VAR:
        {
            if (node->data.var_definition.name == NULL)
                return TREE_ERROR_VARIABLE_NOT_FOUND;

            char* var_name = node->data.var_definition.name;
            double value = 0.0;
            TreeErrorType error = GetVariableValue(var_table, var_name, &value);

            if (error == TREE_ERROR_VARIABLE_NOT_FOUND)
            {
                error = AddVariable(var_table, var_name);
                if (error != TREE_ERROR_NO)
                    return error;

                error = RequestVariableValue(var_table, var_name);
                if (error != TREE_ERROR_NO) return error;

                error = GetVariableValue(var_table, var_name, &value);
                if (error != TREE_ERROR_NO) return error;
            }
            else if (error == TREE_ERROR_VARIABLE_UNDEFINED)
            {
                error = RequestVariableValue(var_table, var_name);
                if (error != TREE_ERROR_NO) return error;

                error = GetVariableValue(var_table, var_name, &value);
                if (error != TREE_ERROR_NO) return error;
            }
            else if (error != TREE_ERROR_NO)
            {
                return error;
            }

            *result = value;
            return TREE_ERROR_NO;
        }

        default:
            return TREE_ERROR_UNKNOWN_OPERATION;
    }
}

TreeErrorType EvaluateTree(Tree* tree, VariableTable* var_table, double* result)
{
    if (tree == NULL || var_table == NULL || result == NULL)
        return TREE_ERROR_NULL_PTR;

    if (tree->root == NULL)
        return TREE_ERROR_NULL_PTR;

    return EvaluateTreeRecursive(tree->root, var_table, result, 0);
}

TreeErrorType ExecuteAssignment(Node* assignment, VariableTable* var_table, double* result)
{
    if (!assignment || assignment->type != NODE_ASSIGN || !var_table || !result)
        return TREE_ERROR_NULL_PTR;

    if (assignment->left->type != NODE_VAR)
        return TREE_ERROR_INVALID_NODE;

    Tree expr_tree = {};
    TreeCtor(&expr_tree);
    expr_tree.root = assignment->right;

    TreeErrorType error = EvaluateTree(&expr_tree, var_table, result);

    if (error == TREE_ERROR_NO)
    {
        char* var_name = assignment->left->data.var_definition.name;
        SetVariableValue(var_table, var_name, *result);
    }

    expr_tree.root = NULL;
    TreeDtor(&expr_tree);

    return error;
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

                case OP_LN:
                case OP_EXP:
                    node->priority = 3;
                    break;

                case OP_POW:
                    node->priority = 4;
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

static Node* CreateVariableNode(const char* name)
{
    if (!name)
        return NULL;

    ValueOfTreeElement data = {};
    data.var_definition.name = strdup(name);
    if (!data.var_definition.name)
        return NULL;

    data.var_definition.hash = ComputeHash(name);
    return CreateNode(NODE_VAR, data, NULL, NULL);
}

size_t CountTreeNodes(Node* node)
{
    if (node == NULL)
        return 0;

    return 1 + CountTreeNodes(node->left) + CountTreeNodes(node->right);
}

#include "DSL_undef.h"
