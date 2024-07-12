#include "generator.h"


generator_t* generator_create(char* file_path_or_name, char* source) {
    generator_t* generator = (generator_t*) Ellie_malloc(sizeof(generator_t));
    assert_allocation(generator);
    generator->parser = parser_create(file_path_or_name, source);
    generator->js_code = str__new("");
    generator->nesting_level = 0;
    return generator;
}

#define EMIT(buffer) {\
    char* old;\
    self->js_code = str__concat(old = self->js_code, buffer);\
    Ellie_free(old);\
}\

#define EMIT_CHILD(buffer) {\
    char* old;\
    char* tabs = str__new("");\
    for (size_t i = 0; i < self->nesting_level; i++) {\
        tabs = str__concat(old = tabs, "\t");\
        Ellie_free(old);\
    }\
    self->js_code = str__concat(old = self->js_code, tabs);\
    self->js_code = str__concat(old = self->js_code, buffer);\
    Ellie_free(old);\
}\

#define EMIT_INDENT() {\
    self->nesting_level++;\
}\

#define EMIT_DEDENT() {\
    self->nesting_level--;\
}\

#define EMIT_NEWLINE() {\
    char* old;\
    self->js_code = str__concat(old = self->js_code, "\n");\
    Ellie_free(old);\
}\

#define DUMP_JSCODE() {\
    printf("JSCODE:\n\n%s\n", self->js_code);\
}\

/****************************************/

#define ASSERT_BOOLEAN(typetag) {\
    if (!typetag_is_bool(typetag)) {\
        throw_errorf(ast->position, "expected type %s, got %s type", typetag_get_name(TYPETAG_BOOL), typetag_get_name(typetag));\
    }\
}\

#define ASSERT_TYPE_BINARY_EXPRESSION(condition, lhs, rhs, opt) {\
    if (!condition) {\
        throw_errorf(ast->position, "invalid operand types %s and %s for operator \"%s\"", typetag_get_name(lhs), typetag_get_name(rhs), opt);\
    }\
}\

#define ASSERT_TYPE_MATCH(condition, lhs, rhs, opt) {\
    if (!condition) {\
        throw_errorf(ast->position, "type mismatch %s and %s for operator \"%s\"", typetag_get_name(lhs), typetag_get_name(rhs), opt);\
    }\
}\

/****************************************/

#define ASSERT_AST_IDENTIFIER(ast)\
    if (ast->type != AST_ID) {\
        throw_errore(ast->position, "expected an identifier");\
    }\

#define ASSERT_AST_NOT_EMPTY_STATEMENT(ast)\
    if (ast->type == AST_EMPTY_STATEMENT) {\
        throw_errore(ast->position, "expected a statement");\
    }\

/****************************************/

static
typetag_t* generator_datatype(generator_t* self, context_t* context, ast_t* ast) {
    switch (ast->type) {
        case AST_NUMBER_DATA_TYPE: {
            return TYPETAG_NUMBER;
        }
        case AST_BIGINT_DATA_TYPE: {
            return TYPETAG_INT;
        }
        case AST_STRING_DATA_TYPE: {
            return TYPETAG_STRING;
        }
        case AST_BOOL_DATA_TYPE: {
            return TYPETAG_BOOL;
        }
        case AST_VOID_DATA_TYPE: {
            return TYPETAG_VOID;
        }
        default:
            DUMP_JSCODE();
            throw_errorf(ast->position, "unknown AST type: %s", AST_XNAME(ast->type));
    }
    return NULL;
}

