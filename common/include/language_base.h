#ifndef LANGUAGE_BASE_H_
#define LANGUAGE_BASE_H_

#include <stdbool.h>
#include "tree_common.h"

void FreeSubtree(Node* node);
Node* CreateNode(NodeType type, ValueOfTreeElement data, Node* left, Node* right);
size_t CountTreeNodes(Node* node);
unsigned int ComputeHash(const char* str);


#endif // LOGIC_FUNCTIONS_H_
