#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "function_parse.h"
#include "tree_error_types.h"

FunctionTable* InitFunctionTable(FunctionTable* table)
{
    assert(table);

    table->functions = NULL;
    table->count = 0;

    return table;
}

FunctionInfo* FindFunction(FunctionTable* func_table, const char* func_name)
{
    if (!func_table || !func_name) return NULL;

    FunctionInfo* current = func_table->functions;
    while (current)
    {
        if (strcmp(current->name, func_name) == 0)
            return current;
        current = current->next;
    }

    return NULL;
}

TreeErrorType AddFunction(FunctionTable* table, const char* name, int param_count, char** param_names, Node* body)
{
    if (!table || !name)
        return TREE_ERROR_INVALID_ARGUMENT;

    if (FindFunction(table, name))
        return TREE_ERROR_FUNCTION_REDEFINITION;

    FunctionInfo* func_info = (FunctionInfo*)calloc(1, sizeof(FunctionInfo));
    if (!func_info)
        return TREE_ERROR_MEMORY;

    func_info->name = strdup(name);
    if (!func_info->name)
    {
        free(func_info);
        return TREE_ERROR_MEMORY;
    }

    func_info->param_count = param_count;
    func_info->body = body;

    if (param_count > 0 && param_names)
    {
        func_info->param_names = (char**)calloc(param_count, sizeof(char*));
        if (!func_info->param_names)
        {
            free(func_info->name);
            free(func_info);
            return TREE_ERROR_MEMORY;
        }

        for (int i = 0; i < param_count; i++)
        {
            func_info->param_names[i] = strdup(param_names[i]);
            if (!func_info->param_names[i])
            {
                for (int j = 0; j < i; j++)
                {
                    free(func_info->param_names[j]);
                }
                free(func_info->param_names);
                free(func_info->name);
                free(func_info);
                return TREE_ERROR_MEMORY;
            }
        }
    }

    func_info->next = table->functions;
    table->functions = func_info;
    table->count++;

    return TREE_ERROR_NO;
}

void DestroyFunctionTable(FunctionTable* table)
{
    if (!table)
        return;

    FunctionInfo* current = table->functions;
    while (current)
    {
        FunctionInfo* next = current->next;

        free(current->name);

        if (current->param_names)
        {
            for (int i = 0; i < current->param_count; i++)
                free(current->param_names[i]);

            free(current->param_names);
        }

        free(current);
        current = next;
    }

    table->functions = NULL;
    table->count = 0;
}