static
typetag_t* generator_expression(generator_t* self, context_t* context, ast_t* ast) {
    switch (ast->type) {
        case AST_ID: {
            char* ID = ((ast_terminal_t*) ast->value)->value;
            if (!table_defined(context->table, ID)) {
                throw_errorf(ast->position, "undefined variable %s", ID);
            }
            EMIT(str__format("%s", ID));
            symbol_info_t* symbol_info = table_lookup(context->table, ID);
            return typetag_clone(symbol_info->data_type);
        }
        case AST_NUMBER_FROM_INT: {
            char* number = ((ast_terminal_t*) ast->value)->value;
            EMIT(str__format("%s", number));
            return TYPETAG_NUMBER;
        }
        case AST_NUMBER_FROM_FLOAT: {
            char* number = ((ast_terminal_t*) ast->value)->value;
            EMIT(str__format("%s", number));
            return TYPETAG_NUMBER;
        }
        case AST_BIGINT: {
            char* bigint = ((ast_terminal_t*) ast->value)->value;
            EMIT(str__format("%s", bigint));
            return TYPETAG_INT;
        }
        case AST_STRING: {
            char* string = ((ast_terminal_t*) ast->value)->value;
            EMIT(str__format("%s", string));
            return TYPETAG_STRING;
        }
        case AST_BOOL: {
            char* boolean = ((ast_terminal_t*) ast->value)->value;
            EMIT(str__format("%s", boolean));
            return TYPETAG_BOOL;
        }
        case AST_NULL: {
            char* null = ((ast_terminal_t*) ast->value)->value;
            EMIT(str__format("%s", null));
            return TYPETAG_NULL;
        }
        case AST_MEMBER_ACCESS: {
            ast_t* object_to_access = ((ast_access_t*) ast->value)->object;
            ast_t* member_to_access = ((ast_access_t*) ast->value)->member;
            /**************************************************/
            ASSERT_AST_IDENTIFIER(member_to_access);
            char* member = ((ast_terminal_t*) member_to_access->value)->value;

            typetag_t* object_type = 
            generator_expression(self, context, object_to_access);
            if (!typetag_has_member(object_type, member)) {
                throw_errorf(ast->position, "object of type %s has no member %s", typetag_get_name(object_type), member);
            }
            EMIT(".");
            typetag_member_info_t* member_info = typetag_get_member(object_type, member);
            EMIT(str__format("%s", member));
            return typetag_clone(member_info->data_type);
        }
        case AST_CALL: {
            ast_t* function_to_call = ((ast_call_t*) ast->value)->callable;
            ast_t** arguments = ((ast_call_t*) ast->value)->arguments;
            /**************************************************/
            typetag_t* function_type =
            generator_expression(self, context, function_to_call);

            if (!typetag_is_callable(function_type)) {
                throw_errorf(ast->position, "expected a function type, got %s", typetag_get_name(function_type));
            }

            EMIT("(");
            size_t i = 0;
            while (arguments[i] != NULL) {
                typetag_t* argument_type = 
                generator_expression(self, context, arguments[i]);
                if (function_type->is_variadict && function_type->argc > 0) {
                    typetag_t* function_paramN = function_type->param_types[function_type->argc - 1];
                    if (!typetag_can_accept(function_paramN, argument_type)) {
                        throw_errorf(ast->position, "expected type %s, got %s", typetag_get_name(function_paramN), typetag_get_name(argument_type));
                    }
                }
                else if (i < function_type->argc) {
                    typetag_t* function_paramN = function_type->param_types[i];
                    if (!typetag_can_accept(function_paramN, argument_type)) {
                        throw_errorf(ast->position, "expected type %s, got %s", typetag_get_name(function_paramN), typetag_get_name(argument_type));
                    }
                }
                else {
                    throw_errorf(ast->position, "expected %d arguments, got %d", function_type->argc, i);
                }

                if (arguments[++i] != NULL) {
                    EMIT(", ");
                }
            }

            if ((!function_type->is_variadict) && function_type->argc != i) {
                throw_errorf(ast->position, "expected %d arguments, got %d", function_type->argc, i);
            }
            EMIT(")");
        
            return typetag_clone(function_type->return_type);
        }
        // 
        case AST_BINARY_MUL: {
            char*  opt = ((ast_binary_t*) ast->value)->operator;
            ast_t* lhs = ((ast_binary_t*) ast->value)->left;
            ast_t* rhs = ((ast_binary_t*) ast->value)->right;
            /**************************************************/
            typetag_t* ltype =
            generator_expression(self, context, lhs);
            EMIT(str__format(" %s ", opt));
            typetag_t* rtype =
            generator_expression(self, context, rhs);
            typetag_t* dtype;
            ASSERT_TYPE_BINARY_EXPRESSION((dtype = typetag_mul(ltype, rtype)), ltype, rtype, opt);
            return dtype;
        }
        case AST_BINARY_DIV: {
            char*  opt = ((ast_binary_t*) ast->value)->operator;
            ast_t* lhs = ((ast_binary_t*) ast->value)->left;
            ast_t* rhs = ((ast_binary_t*) ast->value)->right;
            /**************************************************/
            typetag_t* ltype =
            generator_expression(self, context, lhs);
            EMIT(str__format(" %s ", opt));
            typetag_t* rtype =
            generator_expression(self, context, rhs);
            typetag_t* dtype;
            ASSERT_TYPE_BINARY_EXPRESSION((dtype = typetag_div(ltype, rtype)), ltype, rtype, opt);
            return dtype;
        }
        case AST_BINARY_MOD: {
            char*  opt = ((ast_binary_t*) ast->value)->operator;
            ast_t* lhs = ((ast_binary_t*) ast->value)->left;
            ast_t* rhs = ((ast_binary_t*) ast->value)->right;
            /**************************************************/
            typetag_t* ltype =
            generator_expression(self, context, lhs);
            EMIT(str__format(" %s ", opt));
            typetag_t* rtype =
            generator_expression(self, context, rhs);
            typetag_t* dtype;
            ASSERT_TYPE_BINARY_EXPRESSION((dtype = typetag_mod(ltype, rtype)), ltype, rtype, opt);
            return dtype;
        }
        case AST_BINARY_ADD: {
            char*  opt = ((ast_binary_t*) ast->value)->operator;
            ast_t* lhs = ((ast_binary_t*) ast->value)->left;
            ast_t* rhs = ((ast_binary_t*) ast->value)->right;
            /**************************************************/
            typetag_t* ltype =
            generator_expression(self, context, lhs);
            EMIT(str__format(" %s ", opt));
            typetag_t* rtype =
            generator_expression(self, context, rhs);
            typetag_t* dtype;
            ASSERT_TYPE_BINARY_EXPRESSION((dtype = typetag_add(ltype, rtype)), ltype, rtype, opt);
            return dtype;
        }
        case AST_BINARY_SUB: {
            char*  opt = ((ast_binary_t*) ast->value)->operator;
            ast_t* lhs = ((ast_binary_t*) ast->value)->left;
            ast_t* rhs = ((ast_binary_t*) ast->value)->right;
            /**************************************************/
            typetag_t* ltype =
            generator_expression(self, context, lhs);
            EMIT(str__format(" %s ", opt));
            typetag_t* rtype =
            generator_expression(self, context, rhs);
            typetag_t* dtype;
            ASSERT_TYPE_BINARY_EXPRESSION((dtype = typetag_sub(ltype, rtype)), ltype, rtype, opt);
            return dtype;
        }
        case AST_BINARY_LSHIFT:
        case AST_BINARY_RSHIFT: {
            char*  opt = ((ast_binary_t*) ast->value)->operator;
            ast_t* lhs = ((ast_binary_t*) ast->value)->left;
            ast_t* rhs = ((ast_binary_t*) ast->value)->right;
            /**************************************************/
            typetag_t* ltype =
            generator_expression(self, context, lhs);
            EMIT(str__format(" %s ", opt));
            typetag_t* rtype =
            generator_expression(self, context, rhs);
            typetag_t* dtype;
            ASSERT_TYPE_BINARY_EXPRESSION((dtype = typetag_shift(ltype, rtype)), ltype, rtype, opt);
            return dtype;
        }
        case AST_BINARY_GT :
        case AST_BINARY_LT :
        case AST_BINARY_GTE:
        case AST_BINARY_LTE: {
            char*  opt = ((ast_binary_t*) ast->value)->operator;
            ast_t* lhs = ((ast_binary_t*) ast->value)->left;
            ast_t* rhs = ((ast_binary_t*) ast->value)->right;
            /**************************************************/
            typetag_t* ltype =
            generator_expression(self, context, lhs);
            EMIT(str__format(" %s ", opt));
            typetag_t* rtype =
            generator_expression(self, context, rhs);
            typetag_t* dtype;
            ASSERT_TYPE_BINARY_EXPRESSION((dtype = typetag_compare(ltype, rtype, true)), ltype, rtype, opt);
            return dtype;
        }
        case AST_BINARY_EQ :
        case AST_BINARY_NEQ: {
            char*  opt = ((ast_binary_t*) ast->value)->operator;
            ast_t* lhs = ((ast_binary_t*) ast->value)->left;
            ast_t* rhs = ((ast_binary_t*) ast->value)->right;
            /**************************************************/
            typetag_t* ltype =
            generator_expression(self, context, lhs);
            EMIT(str__format(" %s ", opt));
            typetag_t* rtype =
            generator_expression(self, context, rhs);
            typetag_t* dtype;
            ASSERT_TYPE_BINARY_EXPRESSION((dtype = typetag_compare(ltype, rtype, false)), ltype, rtype, opt);
            return dtype;
        }
        case AST_TERNARY: {
            ast_t* condition  = ((ast_ternary_t*) ast->value)->condition;
            ast_t* true_expr  = ((ast_ternary_t*) ast->value)->left;
            ast_t* false_expr = ((ast_ternary_t*) ast->value)->right;
            /**************************************************/
            EMIT("(");
            typetag_t* ctype =
            generator_expression(self, context, condition);
            ASSERT_BOOLEAN(ctype);
            EMIT(")");
            EMIT(" ? ");
            typetag_t* ttype = 
            generator_expression(self, context, true_expr);
            EMIT(" : ");
            typetag_t* ftype =
            generator_expression(self, context, false_expr);
            typetag_t* dtype;
            ASSERT_TYPE_MATCH((dtype = typetag_equivalent(ttype, ftype)), ttype, ftype, "?");
            return dtype;
        }
        default:
            DUMP_JSCODE();
            throw_errorf(ast->position, "unknown AST type: %s", AST_XNAME(ast->type));
    }
    return NULL;
}

