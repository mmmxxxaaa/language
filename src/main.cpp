#include <stdio.h>
#include "new_great_input.h"
#include "operations.h"

int main(int argc, const char** argv)
{
    VariableTable var_table = {};
    InitVariableTable(&var_table);

    Node* program = ReadProgramFromFile("program.txt", &var_table);
    if (!program)
    {
        printf("Ошибка: не удалось загрузить программу\n");
        return 1;
    }

    printf("Программа загружена успешно.\n\n");

    double result = 0.0;
    TreeErrorType error = ExecuteAssignment(program, &var_table, &result);

    if (error == TREE_ERROR_NO)
    {
        printf("Программа выполнена успешно\n");

        if (program->type == NODE_ASSIGN && program->left->type == NODE_VAR)
        {
            char* var_name = program->left->data.var_definition.name;
            printf("  %s = %.2f\n", var_name, result);
        }
    }
    else
    {
        printf("Ошибка выполнения: %d\n", error);
    }

    FreeSubtree(program);
    DestroyVariableTable(&var_table);

    return 0;
}
