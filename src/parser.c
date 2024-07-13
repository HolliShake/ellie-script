#include "parser.h"

parser_t* parser_create(char* file_path_or_name, char* source) {
    parser_t* parser = (parser_t*) Ellie_malloc(sizeof(parser_t));
    assert_allocation(parser);
    parser->lexer = lexer_create(file_path_or_name, source);
    // lexer_dump(parser->lexer);
    // exit(0);
    return parser;
}

#define check_ttype(ttype) parser_check(self, ttype,  NULL, true )
#define check_value(value) parser_check(self,     0, value, false)

#define accept_ttype(ttype) parser_accept(self, ttype,  NULL, true )
#define accept_value(value) parser_accept(self,     0, value, false)

static
bool parser_check(parser_t* self, token_type_t if_type, char* if_value, bool is_type) {
    return (is_type) ? (self->curr_token->type == if_type) : str__equals(self->curr_token->value, if_value);
}

static
void parser_accept(parser_t* self, token_type_t if_type, char* if_value, bool is_type) {
    if (parser_check(self, if_type, if_value, is_type)) {
        self->prev_token = self->curr_token;
        self->curr_token = lexer_next_token(self->lexer);
        return;
    }

    if (!is_type) {
        throw_errorf(
            self->curr_token->position, 
            "unexpected token \"%s\", did you mean \"%s\"?", 
            self->curr_token->value,
            if_value
        );
    } else {
        throw_errorf(
            self->curr_token->position, 
            "expected token type \"%s\", got \"%s\"", 
            TOKEN_XNAME(if_type), 
            TOKEN_XNAME(self->curr_token->type)
        );
    }
}

/*******************/ 

#define create_ast_array(label) (ast_t**) Ellie_malloc(sizeof(ast_t*)); size_t array_##label = 1;

