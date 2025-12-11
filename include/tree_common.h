#ifndef TREE_COMMON_H_
#define TREE_COMMON_H_

#include <stdlib.h>

const int         kMaxSystemCommandLength             = 512;
const int         kMaxLengthOfFilename                = 256;
const char* const kGeneralFolderNameForLogs           = "tree_logs";
const int         kMaxLengthOfAnswer                  = 256;
const int         kMaxInputCapacity                   = 256;
const int         kMaxPathDepth                       = 100;
const int         kTreeDumpAfterAddingElementCapacity = 512;
const char* const kDefaultDataBaseFilename            = "differenciator_tree.txt";
const int         kMaxNumberOfDerivative              = 4;
const int         kMaxLengthOfTexExpression           = 30000;
const char* const kTexFilename                        = "full_analysis.tex";
const int         kMaxDotBufferLength                 = 64;
const int         kMaxTexDescriptionLength            = 256;
const int         kMaxNOfVariables                    = 100;
const int         kMaxVariableLength                  = 32;
const int         kMaxFuncNameLength                  = 256;
const int         kMaxCustomNotationLength            = 32;
const int         kTaylor                             = 7;

typedef enum {
    NODE_OP,
    NODE_VAR,
    NODE_NUM,
    NODE_IF,
    NODE_WHILE,
    NODE_SEQUENCE,
    NODE_ASSIGN,
    NODE_EMPTY,

    NODE_EQUAL,
    NODE_NOT_EQUAL,
    NODE_LESS,
    NODE_LESS_EQUAL,
    NODE_GREATER,
    NODE_GREATER_EQUAL,

    NODE_AND,
    NODE_OR,
    NODE_NOT,

    NODE_FUNC_DECL,
    NODE_FUNC_CALL,
    NODE_RETURN,
    NODE_PARAM,
    NODE_ARGS
} NodeType;

typedef enum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_COUNT
} OperationType;

typedef struct Node Node;

typedef struct {
    unsigned int hash;
    char* name;
} VariableDefinition;

typedef struct {
    unsigned int hash;
    const char* name;
    OperationType op_value;
} OperationInfo;

typedef struct {
    char* name;
    Node* args;
} FuncCallData;

typedef union {
    double             num_value;
    OperationType      op_value;
    VariableDefinition var_definition;
    char*              func_name;
    FuncCallData       func_call;
} ValueOfTreeElement;

typedef struct Node {
    ValueOfTreeElement  data;
    NodeType            type;
    struct Node*        left;
    struct Node*        right;
    struct Node*        parent;
    int                 priority;  // Приоритет операции (0 для чисел и переменных)
} Node;

typedef struct {
    Node* root;
    size_t size;
    char* file_buffer;
} Tree;

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
    struct FunctionInfo* next;
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
