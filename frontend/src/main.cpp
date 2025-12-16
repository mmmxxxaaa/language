#include <stdio.h>
#include <string.h>
#include "new_great_input.h"
#include "language_base.h"
#include "dump.h"
#include "function_parse.h"
#include "output_infix.h"

int main(int argc, const char** argv)
{
    ParserContext context = {};
    InitVariableTable(&context.var_table);
    InitFunctionTable(&context.func_table);

    const char* filename = "program.txt"; //FIXME

    Node* program = ReadProgramFromFile(filename, &context);
    if (!program)
    {
        fprintf(stderr, "Ошибка: не удалось загрузить программу из файла '%s'\n", filename);
        DestroyVariableTable(&context.var_table);
        return 1;
    }

    Tree tree = {};
    tree.root = program;
    tree.size = CountTreeNodes(program);

    TreeErrorType error = InitTreeLog("program_dump"); //FIXME
    if (error != TREE_ERROR_NO)
    {
        fprintf(stderr, "Ошибка инициализации лога: %d\n", error);
    }
    else
    {
        error = TreeDump(&tree, "program_dump"); //FIXME
        if (error != TREE_ERROR_NO)
            fprintf(stderr, "Ошибка создания дампа: %d\n", error);

        error = CloseTreeLog("program_dump");
        if (error != TREE_ERROR_NO)
            fprintf(stderr, "Ошибка закрытия лога: %d\n", error);
    }

    TreeVerifyResult verify_result = VerifyTree(&tree);
    if (verify_result != TREE_VERIFY_SUCCESS)
        printf("Проверка дерева: %s\n", TreeVerifyResultToString(verify_result));

    printf("Найдено функций: %d\n", context.func_table.count);
    printf("\n Переменные в таблице: %d\n", context.var_table.number_of_variables);

    if (program)
    {
        printf("Дерево построено успешно\n");

        FILE* console = stdout;
        PrintASTToFile(program, console);

        FILE* file = fopen("output.ast", "w");
        if (file)
        {
            PrintASTToFile(program, file);
            fclose(file);
        }
    }

    FreeSubtree(program);
    DestroyVariableTable(&context.var_table);
    DestroyFunctionTable(&context.func_table);

    return 0;
}