#define array_ast_ptr_push(label, array, value) {\
        array[array_##label - 1] = value;\
        array = (ast_t**) Ellie_realloc(array, sizeof(ast_t*) * (1 + array_##label));\
        assert_allocation(array);\
        array[ array_##label++ ] = NULL;\
    }\

static ast_t* parser_type(parser_t* self);
static ast_t* parser_expression(parser_t* self);
static ast_t* parser_mandatory_expression(parser_t* self);
static ast_t* parser_statement(parser_t* self);

static
ast_t* parser_terminal(parser_t* self) {
    if (check_ttype(TOKEN_ID)) {
        ast_t* terminal = ast_create(
            AST_ID, 
            self->curr_token->position, 
            ast_terminal_create(self->curr_token->value)
        );
        accept_ttype(TOKEN_ID);
        return terminal;
    }
    else if (check_ttype(TOKEN_INT)) {
        ast_t* terminal = ast_create(
            AST_NUMBER_FROM_INT, 
            self->curr_token->position, 
            ast_terminal_create(self->curr_token->value)
        );
        accept_ttype(TOKEN_INT);
        return terminal;
    }
    else if (check_ttype(TOKEN_FLOAT)) {
        ast_t* terminal = ast_create(
            AST_NUMBER_FROM_FLOAT, 
            self->curr_token->position, 
            ast_terminal_create(self->curr_token->value)
        );
        accept_ttype(TOKEN_FLOAT);
        return terminal;
    }
    else if (check_ttype(TOKEN_BIGINT)) {
        ast_t* terminal = ast_create(
            AST_BIGINT, 
            self->curr_token->position, 
            ast_terminal_create(self->curr_token->value)
        );
        accept_ttype(TOKEN_BIGINT);
        return terminal;
    }
    else if (check_ttype(TOKEN_STRING)) {
        ast_t* terminal = ast_create(
            AST_STRING, 
            self->curr_token->position, 
            ast_terminal_create(self->curr_token->value)
        );
        accept_ttype(TOKEN_STRING);
        return terminal;
    }
    else if (check_value(KEYWORD_TRUE) || check_value(KEYWORD_FALSE)) {
        ast_t* terminal = ast_create(
            AST_BOOL, 
            self->curr_token->position, 
            ast_terminal_create(self->curr_token->value)
        );
        accept_ttype(TOKEN_KEY);
        return terminal;
    }
    else if (check_value(KEYWORD_NULL)) {
        ast_t* terminal = ast_create(
            AST_NULL, 
            self->curr_token->position, 
            ast_terminal_create(self->curr_token->value)
        );
        accept_ttype(TOKEN_KEY);
        return terminal;
    }
    // typing
    else if (check_value(KEYWORD_NUMBER)) {
        ast_t* terminal = ast_create(
            AST_NUMBER_DATA_TYPE, 
            self->curr_token->position, 
            ast_terminal_create(self->curr_token->value)
        );
        accept_ttype(TOKEN_KEY);
        return terminal;
    }
    else if (check_value(KEYWORD_INT)) {
        ast_t* terminal = ast_create(
            AST_BIGINT_DATA_TYPE, 
            self->curr_token->position, 
            ast_terminal_create(self->curr_token->value)
        );
        accept_ttype(TOKEN_KEY);
        return terminal;
    }
    else if (check_value(KEYWORD_STRING)) {
        ast_t* terminal = ast_create(
            AST_STRING_DATA_TYPE, 
            self->curr_token->position, 
            ast_terminal_create(self->curr_token->value)
        );
        accept_ttype(TOKEN_KEY);
        return terminal;
    }
    else if (check_value(KEYWORD_BOOL)) {
        ast_t* terminal = ast_create(
            AST_BOOL_DATA_TYPE, 
            self->curr_token->position, 
            ast_terminal_create(self->curr_token->value)
        );
        accept_ttype(TOKEN_KEY);
        return terminal;
    }
    else if (check_value(KEYWORD_VOID)) {
        ast_t* terminal = ast_create(
            AST_VOID_DATA_TYPE, 
            self->curr_token->position, 
            ast_terminal_create(self->curr_token->value)
        );
        accept_ttype(TOKEN_KEY);
        return terminal;
    }
    return NULL;
}

static
ast_t* parser_object_data_type(parser_t* self) {
    position_t *start = self->curr_token->position, *ended = NULL;
    accept_value(KEYWORD_OBJECT);
    accept_value("<");
    ast_t* key = parser_type(self);
    accept_value(",");
    ast_t* val = parser_type(self);
    accept_value(">");
    ended = self->prev_token->position;
    return ast_create(
        AST_OBJECT_DATA_TYPE, 
        position_merge(start, ended), 
        ast_object_type_create(key, val)
    );
}

static
ast_t* parser_earray_data_type(parser_t* self) {
    position_t *start = self->curr_token->position, *ended = NULL;
    accept_value(KEYWORD_ARRAY);
    accept_value("<");
    ast_t* element_type = parser_type(self);
    accept_value(">");
    ended = self->prev_token->position;
    return ast_create(
        AST_ARRAY_DATA_TYPE, 
        position_merge(start, ended), 
        ast_array_type_create(element_type)
    );
}

static ast_t* parser_group(parser_t* self) {
    if (check_value("(")) {
        accept_value("(");
        ast_t* node = parser_mandatory_expression(self);
        accept_value(")");
        return node;
    }
    else if (check_value(KEYWORD_OBJECT) || check_value("{")) {
        position_t *start  = self->curr_token->position, *ended = NULL;
        ast_t* object_type = (check_value(KEYWORD_OBJECT))? parser_object_data_type(self) : NULL;

        // data type mode
        if (!check_value("{")) {
            return object_type;
        }
        // record mode
        accept_value("{");
        ast_t** keys = create_ast_array(ksize);
        ast_t** values = create_ast_array(vsize);

        ast_t* key = parser_expression(self), *val = NULL;
        if (key == NULL) {
            if (check_value("...")) {
                val = parser_expression(self);
                array_ast_ptr_push(ksize, keys, key);
                array_ast_ptr_push(vsize, values, val);
            }
        }
        if (key != NULL) {
            accept_value(":");
            val = parser_mandatory_expression(self);

            array_ast_ptr_push(ksize, keys, key);
            array_ast_ptr_push(vsize, values, val);

            while (check_value(",")) {
                char* comma = self->curr_token->value;
                accept_value(",");

                bool is_spread = check_value("...");
                key = parser_expression(self);
                if (key == NULL) {
                    throw_errorf(self->prev_token->position, "missing key, after \"%s\", or remove this/these extra token", comma);
                }
                if (is_spread) {
                    val = key;
                    key = NULL;
                } 
                else {
                    accept_value(":");
                    val = parser_mandatory_expression(self);
                }
                
                array_ast_ptr_push(ksize, keys, key);
                array_ast_ptr_push(vsize, values, val);
            }
        }
        accept_value("}");
        ended = self->prev_token->position;
        return ast_create(
            AST_OBJECT, 
            position_merge(start, ended), 
            ast_object_create(object_type, keys, values)
        );
    }
    else if (check_value(KEYWORD_ARRAY) || check_value("[")) {
        position_t *start = self->curr_token->position, *ended = NULL;
        ast_t* array_type = (check_value(KEYWORD_ARRAY)) ? parser_earray_data_type(self) : NULL;

        // data type mode
        if (!check_value("[")) {
            return array_type;
        }
        // list mode
        accept_value("[");
        ast_t** elements = create_ast_array(esize);

        ast_t* element = parser_expression(self);
        if (element == NULL) {
            if (check_value("...")) {
                element = parser_expression(self);
                array_ast_ptr_push(esize, elements, element);
            }
        }
        if (element != NULL) {
            array_ast_ptr_push(esize, elements, element);
            while (check_value(",")) {
                char* comma = self->curr_token->value;
                accept_value(",");

                element = parser_expression(self);
                if (element == NULL) {
                    throw_errorf(self->prev_token->position, "missing element, after \"%s\", or remove this/these extra token", comma);
                }
                array_ast_ptr_push(esize, elements, element);
            }
        }
        accept_value("]");
        ended = self->prev_token->position;
        return ast_create(
            AST_ARRAY, 
            position_merge(start, ended), 
            ast_array_create(array_type, elements)
        );
    }
    return parser_terminal(self);
}

static ast_t* parser_type(parser_t* self) {
    position_t *start=self->curr_token->position, *ended = NULL;
    /*********************************************/
    bool object_type = check_value(KEYWORD_OBJECT);
    ast_t* data_type = check_value(KEYWORD_OBJECT) 
                     ? parser_object_data_type(self) : check_value(KEYWORD_ARRAY)
                     ? parser_earray_data_type(self) : parser_group(self);

    if (data_type == NULL) {
        throw_errore(self->curr_token->position, "missing data type");
    }

    if (check_value("[")) {
        accept_value("[");
        accept_value("]");
        ended = self->prev_token->position;
        data_type = ast_create(
            AST_ARRAY_DATA_TYPE, 
            position_merge(start, ended), 
            ast_array_type_create(data_type)
        );
    }

    if (check_value("?")) {
        accept_value("?");
        ended = self->prev_token->position;
        data_type = ast_create(
            AST_NULLABLE_DATA_TYPE, 
            position_merge(start, ended), 
            ast_nullable_type_create(data_type)
        );
    }
    return data_type;
}

static ast_t* parser_member_or_call(parser_t* self) {
    position_t *start = self->curr_token->position, *ended = NULL;
    ast_t* node = parser_group(self);
    if (node == NULL) {
        return NULL;
    }
    while (check_value(".") || check_value("?.") || check_value("[") || check_value("(")) {
        if (check_value(".")) {
            char* operator = self->curr_token->value;
            accept_value(".");
            
            ast_t* member = parser_terminal(self);
            if (member == NULL) {
                throw_errorf(self->curr_token->position, "missing member name, after \"?.\" operator", operator);
            }
            ended = self->prev_token->position;
            node = ast_create(
                AST_MEMBER_ACCESS, 
                position_merge(start, ended), 
                ast_access_create(node, member)
            );
        }
        else if (check_value("?.")) {
            char* operator = self->curr_token->value;
            accept_value("?.");

            ast_t* member = parser_terminal(self);
            if (member == NULL) {
                throw_errorf(self->curr_token->position, "missing member name, after \"?.\" operator", operator);
            }
            ended = self->prev_token->position;
            node = ast_create(
                AST_NULLABLE_ACCESS, 
                position_merge(start, ended), 
                ast_access_create(node, member)
            );
        }
        else if (check_value("[")) {
            accept_value("[");
            ast_t* index = parser_expression(self);
            if (index == NULL) {
                throw_errore(self->curr_token->position, "missing index value");
            }
            accept_value("]");
            ended = self->prev_token->position;
            node = ast_create(
                AST_INDEX_ACCESS, 
                position_merge(start, ended), 
                ast_access_create(node, index)
            );
        }
        else if (check_value("(")) {
            accept_value("(");
            ast_t** arguments = create_ast_array(asize);

            ast_t* argument = parser_expression(self);
            if (argument != NULL) {
                array_ast_ptr_push(asize, arguments, argument);
                while (check_value(",")) {
                    char* comma = self->curr_token->value;
                    accept_value(",");

                    argument = parser_expression(self);
                    if (argument == NULL) {
                        throw_errorf(self->prev_token->position, "missing argument, after \"%s\", or remove this/these extra token", comma);
                    }
                    array_ast_ptr_push(asize, arguments, argument);
                }
            }
            accept_value(")");
            ended = self->prev_token->position;
            node = ast_create(
                AST_CALL, 
                position_merge(start, ended), 
                ast_call_create(node, arguments)
            );
        }
    }
    return node;
}

static ast_t* parser_postfix_expression(parser_t* self) {
    ast_t* node = parser_member_or_call(self);
    if (node == NULL) {
        return NULL;
    }
    if (check_value("++") || check_value("--")) {
        char* operator = self->curr_token->value;
        accept_value(operator);
        return ast_create(
            (str__equals(operator, "++") ? AST_POSTFIX_INC : AST_POSTFIX_DEC),
            position_merge(node->position, self->prev_token->position), 
            ast_postfix_create(operator, node)
        );
    }
    return node;
}

static ast_t* parser_unary_expression(parser_t* self) {
    position_t *start = self->curr_token->position;
    if (check_value("!") || check_value("~") || check_value("+") || check_value("-") || check_value("++") || check_value("--") || check_value("...")) {
        char* operator = self->curr_token->value;
        accept_value(operator);

        ast_t* operand = parser_unary_expression(self);
        if (operand == NULL) {
            throw_errorf(self->curr_token->position, "missing operand for unary operator \"%s\"", operator);
        }
        return ast_create(
            (str__equals(operator, "!") 
            ? AST_UNARY_NOT  : (str__equals(operator,  "~")
            ? AST_UNARY_BNOT : (str__equals(operator,  "+")
            ? AST_UNARY_POS  : (str__equals(operator,  "-")
            ? AST_UNARY_NEG  : (str__equals(operator, "++")
            ? AST_UNARY_INC  : (str__equals(operator, "--")
            ? AST_UNARY_DEC  : AST_UNARY_SPREAD)))))),
            position_merge(start, operand->position), 
            ast_unary_create(operator, operand)
        );
    }
    return parser_postfix_expression(self);
}

static ast_t* parser_multiplicative_expression(parser_t* self) {
    ast_t* left = parser_unary_expression(self);
    if (left == NULL) {
        return NULL;
    }
    while (check_value("*") || check_value("/") || check_value("%")) {
        char* operator = self->curr_token->value;
        accept_value(operator);

        ast_t* right = parser_unary_expression(self);
        if (right == NULL) {
            throw_errorf(self->curr_token->position, "missing right operand for operator \"%s\"", operator);
        }

        left = ast_create(
            (str__equals(operator, "*") ? AST_BINARY_MUL : (str__equals(operator, "/") ? AST_BINARY_DIV : AST_BINARY_MOD)), 
            position_merge(left->position, right->position), 
            ast_binary_create(operator, left, right)
        );
    }
    return left;
}

static ast_t* parser_addetive_expression(parser_t* self) {
    ast_t* left = parser_multiplicative_expression(self);
    if (left == NULL) {
        return NULL;
    }
    while (check_value("+") || check_value("-")) {
        char* operator = self->curr_token->value;
        accept_value(operator);

        ast_t* right = parser_multiplicative_expression(self);
        if (right == NULL) {
            throw_errorf(self->curr_token->position, "missing right operand for operator \"%s\"", operator);
        }

        left = ast_create(
            (str__equals(operator, "+") ? AST_BINARY_ADD : AST_BINARY_SUB),
            position_merge(left->position, right->position), 
            ast_binary_create(operator, left, right)
        );
    }
    return left;
}

static ast_t* parser_shift_expression(parser_t* self) {
    ast_t* left = parser_addetive_expression(self);
    if (left == NULL) {
        return NULL;
    }
    while (check_value("<<") || check_value(">>")) {
        char* operator = self->curr_token->value;
        accept_value(operator);

        ast_t* right = parser_addetive_expression(self);
        if (right == NULL) {
            throw_errorf(self->curr_token->position, "missing right operand for operator \"%s\"", operator);
        }

        left = ast_create(
            (str__equals(operator, "<<") ? AST_BINARY_LSHIFT : AST_BINARY_RSHIFT),
            position_merge(left->position, right->position), 
            ast_binary_create(operator, left, right)
        );
    }
    return left;
}

static ast_t* parser_relational_expression(parser_t* self) {
    ast_t* left = parser_shift_expression(self);
    if (left == NULL) {
        return NULL;
    }
    while (check_value("<") || check_value(">") || check_value("<=") || check_value(">=")) {
        char* operator = self->curr_token->value;
        accept_value(operator);

        ast_t* right = parser_shift_expression(self);
        if (right == NULL) {
            throw_errorf(self->curr_token->position, "missing right operand for operator \"%s\"", operator);
        }

        left = ast_create(
            (str__equals(operator, "<") 
            ? AST_BINARY_LT  : (str__equals(operator,  ">") 
            ? AST_BINARY_GT  : (str__equals(operator, "<=") 
            ? AST_BINARY_LTE : AST_BINARY_GTE))),
            position_merge(left->position, right->position), 
            ast_binary_create(operator, left, right)
        );
    }
    return left;
}

static ast_t* parser_equality_expression(parser_t* self) {
    ast_t* left = parser_relational_expression(self);
    if (left == NULL) {
        return NULL;
    }
    while (check_value("==") || check_value("!=")) {
        char* operator = self->curr_token->value;
        accept_value(operator);

        ast_t* right = parser_relational_expression(self);
        if (right == NULL) {
            throw_errorf(self->curr_token->position, "missing right operand for operator \"%s\"", operator);
        }

        left = ast_create(
            (str__equals(operator, "==") ? AST_BINARY_EQ : AST_BINARY_NEQ),
            position_merge(left->position, right->position), 
            ast_binary_create(operator, left, right)
        );
    }
    return left;
}

static ast_t* parser_bitwise_expression(parser_t* self) {
    ast_t* left = parser_equality_expression(self);
    if (left == NULL) {
        return NULL;
    }
    while (check_value("&") || check_value("|") || check_value("^")) {
        char* operator = self->curr_token->value;
        accept_value(operator);

        ast_t* right = parser_equality_expression(self);
        if (right == NULL) {
            throw_errorf(self->curr_token->position, "missing right operand for operator \"%s\"", operator);
        }

        left = ast_create(
            (str__equals(operator, "&") 
            ? AST_BINARY_AND : (str__equals(operator, "|") 
            ? AST_BINARY_OR  : AST_BINARY_XOR)),
            position_merge(left->position, right->position), 
            ast_binary_create(operator, left, right)
        );
    }
    return left;
}

static ast_t* parser_logical_expression(parser_t* self) {
    ast_t* left = parser_bitwise_expression(self);
    if (left == NULL) {
        return NULL;
    }
    while (check_value("&&") || check_value("||")) {
        char* operator = self->curr_token->value;
        accept_value(operator);

        ast_t* right = parser_bitwise_expression(self);
        if (right == NULL) {
            throw_errorf(self->curr_token->position, "missing right operand for operator \"%s\"", operator);
        }

        left = ast_create(
            (str__equals(operator, "&&") ? AST_LOGICAL_AND : AST_LOGICAL_OR),
            position_merge(left->position, right->position), 
            ast_binary_create(operator, left, right)
        );
    }
    return left;
}

static ast_t* parser_ternary_expression(parser_t* self) {
    ast_t* condition = parser_logical_expression(self);
    if (condition == NULL) {
        return NULL;
    }
    if (check_value("?")) {
        accept_value("?");

        ast_t* when_t = parser_ternary_expression(self);
        if (when_t == NULL) {
            throw_errore(self->curr_token->position, "missing true value");
        }
        accept_value(":");

        ast_t* when_f = parser_ternary_expression(self);
        if (when_f == NULL) {
            throw_errore(self->curr_token->position, "missing false value");
        }

        return ast_create(
            AST_TERNARY, 
            position_merge(condition->position, when_f->position), 
            ast_ternary_create(condition, when_t, when_f)
        );
    }
    return condition;
}

static ast_t* parser_assignment_expression(parser_t* self) {
    ast_t* left = parser_ternary_expression(self);
    if (left == NULL) {
        return NULL;
    }
    while (check_value("=")) {
        char* operator = self->curr_token->value;
        accept_value(operator);

        ast_t* right = parser_assignment_expression(self);
        if (right == NULL) {
            throw_errorf(self->curr_token->position, "missing right operand for operator \"%s\"", operator);
        }
        return ast_create(
            AST_ASSIGN, 
            position_merge(left->position, right->position), 
            ast_binary_create(operator, left, right)
        );
    }
    return left;
}

static ast_t* parser_augmented_assignment_expression(parser_t* self) {
    ast_t* left = parser_assignment_expression(self);
    if (left == NULL) {
        return NULL;
    }
    while (
        check_value("*=" ) ||
        check_value("/=" ) ||
        check_value("%=" ) ||
        check_value("+=" ) ||
        check_value("-=" ) ||
        check_value("<<=") ||
        check_value(">>=") ||
        check_value("&=" ) ||
        check_value("|=" ) ||
        check_value("^=" )
    ) {
        char* operator = self->curr_token->value;
        accept_value(operator);

        ast_t* right = parser_augmented_assignment_expression(self);
        if (right == NULL) {
            throw_errorf(self->curr_token->position, "missing right operand for operator \"%s\"", operator);
        }
        return ast_create(
            (str__equals(operator, "*=" ) 
            ? AST_ASSIGN_MUL    : str__equals(operator, "/=" )
            ? AST_ASSIGN_DIV    : str__equals(operator, "%=" )
            ? AST_ASSIGN_MOD    : str__equals(operator, "+=" )
            ? AST_ASSIGN_ADD    : str__equals(operator, "-=" )
            ? AST_ASSIGN_SUB    : str__equals(operator, "<<=")
            ? AST_ASSIGN_LSHIFT : str__equals(operator, ">>=")
            ? AST_ASSIGN_RSHIFT : str__equals(operator, "&=" )
            ? AST_ASSIGN_AND    : str__equals(operator, "|=" )
            ? AST_ASSIGN_OR     : str__equals(operator, "^=" )
            ? AST_ASSIGN_XOR    : AST_ASSIGN),
            position_merge(left->position, right->position), 
            ast_binary_create(operator, left, right)
        );
    }
    return left;
}

static ast_t* parser_expression(parser_t* self) {
    return parser_augmented_assignment_expression(self);
}

static ast_t* parser_mandatory_expression(parser_t* self) {
    ast_t* node = parser_expression(self);
    if (node) {
        return node;
    }
    throw_errore(self->curr_token->position, "an expression is required, but none was found");
    return NULL;
}

/******************************************/
static
ast_t* parser_variable_declaration(parser_t* self, bool is_export);
static
ast_t* parser_const_declaration(parser_t* self, bool is_export);
static
ast_t* parser_async_function(parser_t* self, bool is_export);
static
ast_t* parser_function(parser_t* self, bool is_export, bool is_async);

static
ast_t* parser_public(parser_t* self) {
    position_t* pub_position_starts = self->curr_token->position;
    accept_value(KEYWORD_PUB);
    if (check_value(KEYWORD_VAR)) {
        return parser_variable_declaration(self, true);
    }
    else if (check_value(KEYWORD_CONST)) {
        return parser_const_declaration(self, true);
    }
    else if (check_value(KEYWORD_ASYNC)) {
        return parser_async_function(self, true);
    }
    else if (check_value(KEYWORD_FN)) {
        return parser_function(self, true, false);
    }
    else {
        ast_t* statement = parser_statement(self);
        (statement == NULL) 
        ? throw_errore(pub_position_starts, "incomplete or missing statement")
        : throw_errore(statement->position, "invalid use of pub statement"   );
        return NULL;
    }
}

static
ast_t* parser_variable_declaration(parser_t* self, bool is_export) {
    position_t *start = self->curr_token->position, *ended = NULL;
    accept_value(KEYWORD_VAR);

    ast_t** vrble_names = create_ast_array(vsize);
    ast_t** vrble_types = create_ast_array(tsize);
    ast_t** vrble_datas = create_ast_array(dsize);

    ast_t* vrble_name = NULL, *vrble_type = NULL, *vrble_data = NULL;

    vrble_name = parser_terminal(self);
    if (vrble_name == NULL) {
        throw_errore(self->curr_token->position, "missing variable name");
    }

    vrble_type = parser_type(self);
    if (check_value("=")) {
        accept_value("=");
        vrble_data = parser_expression(self);
        if (vrble_data == NULL) {
            throw_errore(self->curr_token->position, "missing variable value");
        }
    }

    array_ast_ptr_push(vsize, vrble_names, vrble_name);
    array_ast_ptr_push(tsize, vrble_types, vrble_type);
    array_ast_ptr_push(dsize, vrble_datas, vrble_data);

    while (check_value(",")) {
        char* comma = self->curr_token->value;
        accept_value(",");

        vrble_name = parser_terminal(self);
        vrble_data = NULL;
        if (vrble_name == NULL) {
            throw_errorf(self->prev_token->position, "missing variable name, after \"%s\", or remove this/these extra token", comma);
        }
        
        vrble_type = parser_type(self);
        if (check_value("=")) {
            accept_value("=");
            vrble_data = parser_expression(self);
            if (vrble_data == NULL) {
                throw_errore(self->curr_token->position, "missing variable value");
            }
        }

        array_ast_ptr_push(vsize, vrble_names, vrble_name);
        array_ast_ptr_push(tsize, vrble_types, vrble_type);
        array_ast_ptr_push(dsize, vrble_datas, vrble_data);
    }
    
    accept_value(";");
    ended = self->prev_token->position;

    return ast_create(
        AST_VARIABLE_DECLARATION, 
        position_merge(start, ended), 
        ast_variable_declaration_create(vrble_names, vrble_types, vrble_datas, is_export)
    );
}

static
ast_t* parser_const_declaration(parser_t* self, bool is_export) {
    position_t *start = self->curr_token->position, *ended = NULL;
    accept_value(KEYWORD_CONST);

    ast_t** vrble_names = create_ast_array(vsize);
    ast_t** vrble_types = create_ast_array(tsize);
    ast_t** vrble_datas = create_ast_array(dsize);

    ast_t* vrble_name = NULL, *vrble_type = NULL, *vrble_data = NULL;

    vrble_name = parser_terminal(self);
    if (vrble_name == NULL) {
        throw_errore(self->curr_token->position, "missing variable name");
    }

    vrble_type = parser_type(self);
    if (check_value("=")) {
        accept_value("=");
        vrble_data = parser_expression(self);
        if (vrble_data == NULL) {
            throw_errore(self->curr_token->position, "missing variable value");
        }
    }

    array_ast_ptr_push(vsize, vrble_names, vrble_name);
    array_ast_ptr_push(tsize, vrble_types, vrble_type);
    array_ast_ptr_push(dsize, vrble_datas, vrble_data);

    while (check_value(",")) {
        char* comma = self->curr_token->value;
        accept_value(",");

        vrble_name = parser_terminal(self);
        vrble_data = NULL;
        if (vrble_name == NULL) {
            throw_errorf(self->prev_token->position, "missing variable name, after \"%s\", or remove this/these extra token", comma);
        }
        
        vrble_type = parser_type(self);
        if (check_value("=")) {
            accept_value("=");
            vrble_data = parser_expression(self);
            if (vrble_data == NULL) {
                throw_errore(self->curr_token->position, "missing variable value");
            }
        }

        array_ast_ptr_push(vsize, vrble_names, vrble_name);
        array_ast_ptr_push(tsize, vrble_types, vrble_type);
        array_ast_ptr_push(dsize, vrble_datas, vrble_data);
    }
    
    accept_value(";");
    ended = self->prev_token->position;

    return ast_create(
        AST_CONST_DECLARATION, 
        position_merge(start, ended), 
        ast_variable_declaration_create(vrble_names, vrble_types, vrble_datas, is_export)
    );
}

static
ast_t* parser_local_declaration(parser_t* self) {
    position_t *start = self->curr_token->position, *ended = NULL;
    accept_value(KEYWORD_LET);

    ast_t** vrble_names = create_ast_array(vsize);
    ast_t** vrble_types = create_ast_array(tsize);
    ast_t** vrble_datas = create_ast_array(dsize);

    ast_t* vrble_name = NULL, *vrble_type = NULL, *vrble_data = NULL;

    vrble_name = parser_terminal(self);
    if (vrble_name == NULL) {
        throw_errore(self->curr_token->position, "missing variable name");
    }

    vrble_type = parser_type(self);
    if (check_value("=")) {
        accept_value("=");
        vrble_data = parser_expression(self);
        if (vrble_data == NULL) {
            throw_errore(self->curr_token->position, "missing variable value");
        }
    }

    array_ast_ptr_push(vsize, vrble_names, vrble_name);
    array_ast_ptr_push(tsize, vrble_types, vrble_type);
    array_ast_ptr_push(dsize, vrble_datas, vrble_data);

    while (check_value(",")) {
        char* comma = self->curr_token->value;
        accept_value(",");

        vrble_name = parser_terminal(self);
        vrble_data = NULL;
        if (vrble_name == NULL) {
            throw_errorf(self->prev_token->position, "missing variable name, after \"%s\", or remove this/these extra token", comma);
        }
        
        vrble_type = parser_type(self);
        if (check_value("=")) {
            accept_value("=");
            vrble_data = parser_expression(self);
            if (vrble_data == NULL) {
                throw_errore(self->curr_token->position, "missing variable value");
            }
        }

        array_ast_ptr_push(vsize, vrble_names, vrble_name);
        array_ast_ptr_push(tsize, vrble_types, vrble_type);
        array_ast_ptr_push(dsize, vrble_datas, vrble_data);
    }
    
    accept_value(";");
    ended = self->prev_token->position;

    return ast_create(
        AST_LOCAL_DECLARATION, 
        position_merge(start, ended), 
        ast_variable_declaration_create(vrble_names, vrble_types, vrble_datas, false)
    );
}

static
ast_t* parser_if_statement(parser_t* self) {
    position_t *start = self->curr_token->position, *ended = NULL;
    accept_value(KEYWORD_IF);
    accept_value("(");
    ast_t* condition = parser_expression(self);
    if (condition == NULL) {
        throw_errore(self->curr_token->position, "missing if/else condition");
    }
    accept_value(")");
    ast_t* if_block = parser_statement(self);
    if (if_block == NULL) {
        throw_errore(self->curr_token->position, "missing if block");
    }
    ast_t* else_block = NULL;
    if (check_value(KEYWORD_ELSE)) {
        accept_value(KEYWORD_ELSE);
        else_block = parser_statement(self);
        if (else_block == NULL) {
            throw_errore(self->curr_token->position, "missing else block");
        }
    }
    ended = self->prev_token->position;

    return ast_create(
        AST_IF_STATEMENT, 
        position_merge(start, ended), 
        ast_if_create(condition, if_block, else_block)
    );
}

static
ast_t* parser_while_statement(parser_t* self) {
    position_t *start = self->curr_token->position, *ended = NULL;
    accept_value(KEYWORD_WHILE);
    accept_value("(");
    ast_t* condition = parser_expression(self);
    if (condition == NULL) {
        throw_errore(self->curr_token->position, "missing while condition");
    }
    accept_value(")");
    ast_t* block = parser_statement(self);
    if (block == NULL) {
        throw_errore(self->curr_token->position, "missing while block");
    }
    ended = self->prev_token->position;

    return ast_create(
        AST_WHILE_STATEMENT, 
        position_merge(start, ended), 
        ast_while_create(condition, block)
    );
}

static
ast_t* parser_async_function(parser_t* self, bool is_export) {
    accept_value(KEYWORD_ASYNC);
    return parser_function(self, is_export, true);
}

static
ast_t* parser_function(parser_t* self, bool is_export, bool is_async) {
    position_t *start = self->curr_token->position, *ended = NULL;
    accept_value(KEYWORD_FN);
    ast_t* id = parser_terminal(self);
    if (id == NULL) {
        throw_errore(self->curr_token->position, "missing function name");
    }

    accept_value("(");
    ast_t** param_names = create_ast_array(psize);
    ast_t** param_types = create_ast_array(tsize);

    ast_t* param_name = parser_terminal(self), *param_type = NULL;
    if (param_name != NULL) {
        param_type = parser_type(self);

        array_ast_ptr_push(psize, param_names, param_name);
        array_ast_ptr_push(tsize, param_types, param_type);

        while (check_value(",")) {
            char* comma = self->curr_token->value;
            accept_value(",");

            param_name = parser_terminal(self);
            if (param_name == NULL) {
                throw_errorf(self->prev_token->position, "missing parameter name, after \"%s\", or remove this/these extra token", comma);
            }
           
            param_type = parser_type(self);

            array_ast_ptr_push(psize, param_names, param_name);
            array_ast_ptr_push(tsize, param_types, param_type);
        }
    }
    accept_value(")");

    ast_t* return_type = parser_type(self);

    // begin body
    accept_value("{");
    ast_t** children = create_ast_array(bsize);
    assert_allocation(children);

    ast_t* child = parser_statement(self);
    while (child != NULL) {
        array_ast_ptr_push(bsize, children, child);
        child = parser_statement(self);
    }
    accept_value("}");
    ended = self->prev_token->position;

    return ast_create(
        (is_async) ? AST_ASYNC_FUNCTION : AST_FUNCTION, 
        position_merge(start, ended), 
        ast_function_create(id, param_names, param_types, return_type, children, is_export, is_async)
    );
}

static
ast_t* parser_block(parser_t* self) {
    position_t *start = self->curr_token->position, *ended = NULL;
    accept_value("{");
    ast_t** children = create_ast_array(bsize);
    assert_allocation(children);

    ast_t* child = parser_statement(self);
    while (child != NULL) {
        array_ast_ptr_push(bsize, children, child);
        child = parser_statement(self);
    }
    accept_value("}");
    ended = self->prev_token->position;

    return ast_create(
        AST_BLOCK, 
        position_merge(start, ended), 
        ast_block_create(children)
    );
}

static
ast_t* parser_return_statement(parser_t* self) {
    position_t *start = self->curr_token->position, *ended = NULL;
    accept_value(KEYWORD_RETURN);
    ast_t* value = parser_expression(self);
    accept_value(";");
    ended = self->prev_token->position;
    return ast_create(
        AST_RETURN, 
        position_merge(start, ended), 
        ast_return_create(value)
    );
}

static
ast_t* parser_statement(parser_t* self) {
    if (check_value(KEYWORD_PUB)) {
        return parser_public(self);
    }
    else if (check_value(KEYWORD_VAR)) {
        return parser_variable_declaration(self, false);
    }
    else if (check_value(KEYWORD_CONST)) {
        return parser_const_declaration(self, false);
    }
    else if (check_value(KEYWORD_LET)) {
        return parser_local_declaration(self);
    }
    else if (check_value(KEYWORD_IF)) {
        return parser_if_statement(self);
    }
    else if (check_value(KEYWORD_WHILE)) {
        return parser_while_statement(self);
    }
    else if (check_value(KEYWORD_ASYNC)) {
        return parser_async_function(self, false);
    }
    else if (check_value(KEYWORD_FN)) {
        return parser_function(self, false, false);
    }
    else if (check_value("{")) {
        return parser_block(self);
    }
    else if (check_value(KEYWORD_RETURN)) {
        return parser_return_statement(self);
    }
    else {
        position_t *start = self->curr_token->position, *ended = NULL;
        ast_t* expr = parser_expression(self);
        if (expr == NULL) {
            bool is_empty_expression = check_value(";");
            while (check_value(";")) {
                accept_value(";");
            }
            ended = self->prev_token->position;
            return (!is_empty_expression) ? NULL : ast_create(
                AST_EMPTY_STATEMENT, 
                position_merge(start, ended),
                NULL
            );
        }
        accept_value(";");
        return ast_create(
            AST_EXPRESSION_STATEMENT, 
            position_merge(expr->position, self->prev_token->position), 
            ast_expressiont_statement_create(expr)
        );
    }
}

static
ast_t* parser_program(parser_t* self) {
    position_t *start = self->curr_token->position, *ended = NULL;

    ast_t** children = create_ast_array(bsize);
    assert_allocation(children);

    ast_t* child = parser_statement(self);
    while (child != NULL) {
        array_ast_ptr_push(bsize, children, child);
        child = parser_statement(self);
    }

    accept_ttype(TOKEN_EOF);
    ended = self->prev_token->position;

    // test
    // size_t i = 0;
    // while (children[i] != NULL) {
    //     ast_t* last = children[i++];
    //     printf("%s\n", ast_to_string(last));
    // }

    return ast_create(AST_PROGRAM, position_merge(start, ended), ast_program_create(children));
}

ast_t* parser_parse(parser_t* self) {
    self->curr_token = lexer_next_token(self->lexer);
    self->prev_token = self->curr_token;
    void * ast = parser_program(self);
    GC_collect_a_little();
    return ast;
}