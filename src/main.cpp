#include <stdio.h>
#include "new_great_input.h"
#include "operations.h"
//выполняет любую прогу //FIXME как будто хуятина какая-то

int main(int argc, const char** argv)
{
    VariableTable var_table = {};
    InitVariableTable(&var_table);

    Node* program = ReadProgramFromFile("program.txt", &var_table);
    if (!program)
    {
        printf("Ошибка: не удалось загрузить программу\n");
        DestroyVariableTable(&var_table);

        return 1;
    }

    printf("Программа загружена успешно.\n\n");

    double result = 0.0;
    TreeErrorType error = ExecuteProgram(program, &var_table, &result);

    if (error == TREE_ERROR_NO)
    {
        printf("Программа выполнена успешно\n");
        printf("Результат: %.2f\n", result);

        printf("\n Переменные после выполнения:\n");
        for (int i = 0; i < var_table.number_of_variables; i++)
        {
            printf("  %s = %.2f\n", var_table.variables[i].name, var_table.variables[i].value);
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
