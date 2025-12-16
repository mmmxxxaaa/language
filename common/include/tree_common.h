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
const char* const kTexFilename                        = "full_analysis.tex"; //FIXME убрать нахуй
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
    Node* root;
    size_t size;
    char* file_buffer;
} Tree;

typedef struct {
    unsigned int hash;
    char* name;
} VariableDefinition;

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

#define DEBUG

#ifdef DEBUG
    #define DEBUG_PRINT(format, ...) \
        do { \
            fprintf(stderr, "[DEBUG %s:%d] ", __FILE__, __LINE__); \
            fprintf(stderr, format, ##__VA_ARGS__); \
        } while(0)
#else
    #define DEBUG_PRINT(format, ...) ((void)0)
#endif

#endif //TREE_COMMON_H_
