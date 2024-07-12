#include "ast.h"

ast_t* ast_create(ast_type_t type, position_t* position, void* value) {
    ast_t* ast = (ast_t*) Ellie_malloc(sizeof(ast_t));
    assert_allocation(ast);
    ast->type = type;
    ast->position = position;
    ast->value = value;
    return ast;
}

void ast_free(ast_t* self) {
    Ellie_free(self->position);
    Ellie_free(self->value);
    Ellie_free(self);
}

char* ast_to_string(ast_t* self) {
    return str__format("AST(%s)", AST_XNAME(self->type));
}

// 
ast_terminal_t* ast_terminal_create(char* value) {
    ast_terminal_t* terminal = (ast_terminal_t*) Ellie_malloc(sizeof(ast_terminal_t));
    assert_allocation(terminal);
    terminal->value = value;
    return terminal;
}

ast_object_type_t* ast_object_type_create(ast_t* key, ast_t* val) {
    ast_object_type_t* record = (ast_object_type_t*) Ellie_malloc(sizeof(ast_object_type_t));
    assert_allocation(record);
    record->key = key;
    record->val = val;
    return record;
}

ast_array_type_t* ast_array_type_create(ast_t* type) {
    ast_array_type_t* array = (ast_array_type_t*) Ellie_malloc(sizeof(ast_array_type_t));
    assert_allocation(array);
    array->type = type;
    return array;
}

ast_nullable_type_t* ast_nullable_type_create(ast_t* type) {
    ast_nullable_type_t* nullable = (ast_nullable_type_t*) Ellie_malloc(sizeof(ast_nullable_type_t));
    assert_allocation(nullable);
    nullable->type = type;
    return nullable;
}

ast_object_t* ast_object_create(ast_t* type, ast_t** keys, ast_t** values) {
    ast_object_t* object = (ast_object_t*) Ellie_malloc(sizeof(ast_object_t));
    assert_allocation(object);
    object->type = type;
    object->keys = keys;
    object->values = values;
    return object;
}

ast_array_t* ast_array_create(ast_t* type, ast_t** elements) {
    ast_array_t* array = (ast_array_t*) Ellie_malloc(sizeof(ast_array_t));
    assert_allocation(array);
    array->type = type;
    array->elements = elements;
    return array;
}

ast_access_t* ast_access_create(ast_t* object, ast_t* member) {
    ast_access_t* access = (ast_access_t*) Ellie_malloc(sizeof(ast_access_t));
    assert_allocation(access);
    access->object = object;
    access->member = member;
    return access;
}

ast_call_t* ast_call_create(ast_t* callable, ast_t** arguments) {
    ast_call_t* call = (ast_call_t*) Ellie_malloc(sizeof(ast_call_t));
    assert_allocation(call);
    call->callable = callable;
    call->arguments = arguments;
    return call;
}

ast_postfix_t* ast_postfix_create(char* operator, ast_t* operand) {
    ast_postfix_t* ast_postfix = (ast_postfix_t*) Ellie_malloc(sizeof(ast_postfix_t));
    assert_allocation(ast_postfix);
    ast_postfix->operator = operator;
    ast_postfix->operand = operand;
    return ast_postfix;
}

ast_unary_t* ast_unary_create(char* operator, ast_t* operand) {
    ast_unary_t* ast_unary = (ast_unary_t*) Ellie_malloc(sizeof(ast_unary_t));
    assert_allocation(ast_unary);
    ast_unary->operator = operator;
    ast_unary->operand = operand;
    return ast_unary;
}

ast_binary_t* ast_binary_create(char* operator, ast_t* left, ast_t* right) {
    ast_binary_t* ast_binary = (ast_binary_t*) Ellie_malloc(sizeof(ast_binary_t));
    assert_allocation(ast_binary);
    ast_binary->operator = operator;
    ast_binary->left = left;
    ast_binary->right = right;
    return ast_binary;
}

ast_ternary_t* ast_ternary_create(ast_t* condition, ast_t* left, ast_t* right) {
    ast_ternary_t* ast_ternary = (ast_ternary_t*) Ellie_malloc(sizeof(ast_ternary_t));
    assert_allocation(ast_ternary);
    ast_ternary->condition = condition;
    ast_ternary->left = left;
    ast_ternary->right = right;
    return ast_ternary;
}

// 

ast_variable_declaration_t* ast_variable_declaration_create(ast_t** variable_name, ast_t** variable_type, ast_t** variable_value) {
    ast_variable_declaration_t* ast_variable_declaration = (ast_variable_declaration_t*) Ellie_malloc(sizeof(ast_variable_declaration_t));
    assert_allocation(ast_variable_declaration);
    ast_variable_declaration->variable_name = variable_name;
    ast_variable_declaration->variable_type = variable_type;
    ast_variable_declaration->variable_value = variable_value;
    return ast_variable_declaration;
}

ast_if_t* ast_if_create(ast_t* condition, ast_t* if_body, ast_t* else_body) {
    ast_if_t* ast_if = (ast_if_t*) Ellie_malloc(sizeof(ast_if_t));
    assert_allocation(ast_if);
    ast_if->condition = condition;
    ast_if->if_body = if_body;
    ast_if->else_body = else_body;
    return ast_if;
}

ast_function_t* ast_function_create(ast_t* function_name, ast_t** parameters_name, ast_t** parameters_type, ast_t* return_type, ast_t** body, bool is_export, bool is_async) {
    ast_function_t* ast_function = (ast_function_t*) Ellie_malloc(sizeof(ast_function_t));
    assert_allocation(ast_function);
    ast_function->function_name = function_name;
    ast_function->parameters_name = parameters_name;
    ast_function->parameters_type = parameters_type;
    ast_function->return_type = return_type;
    ast_function->body = body;
    ast_function->is_export = is_export;
    ast_function->is_async = is_async;
    return ast_function;
}

ast_block_t* ast_block_create(ast_t** children) {
    ast_block_t* ast_block = (ast_block_t*) Ellie_malloc(sizeof(ast_block_t));
    assert_allocation(ast_block);
    ast_block->children = children;
    return ast_block;
}

ast_return_t* ast_return_create(ast_t* value) {
    ast_return_t* ast_return = (ast_return_t*) Ellie_malloc(sizeof(ast_return_t));
    assert_allocation(ast_return);
    ast_return->return_value = value;
    return ast_return;
}

ast_expressiont_statement_t* ast_expressiont_statement_create(ast_t* expression) {
    ast_expressiont_statement_t* ast_expressiont_statement = (ast_expressiont_statement_t*) Ellie_malloc(sizeof(ast_expressiont_statement_t));
    assert_allocation(ast_expressiont_statement);
    ast_expressiont_statement->expression = expression;
    return ast_expressiont_statement;
}

ast_program_t* ast_program_create(ast_t** children) {
    ast_program_t* ast_program = (ast_program_t*) Ellie_malloc(sizeof(ast_program_t));
    assert_allocation(ast_program);
    ast_program->children = children;
    return ast_program;
}