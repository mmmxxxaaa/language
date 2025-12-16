#ifndef FUNCTION_PARSE_H_
#define FUNCTION_PARSE_H_

#include "tree_common_frontend.h"
#include "tree_common.h"
#include "tree_error_types.h"

FunctionTable* InitFunctionTable(FunctionTable* table);
void DestroyFunctionTable(FunctionTable* table);
FunctionInfo* FindFunction(FunctionTable* func_table, const char* func_name);
TreeErrorType AddFunction(FunctionTable* table, const char* name, int param_count, char** param_names, Node* body);


#endif // FUNCTION_PARSE_H_
