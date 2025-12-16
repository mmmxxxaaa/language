#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "function_parse.h"
#include "tree_error_types.h"
#include "tree_base.h"

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
    DEBUG_PRINT("%p\n", (void*)current);
    while (current)
    {
        DEBUG_PRINT("---\n%s---\n", current->name);
        if (strcmp(current->name, func_name) == 0)
            return current;
        current = current->next;
    }

    return NULL;
}

TreeErrorType AddFunction(FunctionTable* table, const char* name, int param_count, char** param_names, Node* body)
{
    if (!table || !name)
        return TREE_ERROR_NULL_PTR;

    FunctionInfo* current = table->functions;
    while (current)
    {
        if (strcmp(current->name, name) == 0)
        {
            if (current->body == NULL && body != NULL)
            {
                current->body = body; //сохр указатель на существующее дерево

                if (current->param_count != param_count)
                {
                    fprintf(stderr, "Error: Parameter count mismatch for function '%s'\n", name);
                    return TREE_ERROR_FUNCTION_REDEFINITION;
                }

                return TREE_ERROR_NO;
            }
            else if (current->body != NULL)
            {
                return TREE_ERROR_FUNCTION_REDEFINITION;
            }
            else
            {
                return TREE_ERROR_NO;
            }
        }
        current = current->next;
    }

    FunctionInfo* new_func = (FunctionInfo*)calloc(1, sizeof(FunctionInfo));
    if (!new_func)
        return TREE_ERROR_ALLOCATION;

    new_func->name = strdup(name);
    if (!new_func->name)
    {
        free(new_func);
        return TREE_ERROR_ALLOCATION;
    }

    new_func->param_count = param_count;
    new_func->body = body;
    new_func->next = NULL;

    if (param_count > 0 && param_names) // копируем имена параметров
    {
        new_func->param_names = (char**)calloc(param_count, sizeof(char*));
        if (!new_func->param_names)
        {
            free(new_func->name);
            free(new_func);
            return TREE_ERROR_ALLOCATION;
        }

        for (int i = 0; i < param_count; i++)
        {
            if (param_names[i])
            {
                new_func->param_names[i] = strdup(param_names[i]);
                if (!new_func->param_names[i])
                {
                    for (int j = 0; j < i; j++)
                        free(new_func->param_names[j]);
                    free(new_func->param_names);
                    free(new_func->name);
                    free(new_func);
                    return TREE_ERROR_ALLOCATION;
                }
            }
        }
    }
    else
    {
        new_func->param_names = NULL;
    }

    new_func->next = table->functions;
    table->functions = new_func;
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
