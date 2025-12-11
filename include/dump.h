#ifndef DUMP_H_
#define DUMP_H_

#include <stdio.h>
#include <time.h>
#include "tree_common.h"
#include "tree_error_types.h"

TreeErrorType GenerateDotFile(Tree* tree, const char* filename);
void CreateDotNodes(Tree* tree, FILE* dot_file);
void CreateTreeConnections(Node* node, FILE* dot_file);
const char* GetNodeColor(Node* node, Tree* tree);


TreeErrorType WriteTreeCommonPicture(Tree* tree, FILE* htm_file,
                                     const char* folder_path,
                                     const char* folder_name);
void WriteDumpHeader(FILE* htm_file, time_t now, const char* comment);
void WriteDumpFooter(FILE* htm_file);
TreeErrorType TreeDumpToHtm(Tree* tree, FILE* htm_file,
                            const char* folder_path,
                            const char* folder_name,
                            const char* comment);

TreeErrorType TreeDump(Tree* tree, const char* filename);
TreeErrorType InitTreeLog(const char* filename);
TreeErrorType CloseTreeLog(const char* filename);

TreeVerifyResult VerifyTree(Tree* tree);
const char* TreeVerifyResultToString(TreeVerifyResult result);

#endif // DUMP_H_
