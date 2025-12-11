#ifndef DIFF_OPERATIONS
#define DIFF_OPERATIONS

#include <stdio.h>
#include "tree_error_types.h"
#include "tree_common.h"
#include "variable_parse.h"

void  FreeSubtree(Node* node);
Node* CreateNode(NodeType type, ValueOfTreeElement data, Node* left, Node* right);
size_t CountTreeNodes(Node* node);


#endif // DIFF_OPERATIONS
