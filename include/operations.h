#ifndef DIFF_OPERATIONS
#define DIFF_OPERATIONS

#include <stdio.h>
#include "tree_error_types.h"
#include "tree_common.h"
#include "variable_parse.h"


void  FreeSubtree(Node* node);
size_t CountTreeNodes(Node* node);
TreeErrorType EvaluateTree(Tree* tree, VariableTable* var_table, double* result);

TreeErrorType ExecuteProgram(Node* program, VariableTable* var_table, double* result);
TreeErrorType ExecuteAssignment(Node* assignment, VariableTable* var_table, double* result);
TreeErrorType ExecuteIf(Node* node, VariableTable* var_table, double* result);

Node* CreateNode(NodeType type, ValueOfTreeElement data, Node* left, Node* right);


#endif // DIFF_OPERATIONS
