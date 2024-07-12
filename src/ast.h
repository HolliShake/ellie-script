#include "global.h"
#include "position.h"

#ifndef AST_H
#define AST_H
    
    typedef enum ast_type {
        AST_ID,
        AST_NUMBER_FROM_INT, // INT,
        AST_NUMBER_FROM_FLOAT, // FLOAT,
        AST_BIGINT,
        AST_STRING,
        AST_BOOL,
        AST_NULL,
        AST_OBJECT,
        AST_ARRAY,
        // 
        AST_NUMBER_DATA_TYPE,
        AST_BIGINT_DATA_TYPE,
        AST_STRING_DATA_TYPE,
        AST_BOOL_DATA_TYPE,
        AST_VOID_DATA_TYPE,
        AST_OBJECT_DATA_TYPE,
        AST_ARRAY_DATA_TYPE,
        AST_NULLABLE_DATA_TYPE,
        // 
        AST_MEMBER_ACCESS,
        AST_NULLABLE_ACCESS,
        AST_INDEX_ACCESS,
        AST_CALL,
        // 
        AST_POSTFIX_INC,
        AST_POSTFIX_DEC,
        // 
        AST_UNARY_NOT,
        AST_UNARY_BNOT,
        AST_UNARY_POS,
        AST_UNARY_NEG,
        AST_UNARY_INC,
        AST_UNARY_DEC,
        AST_UNARY_SPREAD,
        // 
        AST_BINARY_MUL,
        AST_BINARY_DIV,
        AST_BINARY_MOD,
        AST_BINARY_ADD,
        AST_BINARY_SUB,
        AST_BINARY_LSHIFT,
        AST_BINARY_RSHIFT,
        AST_BINARY_LT,
        AST_BINARY_GT,
        AST_BINARY_LTE,
        AST_BINARY_GTE,
        AST_BINARY_EQ,
        AST_BINARY_NEQ,
        AST_BINARY_AND,
        AST_BINARY_OR,
        AST_BINARY_XOR,
        AST_LOGICAL_AND,
        AST_LOGICAL_OR,
        AST_TERNARY,
        AST_ASSIGN,
        AST_ASSIGN_MUL,
        AST_ASSIGN_DIV,
        AST_ASSIGN_MOD,
        AST_ASSIGN_ADD,
        AST_ASSIGN_SUB,
        AST_ASSIGN_LSHIFT,
        AST_ASSIGN_RSHIFT,
        AST_ASSIGN_AND,
        AST_ASSIGN_OR,
        AST_ASSIGN_XOR,
        AST_BINARY_EXPR,
        // 
        AST_VARIABLE_DECLARATION,
        AST_CONST_DECLARATION,
        AST_LOCAL_DECLARATION,
        AST_IF_STATEMENT,
        AST_ASYNC_FUNCTION,
        AST_FUNCTION,
        AST_BLOCK,
        // 
        AST_RETURN,
        AST_EMPTY_STATEMENT,
        AST_EXPRESSION_STATEMENT,
        AST_PROGRAM
    } ast_type_t;

    #define AST_NAME(ast_type) #ast_type
    #define AST_XNAME(ast_type) \
        (ast_type == AST_ID) ? AST_NAME(AST_ID): \
        (ast_type == AST_NUMBER_FROM_INT  ) ? AST_NAME(AST_NUMBER_FROM_INT  ) : \
        (ast_type == AST_NUMBER_FROM_FLOAT) ? AST_NAME(AST_NUMBER_FROM_FLOAT) : \
        (ast_type == AST_BIGINT) ? AST_NAME(AST_BIGINT) : \
        (ast_type == AST_STRING) ? AST_NAME(AST_STRING) : \
        (ast_type == AST_BOOL)   ? AST_NAME(AST_BOOL)   : \
        (ast_type == AST_NULL)   ? AST_NAME(AST_NULL)   : \
        (ast_type == AST_OBJECT) ? AST_NAME(AST_OBJECT) : \
        (ast_type == AST_ARRAY)  ? AST_NAME(AST_ARRAY ) : \
        (ast_type == AST_NUMBER_DATA_TYPE) ? AST_NAME(AST_NUMBER_DATA_TYPE) : \
        (ast_type == AST_BIGINT_DATA_TYPE) ? AST_NAME(AST_BIGINT_DATA_TYPE) : \
        (ast_type == AST_STRING_DATA_TYPE) ? AST_NAME(AST_STRING_DATA_TYPE) : \
        (ast_type == AST_BOOL_DATA_TYPE)   ? AST_NAME(AST_BOOL_DATA_TYPE)   : \
        (ast_type == AST_VOID_DATA_TYPE)   ? AST_NAME(AST_VOID_DATA_TYPE)   : \
        (ast_type == AST_OBJECT_DATA_TYPE  ) ? AST_NAME(AST_OBJECT_DATA_TYPE  ) : \
        (ast_type == AST_ARRAY_DATA_TYPE   ) ? AST_NAME(AST_ARRAY_DATA_TYPE   ) : \
        (ast_type == AST_NULLABLE_DATA_TYPE) ? AST_NAME(AST_NULLABLE_DATA_TYPE) : \
        (ast_type == AST_MEMBER_ACCESS) ? AST_NAME(AST_MEMBER_ACCESS) : \
        (ast_type == AST_NULLABLE_ACCESS) ? AST_NAME(AST_NULLABLE_ACCESS) : \
        (ast_type == AST_INDEX_ACCESS) ? AST_NAME(AST_INDEX_ACCESS) : \
        (ast_type == AST_CALL) ? AST_NAME(AST_CALL) : \
        (ast_type == AST_POSTFIX_INC ) ? AST_NAME(AST_POSTFIX_INC) : \
        (ast_type == AST_POSTFIX_DEC ) ? AST_NAME(AST_POSTFIX_DEC) : \
        (ast_type == AST_UNARY_NOT   ) ? AST_NAME(AST_UNARY_NOT  ) : \
        (ast_type == AST_UNARY_BNOT  ) ? AST_NAME(AST_UNARY_BNOT ) : \
        (ast_type == AST_UNARY_POS   ) ? AST_NAME(AST_UNARY_POS  ) : \
        (ast_type == AST_UNARY_NEG   ) ? AST_NAME(AST_UNARY_NEG  ) : \
        (ast_type == AST_UNARY_INC   ) ? AST_NAME(AST_UNARY_INC  ) : \
        (ast_type == AST_UNARY_DEC   ) ? AST_NAME(AST_UNARY_DEC  ) : \
        (ast_type == AST_UNARY_SPREAD) ? AST_NAME(AST_UNARY_SPREAD) : \
        (ast_type == AST_BINARY_MUL) ? AST_NAME(AST_BINARY_MUL) : \
        (ast_type == AST_BINARY_DIV) ? AST_NAME(AST_BINARY_DIV) : \
        (ast_type == AST_BINARY_MOD) ? AST_NAME(AST_BINARY_MOD) : \
        (ast_type == AST_BINARY_ADD) ? AST_NAME(AST_BINARY_ADD) : \
        (ast_type == AST_BINARY_SUB) ? AST_NAME(AST_BINARY_SUB) : \
        (ast_type == AST_BINARY_LSHIFT) ? AST_NAME(AST_BINARY_LSHIFT) : \
        (ast_type == AST_BINARY_RSHIFT) ? AST_NAME(AST_BINARY_RSHIFT) : \
        (ast_type == AST_BINARY_LT  ) ? AST_NAME(AST_BINARY_LT  ) : \
        (ast_type == AST_BINARY_GT  ) ? AST_NAME(AST_BINARY_GT  ) : \
        (ast_type == AST_BINARY_LTE ) ? AST_NAME(AST_BINARY_LTE ) : \
        (ast_type == AST_BINARY_GTE ) ? AST_NAME(AST_BINARY_GTE ) : \
        (ast_type == AST_BINARY_EQ  ) ? AST_NAME(AST_BINARY_EQ  ) : \
        (ast_type == AST_BINARY_NEQ ) ? AST_NAME(AST_BINARY_NEQ ) : \
        (ast_type == AST_BINARY_AND ) ? AST_NAME(AST_BINARY_AND ) : \
        (ast_type == AST_BINARY_OR  ) ? AST_NAME(AST_BINARY_OR  ) : \
        (ast_type == AST_BINARY_XOR ) ? AST_NAME(AST_BINARY_XOR ) : \
        (ast_type == AST_LOGICAL_AND) ? AST_NAME(AST_LOGICAL_AND) : \
        (ast_type == AST_LOGICAL_OR ) ? AST_NAME(AST_LOGICAL_OR ) : \
        (ast_type == AST_TERNARY ) ? AST_NAME(AST_TERNARY) : \
        (ast_type == AST_ASSIGN  ) ? AST_NAME(AST_ASSIGN ) : \
        (ast_type == AST_ASSIGN_MUL ) ? AST_NAME(AST_ASSIGN_MUL) : \
        (ast_type == AST_ASSIGN_DIV ) ? AST_NAME(AST_ASSIGN_DIV) : \
        (ast_type == AST_ASSIGN_MOD ) ? AST_NAME(AST_ASSIGN_MOD) : \
        (ast_type == AST_ASSIGN_ADD ) ? AST_NAME(AST_ASSIGN_ADD) : \
        (ast_type == AST_ASSIGN_SUB ) ? AST_NAME(AST_ASSIGN_SUB) : \
        (ast_type == AST_ASSIGN_LSHIFT) ? AST_NAME(AST_ASSIGN_LSHIFT) : \
        (ast_type == AST_ASSIGN_RSHIFT) ? AST_NAME(AST_ASSIGN_RSHIFT) : \
        (ast_type == AST_ASSIGN_AND  ) ? AST_NAME(AST_ASSIGN_AND  ) : \
        (ast_type == AST_ASSIGN_OR   ) ? AST_NAME(AST_ASSIGN_OR   ) : \
        (ast_type == AST_ASSIGN_XOR  ) ? AST_NAME(AST_ASSIGN_XOR  ) : \
        (ast_type == AST_BINARY_EXPR ) ? AST_NAME(AST_BINARY_EXPR ) : \
        (ast_type == AST_VARIABLE_DECLARATION) ? AST_NAME(AST_VARIABLE_DECLARATION) : \
        (ast_type == AST_CONST_DECLARATION) ? AST_NAME(AST_CONST_DECLARATION) : \
        (ast_type == AST_LOCAL_DECLARATION) ? AST_NAME(AST_LOCAL_DECLARATION) : \
        (ast_type == AST_IF_STATEMENT) ? AST_NAME(AST_IF_STATEMENT) : \
        (ast_type == AST_ASYNC_FUNCTION) ? AST_NAME(AST_ASYNC_FUNCTION) : \
        (ast_type == AST_FUNCTION   ) ? AST_NAME(AST_FUNCTION)    : \
        (ast_type == AST_BLOCK      ) ? AST_NAME(AST_BLOCK)       : \
        (ast_type == AST_RETURN) ? AST_NAME(AST_RETURN) : \
        (ast_type == AST_EMPTY_STATEMENT) ? AST_NAME(AST_EMPTY_STATEMENT) : \
        (ast_type == AST_EXPRESSION_STATEMENT) ? AST_NAME(AST_EXPRESSION_STATEMENT) : \
        (ast_type == AST_PROGRAM) ? AST_NAME(AST_PROGRAM) : "UNKNOWN_AST_TYPE"\

    typedef struct ast_struct {
        ast_type_t type;
        position_t* position;
        void* value;
    } ast_t;

    typedef struct ast_terminal_struct {
        char* value;
    } ast_terminal_t;

    typedef struct ast_object_type_struct {
        ast_t* key; // key type
        ast_t* val; // value type
    } ast_object_type_t;

    typedef struct ast_array_type_struct {
        ast_t* type; // element type
    } ast_array_type_t;

    typedef struct ast_nullable_type_struct {
        ast_t* type; // element type
    } ast_nullable_type_t;

    typedef struct ast_object_struct {
        ast_t* type;    // object type
        ast_t** keys;   // array of ast_t*
        ast_t** values; // array of ast_t*
    } ast_object_t;

    typedef struct ast_array_struct {
        ast_t* type; // element type
        ast_t** elements; // array of ast_t*
    } ast_array_t;

    typedef struct ast_access_struct {
        ast_t* object;
        ast_t* member;
    } ast_access_t;

    typedef struct ast_call_struct {
        ast_t* callable;
        ast_t** arguments;
    } ast_call_t;

    typedef struct ast_postfix_or_unary_struct {
        char* operator;
        ast_t* operand;
    } ast_postfix_t, ast_unary_t;

    typedef struct ast_binary_struct {
        char* operator;
        ast_t* left;
        ast_t* right;
    } ast_binary_t;

    typedef struct ast_ternary_struct {
        ast_t* condition;
        ast_t* left;
        ast_t* right;
    } ast_ternary_t;

    // 
    typedef struct ast_variable_declaration_struct {
        ast_t** variable_name;
        ast_t** variable_type;
        ast_t** variable_value;
    } ast_variable_declaration_t;

    typedef struct ast_if_struct {
        ast_t* condition;
        ast_t* if_body;
        ast_t* else_body;
    } ast_if_t;

    typedef struct ast_function_struct {
        ast_t* function_name;
        ast_t** parameters_name; // array of ast_t* -> ast_terminal_t
        ast_t** parameters_type; // array of ast_t*
        ast_t* return_type;      // ast_t*
        ast_t** body;            // array of ast_t*
        bool is_export;
        bool is_async;
    } ast_function_t;

    typedef struct ast_block_struct {
        ast_t** children;
    } ast_block_t;

    typedef struct ast_return_struct {
        ast_t* return_value;
    } ast_return_t;

    typedef struct ast_expressiont_statement_struct {
        ast_t* expression;
    } ast_expressiont_statement_t;

    typedef struct ast_program_struct {
        ast_t** children;
    } ast_program_t;
    
    ast_t* ast_create(ast_type_t type, position_t* position, void* value);
    void ast_free(ast_t* self);
    char* ast_to_string(ast_t* self);
    //
    ast_terminal_t* ast_terminal_create(char* value);
    ast_object_type_t* ast_object_type_create(ast_t* key, ast_t* val);
    ast_array_type_t* ast_array_type_create(ast_t* type);
    ast_nullable_type_t* ast_nullable_type_create(ast_t* type);
    ast_object_t* ast_object_create(ast_t* type, ast_t** keys, ast_t** values);
    ast_array_t* ast_array_create(ast_t* type, ast_t** elements);
    ast_access_t* ast_access_create(ast_t* object, ast_t* member);
    ast_call_t* ast_call_create(ast_t* callable, ast_t** arguments);
    ast_postfix_t* ast_postfix_create(char* operator, ast_t* operand);
    ast_unary_t* ast_unary_create(char* operator, ast_t* operand);
    ast_binary_t* ast_binary_create(char* operator, ast_t* left, ast_t* right);
    ast_ternary_t* ast_ternary_create(ast_t* condition, ast_t* left, ast_t* right);
    // 
    ast_variable_declaration_t* ast_variable_declaration_create(ast_t** variable_name, ast_t** variable_type, ast_t** variable_value);
    ast_if_t* ast_if_create(ast_t* condition, ast_t* if_body, ast_t* else_body);
    ast_function_t* ast_function_create(ast_t* function_name, ast_t** parameters_name, ast_t** parameters_type, ast_t* return_type, ast_t** body, bool is_export, bool is_async);
    ast_block_t* ast_block_create(ast_t** children);
    ast_return_t* ast_return_create(ast_t* return_value);
    ast_expressiont_statement_t* ast_expressiont_statement_create(ast_t* expression);
    ast_program_t* ast_program_create(ast_t** children);
#endif