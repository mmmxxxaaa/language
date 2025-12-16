#ifndef OUTPUT_INFIX_H_
#define OUTPUT_INFIX_H_

#include "tree_common_frontend.h"
#include "tree_common.h"
#include <stdio.h>

typedef struct {
    NodeType node_type;
    const char* ast_str;
} ASTMapping;

void PrintASTToFile(Node* node, FILE* file);

#endif // OUTPUT_INFIX_H_