static
void generator_statement(generator_t* self, context_t* context, ast_t* ast) {
    // NOTE: No Ellie_free(ast) call on statements
    switch (ast->type) {
        case AST_VARIABLE_DECLARATION: {
            if (!context_is_global(context)) {
                throw_errore(ast->position, "variable declaration not allowed here");
            }
            if (context_is_single(context)) {
                throw_errore(ast->position, "var declaration not allowed in single context");
            }
            ast_t** variable_names = ((ast_variable_declaration_t*) ast->value)->variable_name;
            ast_t** variable_types = ((ast_variable_declaration_t*) ast->value)->variable_type;
            ast_t** variable_datas = ((ast_variable_declaration_t*) ast->value)->variable_value;
            /**************************************************/
            EMIT_CHILD("var ");

            size_t i = 0;
            while (variable_names[i] != NULL) {
                ast_t* variable_name = variable_names[i];
                ASSERT_AST_IDENTIFIER(variable_name);

                char* name = ((ast_terminal_t*) variable_name->value)->value;
                if (table_exists(context->table, name)) {
                    throw_errorf(ast->position, "variable %s already defined", name);
                }

                EMIT(str__format("%s", name));

                typetag_t* variable_datatype = generator_datatype(self, context, variable_types[i]);

                ast_t* variable_data = variable_datas[i];
                if (variable_data != NULL) {
                    EMIT(" = ");
                    typetag_t* value_datatype = generator_expression(self, context, variable_data);
                    ASSERT_TYPE_MATCH((typetag_can_accept(variable_datatype, value_datatype)), variable_datatype, value_datatype, "=");
                }

                table_insert(context->table, symbol_info_create_variable(
                    name, 
                    variable_datatype,
                    true,
                    false
                ));
                if (variable_names[++i] != NULL) {
                    EMIT(", ")
                }
            }
            EMIT(";");
            break;
        }
        case AST_CONST_DECLARATION: {
            if (context_is_single(context)) {
                throw_errore(ast->position, "const declaration not allowed in single context");
            }
            ast_t** variable_names = ((ast_variable_declaration_t*) ast->value)->variable_name;
            ast_t** variable_types = ((ast_variable_declaration_t*) ast->value)->variable_type;
            ast_t** variable_datas = ((ast_variable_declaration_t*) ast->value)->variable_value;
            /**************************************************/
            EMIT_CHILD("const ");

            size_t i = 0;
            while (variable_names[i] != NULL) {
                ast_t* variable_name = variable_names[i];
                ASSERT_AST_IDENTIFIER(variable_name);

                char* name = ((ast_terminal_t*) variable_name->value)->value;
                if (table_exists(context->table, name)) {
                    throw_errorf(ast->position, "variable %s already defined", name);
                }

                EMIT(str__format("%s", name));

                typetag_t* variable_datatype = generator_datatype(self, context, variable_types[i]);

                ast_t* variable_data = variable_datas[i];
                if (variable_data != NULL) {
                    EMIT(" = ");
                    typetag_t* value_datatype = generator_expression(self, context, variable_data);
                    ASSERT_TYPE_MATCH((typetag_can_accept(variable_datatype, value_datatype)), variable_datatype, value_datatype, "=");
                } else {
                    throw_errorf(ast->position, "const variable %s must be initialized", name);
                }
                
                table_insert(context->table, symbol_info_create_variable(
                    name, 
                    variable_datatype,
                    context_is_global(context),
                    false
                ));
                if (variable_names[++i] != NULL) {
                    EMIT(", ")
                }
            }
            EMIT(";");
            break;
        }
        case AST_LOCAL_DECLARATION: {
            if (!context_is_local(context)) {
                throw_errore(ast->position, "variable declaration not allowed here");
            }
            if (context_is_single(context)) {
                throw_errore(ast->position, "var declaration not allowed in single context");
            }
            ast_t** variable_names = ((ast_variable_declaration_t*) ast->value)->variable_name;
            ast_t** variable_types = ((ast_variable_declaration_t*) ast->value)->variable_type;
            ast_t** variable_datas = ((ast_variable_declaration_t*) ast->value)->variable_value;
            /**************************************************/
            EMIT_CHILD("let ");

            size_t i = 0;
            while (variable_names[i] != NULL) {
                ast_t* variable_name = variable_names[i];
                ASSERT_AST_IDENTIFIER(variable_name);

                char* name = ((ast_terminal_t*) variable_name->value)->value;
                if (table_exists(context->table, name)) {
                    throw_errorf(ast->position, "variable %s already defined", name);
                }

                EMIT(str__format("%s", name));

                typetag_t* variable_datatype = generator_datatype(self, context, variable_types[i]);

                ast_t* variable_data = variable_datas[i];
                if (variable_data != NULL) {
                    EMIT(" = ");
                    typetag_t* value_datatype = generator_expression(self, context, variable_data);
                    ASSERT_TYPE_MATCH((typetag_can_accept(variable_datatype, value_datatype)), variable_datatype, value_datatype, "=");
                }

                table_insert(context->table, symbol_info_create_variable(
                    name, 
                    variable_datatype,
                    true,
                    false
                ));
                if (variable_names[++i] != NULL) {
                    EMIT(", ")
                }
            }
            EMIT(";");
            break;
        }
        case AST_IF_STATEMENT: {
            ast_t* condition = ((ast_if_t*) ast->value)->condition;
            ast_t* if_body   = ((ast_if_t*) ast->value)->if_body;
            ast_t* else_body = ((ast_if_t*) ast->value)->else_body;
            /**************************************************/
            EMIT_CHILD("if ")
            EMIT("(");
            typetag_t* ctype = 
            generator_expression(self, context, condition);
            ASSERT_BOOLEAN(ctype);
            EMIT(")");
            EMIT(" ");
            context_t* conditional_context = CONTEXT_LOCAL(context);
                context_to_single(conditional_context);
                context_to_conditional(conditional_context);
            if (if_body->type != AST_BLOCK) {
                ASSERT_AST_NOT_EMPTY_STATEMENT(if_body);
                EMIT_NEWLINE();
                EMIT_INDENT();
                generator_statement(self, conditional_context, if_body);
                EMIT_DEDENT();
            } else {
                ASSERT_AST_NOT_EMPTY_STATEMENT(if_body);
                EMIT_NEWLINE();
                generator_statement(self, conditional_context, if_body);
            }
            if (else_body != NULL) {
                EMIT_NEWLINE();
                EMIT_CHILD("else ");
                if (else_body->type != AST_BLOCK) {
                    ASSERT_AST_NOT_EMPTY_STATEMENT(else_body);
                    EMIT_NEWLINE();
                    EMIT_INDENT();
                    generator_statement(self, context, else_body);
                    EMIT_DEDENT();
                } else {
                    ASSERT_AST_NOT_EMPTY_STATEMENT(else_body);
                    EMIT_NEWLINE();
                    generator_statement(self, context, else_body);
                }
            }
            break;
        }
        case AST_ASYNC_FUNCTION:
        case AST_FUNCTION: {
            if (!context_is_global(context)) {
                throw_errore(ast->position, "function declaration not allowed here");
            }

            char* function_name = ((ast_terminal_t*) ((ast_function_t*) ast->value)->function_name->value)->value;
            /**************************************************/
            bool export = ((ast_function_t*) ast->value)->is_export;
            bool async  = ((ast_function_t*) ast->value)->is_async;
            symbol_info_t* symbol_info = table_lookup(context->table, function_name);

            context_t* function_context = (async) ? CONTEXT_AWAITABLE_FUNCTION(context) : CONTEXT_FUNCTION(context);
            context_bind_function_return(function_context, function_name, symbol_info->return_type);
            
            if (export) {
                EMIT_CHILD("export ");
            }

            if (async) {
                EMIT_CHILD("async ");
            } else {
                EMIT_CHILD("");
            }

            EMIT(str__format("function %s", symbol_info->name));
            EMIT("(");
            int argc = 0;
            while (symbol_info->param_names[argc] != NULL) {
                char* /**/ param_name = symbol_info->param_names[argc];
                typetag_t* param_type = symbol_info->param_types[argc];

                if (table_exists(function_context->table, param_name)) {
                    throw_errorf(ast->position, "parameter %s already defined", param_name);
                }

                table_insert(function_context->table, symbol_info_create_variable(
                    param_name, 
                    param_type,
                    false,
                    false
                ));

                EMIT(str__format("%s", param_name));
                if (symbol_info->param_names[++argc] != NULL) {
                    EMIT(", ");
                }
            }
            EMIT(")");
            EMIT(" ");
            context_t* local_context = CONTEXT_LOCAL(function_context);
            EMIT("{");
            EMIT_NEWLINE();
            EMIT_INDENT();
            ast_t** body = ((ast_function_t*) ast->value)->body;
            
            size_t i = 0;
            while (body[i] != NULL) {
                generator_statement(self, local_context, body[i]);
                if (body[++i] != NULL) {
                    EMIT_NEWLINE();
                }
            }
            if (i != 0) EMIT_NEWLINE();
            EMIT_DEDENT();
            EMIT("}");
            if ((!function_context->returned_a_value) && !typetag_is_void(function_context->return_type)) {
                throw_errorf(ast->position, "function %s must return a value of type(%s)", function_name, typetag_get_name(function_context->return_type));
            }
            break;
        }
        case AST_BLOCK: {
            ast_t** body = ((ast_block_t*) ast->value)->children;
            /**************************************************/
            context_t* local_context = CONTEXT_LOCAL(context);
            EMIT_CHILD("{");
            EMIT_NEWLINE();
            EMIT_INDENT();
            size_t i = 0;
            while (body[i] != NULL) {
                generator_statement(self, local_context, body[i]);
                if (body[++i] != NULL) {
                    EMIT_NEWLINE();
                }
            }
            EMIT_DEDENT();
            if (i != 0) EMIT_NEWLINE();
            EMIT_CHILD("}");
            break;
        }
        case AST_RETURN: {
            bool in_function = false;
            context_t* current = context;
            while (current != NULL) {
                if (context_is_function(current)) {
                    in_function = true;
                    break;
                }
                current = current->parent;
            }
            if (!in_function) {
                throw_errore(ast->position, "return statement not allowed here");
            }

            context_t* maybe_conditional = context;
            bool in_conditional = false;
            while (maybe_conditional != NULL) {
                if (context_is_conditional(maybe_conditional)) {
                    in_conditional = true;
                    break;
                }
                maybe_conditional = maybe_conditional->parent;
            }

            ast_t* expression = ((ast_return_t*) ast->value)->return_value;
            /**************************************************/
            EMIT_CHILD("return");
            if (expression != NULL) {
                EMIT(" ");
                typetag_t* dtype = 
                generator_expression(self, context, expression);
                if (!typetag_can_accept(current->return_type, dtype)) {
                    throw_errorf(ast->position, "expected type %s, got %s", typetag_get_name(current->return_type), typetag_get_name(dtype));
                }
                current->returned_a_value = !in_conditional;
            }  else {
                if (!typetag_is_void(current->return_type)) {
                    throw_errorf(ast->position, "function %s must return a value of type(%s)", current->function_name, typetag_get_name(current->return_type));
                }
            }
            EMIT(";");
            break;
        }
        case AST_EXPRESSION_STATEMENT: {
            ast_t* expression = ((ast_expressiont_statement_t*) ast->value)->expression;
            /**********************************/
            EMIT_CHILD("");
            generator_expression(self, context, expression);
            EMIT(";");
            break;
        }
        default:
            DUMP_JSCODE();
            throw_errorf(ast->position, "unknown AST type: %s", AST_XNAME(ast->type));
    }
}

