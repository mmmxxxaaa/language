#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "reading_and_building.h"
#include "language_base.h"
#include "tree_common.h"
#include "backend_dump.h"

static long GetFileSize(FILE* file)
{
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    return size;
}

static char* ReadFileToString(const char* filename)
{
    FILE* file = fopen(filename, "r");
    if (!file) return NULL;

    long size = GetFileSize(file);
    if (size <= 0)
    {
        fclose(file);
        return NULL;
    }

    char* buffer = (char*)calloc(size + 1, sizeof(char));
    if (!buffer)
    {
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, size, file);
    fclose(file);

    return buffer;
}

int main(int argc, char** argv)
{
    const char* filename = (argc > 1) ? argv[1] : "output.ast";

    char* program = ReadFileToString(filename);
    if (!program)
    {
        printf("Failed to read file: %s\n", filename);
        return 1;
    }

    Tree* tree = BuildTreeFromExpression(program);
    free(program);

    if (!tree)
    {
        printf("Failed to build tree\n");
        return 1;
    }

    printf("Tree built: %zu nodes\n", tree->size);

    BackendTreeDump(tree, "tree_dump.txt");

    FreeSubtree(tree->root);
    free(tree);

    return 0;
}
