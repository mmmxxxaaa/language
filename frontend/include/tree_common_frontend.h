#ifndef TREE_COMMON_FRONTEND_H_
#define TREE_COMMON_FRONTEND_H_

#include <stdlib.h>
#include "tree_common.h"

//=============== ТАБЛИЦЫ =============================
typedef struct {
    char   name[kMaxVariableLength];
    double value;
    size_t hash;
    bool   is_defined;
} Variable;

typedef struct {
    Variable* variables;
    int       number_of_variables;
    bool      is_sorted;
} VariableTable;

typedef struct FunctionInfo {
    char* name;
    int param_count;
    char** param_names;
    Node* body;
    FunctionInfo* next;
} FunctionInfo;

typedef struct FunctionTable {
    FunctionInfo* functions;
    int count;
} FunctionTable;

typedef struct ParserContext {
    FunctionTable func_table;
    VariableTable var_table;
} ParserContext;


#endif //TREE_COMMON_H_