static
void generator_forward_declaration(generator_t* self, context_t* context, ast_t* ast) {
    switch (ast->type) {
        case AST_ASYNC_FUNCTION:
        case AST_FUNCTION: {
            // Dummy context
            context_t* function_context = CONTEXT_FUNCTION(context);
            ast_function_t* function = (ast_function_t*) ast->value;
            /********************************************/ 
            ASSERT_AST_IDENTIFIER(function->function_name);
            char* function_name = ((ast_terminal_t*) function->function_name->value)->value;
            if (table_exists(context->table, function_name)) {
                throw_errorf(ast->position, "name for function %s already defined", function_name);
            }

            /********************************************/ 
            ast_t** parameters_name = function->parameters_name;
            ast_t** parameters_type = function->parameters_type;
            int argc = 0; // supported argument count or number of parameters

            char** param_names = (char**) Ellie_malloc(sizeof(char*));
            typetag_t** param_types = (typetag_t**) Ellie_malloc(sizeof(typetag_t*));

            while (parameters_name[argc] != NULL) {
                ast_t* parameter_name = parameters_name[argc];
                ast_t* parameter_type = parameters_type[argc];
                ASSERT_AST_IDENTIFIER(parameter_name);

                char* /**/ resolved_name = ((ast_terminal_t*) parameter_name->value)->value;
                typetag_t* resolved_type = generator_datatype(self, function_context, parameter_type);

                param_names[argc] = resolved_name;
                param_types[argc] = resolved_type;
                argc++;

                param_names = (char**) Ellie_realloc(param_names, (1 + argc) * sizeof(char*));
                assert_allocation(param_names);
                param_names[argc] = NULL;

                param_types = (typetag_t**) Ellie_realloc(param_types, (1 + argc) * sizeof(typetag_t*));
                assert_allocation(param_types);
                param_types[argc] = NULL;
            }

            typetag_t* return_type = generator_datatype(self, context, function->return_type);

            table_insert(context->table, symbol_info_create_function(
                function_name, 
                typetag_create_function_type(param_types, return_type, argc, false),
                argc,
                param_names,
                param_types,
                return_type,
                function->is_async
            ));
            break;
        }
        default:
            break;
    }
}

static
void generator_program(generator_t* self, ast_t* ast) {
    context_t* context = CONTEXT_GLOBAL();
    js_link_init(context);

    ast_t** body = ((ast_program_t*) ast->value)->children;
    size_t i;

    i = 0;
    while (body[i] != NULL) {
        generator_forward_declaration(self, context, body[i++]);
    }

    i = 0;
    while (body[i] != NULL) {
        generator_statement(self, context, body[i]);
        if (body[++i] != NULL) {
            EMIT_NEWLINE();
        }
    }
}

void* generator_generate(generator_t* self) {
    ast_t* ast = parser_parse(self->parser);
    generator_program(self, ast);
    GC_collect_a_little();
    DUMP_JSCODE();
    return self->js_code;
}