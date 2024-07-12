#include "lexer.h"

lexer_t* lexer_create(char* file_path_or_name, char* source) {
    lexer_t* lexer = (lexer_t*) Ellie_malloc(sizeof(lexer_t));
    assert_allocation(lexer);
    lexer->file_path_or_name = file_path_or_name;
    lexer->source = source;
    lexer->source_length = str__length(source);
    lexer->lookahead = (str__length(source) <= 0) ? 0 : str__char_code_at(source, 0); 
    lexer->index = 0;
    lexer->current_line = 1;
    lexer->current_colm = 1;
    return lexer;
}

static
void lexer_forward(lexer_t* self) {
    if (self->lookahead == '\n') {
        self->current_line ++;
        self->current_colm = 1;
    } else {
        self->current_colm ++;
    }

    self->index++;
    self->lookahead = (self->index >= self->source_length) ? 0 : str__char_code_at(self->source, self->index); 
}

bool lexer_is_eof(lexer_t* self) {
    return self->index >= self->source_length;
}

static
bool lexer_is_whitespace(lexer_t* self) {
    return utf__isWhiteSpace(self->lookahead);
}

static
bool lexer_is_identifier(lexer_t* self) {
    return utf__isLetter(self->lookahead);
}

static
bool lexer_is_identifier_part(lexer_t* self) {
    return utf__isLetterOrDigit(self->lookahead);
}

static
bool lexer_is_digit(lexer_t* self) {
    return utf__isDigit(self->lookahead);
}

static
bool lexer_is_string(lexer_t* self) {
    return self->lookahead == '\'' || 
           self->lookahead ==  '"' ||
           self->lookahead ==  '`';
}

static
bool lexer_is_hex_digit(lexer_t* self) {
    unsigned char c = self->lookahead;
    return c >= '0' && c <= '9' || c >= 'a' && c <= 'f' || c >= 'A' && c <= 'F';
}

static
bool lexer_is_octal_digit(lexer_t* self) {
    return self->lookahead >= '0' && self->lookahead <= '7';
}

static
bool lexer_is_binary_digit(lexer_t* self) {
    return self->lookahead == '0' || self->lookahead == '1';
}

static 
token_t* lexer_get_identifier(lexer_t* self) {
    char* buffer = str__new(""), *old;
    position_t* initial_pos = position_create(self->current_line, self->current_colm);

    while (!lexer_is_eof(self) && (lexer_is_identifier(self) || (lexer_is_identifier_part(self) && str__length(buffer) > 0))) {
        char* lvalue = str__from_char_code(self->lookahead);
        buffer = str__concat(old = buffer, lvalue);
        Ellie_free(old); Ellie_free(lvalue);
        lexer_forward(self);
    }

    if (str__length(buffer) > MAX_IDENTIFIER_LENGTH) {
        throw_errorf(initial_pos, "identifier \"%s\" is too long", buffer);
    }

    return token_create(is_keyword(buffer) ? TOKEN_KEY : TOKEN_ID, buffer, initial_pos);
}

static 
token_t* lexer_get_number(lexer_t* self) { 
    char* buffer = str__new(""), *old;
    position_t* initial_pos = position_create(self->current_line, self->current_colm);

    // 
    token_type_t type = TOKEN_INT;
    while (!lexer_is_eof(self) && lexer_is_digit(self)) {
        char* lvalue = str__from_char_code(self->lookahead);
        buffer = str__concat(old = buffer, lvalue);
        Ellie_free(old); Ellie_free(lvalue);
        lexer_forward(self);
    }

    if (str__equals(buffer, "0")) {
        char* lvalue;

        switch (self->lookahead) {
            case 'x':
            case 'X':
                lvalue = str__from_char_code(self->lookahead);
                buffer = str__concat(old = buffer, lvalue);
                Ellie_free(old); Ellie_free(lvalue);
                lexer_forward(self);

                if (!lexer_is_hex_digit(self)) {
                    throw_errorf(initial_pos, "expected hex digit, got \"%s\"", str__from_char_code(self->lookahead));
                }

                while (!lexer_is_eof(self) && lexer_is_hex_digit(self)) {
                    lvalue = str__from_char_code(self->lookahead);
                    buffer = str__concat(old = buffer, lvalue);
                    Ellie_free(old); Ellie_free(lvalue);
                    lexer_forward(self);
                }
                break;

            case 'o':
            case 'O':
                lvalue = str__from_char_code(self->lookahead);
                buffer = str__concat(old = buffer, lvalue);
                Ellie_free(old); Ellie_free(lvalue);
                lexer_forward(self);

                if (!lexer_is_octal_digit(self)) {
                    throw_errorf(initial_pos, "expected octal digit, got \"%s\"", str__from_char_code(self->lookahead));
                }

                while (!lexer_is_eof(self) && lexer_is_octal_digit(self)) {
                    lvalue = str__from_char_code(self->lookahead);
                    buffer = str__concat(old = buffer, lvalue);
                    Ellie_free(old); Ellie_free(lvalue);
                    lexer_forward(self);
                }
                break;

            case 'b':
            case 'B':
                lvalue = str__from_char_code(self->lookahead);
                buffer = str__concat(old = buffer, lvalue);
                Ellie_free(old); Ellie_free(lvalue);
                lexer_forward(self);

                if (!lexer_is_binary_digit(self)) {
                    throw_errorf(initial_pos, "expected binary digit, got \"%s\"", str__from_char_code(self->lookahead));
                }

                while (!lexer_is_eof(self) && lexer_is_binary_digit(self)) {
                    lvalue = str__from_char_code(self->lookahead);
                    buffer = str__concat(old = buffer, lvalue);
                    Ellie_free(old); Ellie_free(lvalue);
                    lexer_forward(self);
                }
                break;
            
            default: break;
        }

        if (!str__equals(buffer, "0")) {
            if (self->lookahead == 'n' || self->lookahead == 'N') {
                lvalue = str__from_char_code(self->lookahead);
                buffer = str__concat(old = buffer, lvalue);
                Ellie_free(old); Ellie_free(lvalue);
                lexer_forward(self);
                type = TOKEN_BIGINT;
            }

            token_create(type, buffer, initial_pos);
        }
    }

    if (self->lookahead == '.') {
        char* lvalue = str__from_char_code(self->lookahead);
        buffer = str__concat(old = buffer, lvalue);
        Ellie_free(old); Ellie_free(lvalue);
        lexer_forward(self);

        if (!lexer_is_digit(self)) {
            throw_errorf(initial_pos, "expected digit, got \"%s\"", str__from_char_code(self->lookahead));
        }

        while (!lexer_is_eof(self) && lexer_is_digit(self)) {
            lvalue = str__from_char_code(self->lookahead);
            buffer = str__concat(old = buffer, lvalue);
            Ellie_free(old); Ellie_free(lvalue);
            lexer_forward(self);
        }

        type = TOKEN_FLOAT;
    }

    if (self->lookahead == 'e' || self->lookahead == 'E') {
        char* lvalue = str__from_char_code(self->lookahead);
        buffer = str__concat(old = buffer, lvalue);
        Ellie_free(old); Ellie_free(lvalue);
        lexer_forward(self);

        if (self->lookahead == '+' || self->lookahead == '-') {
            lvalue = str__from_char_code(self->lookahead);
            buffer = str__concat(old = buffer, lvalue);
            Ellie_free(old); Ellie_free(lvalue);
            lexer_forward(self);
        }

        if (!lexer_is_digit(self)) {
            throw_errorf(initial_pos, "expected digit, got \"%s\"", str__from_char_code(self->lookahead));
        }

        while (!lexer_is_eof(self) && lexer_is_digit(self)) {
            lvalue = str__from_char_code(self->lookahead);
            buffer = str__concat(old = buffer, lvalue);
            Ellie_free(old); Ellie_free(lvalue);
            lexer_forward(self);
        }

        type = TOKEN_FLOAT;
    }

    if (type == TOKEN_INT) {
        if (self->lookahead == 'n' || self->lookahead == 'N') {
            char* lvalue = str__from_char_code(self->lookahead);
            buffer = str__concat(old = buffer, lvalue);
            Ellie_free(old); Ellie_free(lvalue);
            lexer_forward(self);
            type = TOKEN_BIGINT;
        }
    }

    // Continue here
    return token_create(type, buffer, initial_pos);
}

static
token_t* lexer_get_string(lexer_t* self) {
    char* buffer = str__new(""), *lvalue, *old;
    position_t* initial_pos = position_create(self->current_line, self->current_colm);

    bool is_multiline = self->lookahead == '`';
    int opener = self->lookahead, closer = 0;

    lvalue = str__from_char_code(self->lookahead);
    buffer = str__concat(old = buffer, lvalue);
    Ellie_free(old); Ellie_free(lvalue);
    lexer_forward(self);

    closer = self->lookahead;

    while (!lexer_is_eof(self) && (opener != closer)) {
        
        if (self->lookahead == '\n' && is_multiline) {
            break;
        }

        if (self->lookahead == '\\') {
            lexer_forward(self);

            switch (self->lookahead)
            {
                case 'b':
                    buffer = str__concat(old = buffer, "\b");
                    Ellie_free(old);
                    break;
                case 'n':
                    buffer = str__concat(old = buffer, "\\n");
                    Ellie_free(old);
                    break;
                case 't':
                    buffer = str__concat(old = buffer, "\\t");
                    Ellie_free(old);
                    break;
                case 'r':
                    buffer = str__concat(old = buffer, "\\r");
                    Ellie_free(old);
                    break;
                case '\'':
                    buffer = str__concat(old = buffer, "\\'");
                    Ellie_free(old);
                    break;
                case '"':
                    buffer = str__concat(old = buffer, "\\\"");
                    Ellie_free(old);
                    break;
                default:
                    throw_errorf(initial_pos, "invalid escape sequence \"%s\"", str__from_char_code(self->lookahead));
            }
        } else {
            lvalue = str__from_char_code(self->lookahead);
            buffer = str__concat(old = buffer, lvalue);
            Ellie_free(old); Ellie_free(lvalue);
        }

        lexer_forward(self);
        closer = self->lookahead;
    }

    if (opener == closer) {
        lvalue = str__from_char_code(self->lookahead);
        buffer = str__concat(old = buffer, lvalue);
        Ellie_free(old); Ellie_free(lvalue);
        lexer_forward(self);
    } else {
        throw_errorf(initial_pos, "expected '\"', got \"%s\"", str__from_char_code(self->lookahead));
    }

    return token_create(TOKEN_STRING, buffer, initial_pos);
}

static
token_t* lexer_get_symbol(lexer_t* self) {
    char* buffer = str__new(""), *lvalue, *old;
    position_t* initial_pos = position_create(self->current_line, self->current_colm);

    switch (self->lookahead) {
        case '(':
        case ')':
        case '[':
        case ']':
        case '{':
        case '}':
        case ',':
        case ':':
        case ';':
            lvalue = str__from_char_code(self->lookahead);
            buffer = str__concat(old = buffer, lvalue);
            Ellie_free(old); Ellie_free(lvalue);
            lexer_forward(self);
            break;
        case '?':
            lvalue = str__from_char_code(self->lookahead);
            buffer = str__concat(old = buffer, lvalue);
            Ellie_free(old); Ellie_free(lvalue);
            lexer_forward(self);

            if (self->lookahead == '?' || self->lookahead == '.') {
                lvalue = str__from_char_code(self->lookahead);
                buffer = str__concat(old = buffer, lvalue);
                Ellie_free(old); Ellie_free(lvalue);
                lexer_forward(self);
            }
            break;
        case '.':
            lvalue = str__from_char_code(self->lookahead);
            buffer = str__concat(old = buffer, lvalue);
            Ellie_free(old); Ellie_free(lvalue);
            lexer_forward(self);

            if (self->lookahead == '.') {
                lvalue = str__from_char_code(self->lookahead);
                buffer = str__concat(old = buffer, lvalue);
                Ellie_free(old); Ellie_free(lvalue);
                lexer_forward(self);
                int copy_look = self->lookahead;
                lvalue = str__from_char_code(self->lookahead);
                buffer = str__concat(old = buffer, lvalue);
                Ellie_free(old); Ellie_free(lvalue);
                lexer_forward(self);

                if (copy_look != '.') {
                    throw_errore(initial_pos, "expected '...', got \"..\"");
                }
            }
            break;
        case '*':
        case '/':
        case '%':
            lvalue = str__from_char_code(self->lookahead);
            buffer = str__concat(old = buffer, lvalue);
            Ellie_free(old); Ellie_free(lvalue);
            lexer_forward(self);

            if (self->lookahead == '=') {
                lvalue = str__from_char_code(self->lookahead);
                buffer = str__concat(old = buffer, lvalue);
                Ellie_free(old); Ellie_free(lvalue);
                lexer_forward(self);
            }
            break;
        case '+':
            lvalue = str__from_char_code(self->lookahead);
            buffer = str__concat(old = buffer, lvalue);
            Ellie_free(old); Ellie_free(lvalue);
            lexer_forward(self);

            if (self->lookahead == '+' || self->lookahead == '=') {
                lvalue = str__from_char_code(self->lookahead);
                buffer = str__concat(old = buffer, lvalue);
                Ellie_free(old); Ellie_free(lvalue);
                lexer_forward(self);
            }
            break;
        case '-':
            lvalue = str__from_char_code(self->lookahead);
            buffer = str__concat(old = buffer, lvalue);
            Ellie_free(old); Ellie_free(lvalue);
            lexer_forward(self);

            if (self->lookahead == '-' || self->lookahead == '=') {
                lvalue = str__from_char_code(self->lookahead);
                buffer = str__concat(old = buffer, lvalue);
                Ellie_free(old); Ellie_free(lvalue);
                lexer_forward(self);
            }
            break;
        case '<':
            lvalue = str__from_char_code(self->lookahead);
            buffer = str__concat(old = buffer, lvalue);
            Ellie_free(old); Ellie_free(lvalue);
            lexer_forward(self);

            if (self->lookahead == '<') {
                lvalue = str__from_char_code(self->lookahead);
                buffer = str__concat(old = buffer, lvalue);
                Ellie_free(old); Ellie_free(lvalue);
                lexer_forward(self);
            }

            if (self->lookahead == '=') {
                lvalue = str__from_char_code(self->lookahead);
                buffer = str__concat(old = buffer, lvalue);
                Ellie_free(old); Ellie_free(lvalue);
                lexer_forward(self);
            }
            break;
        case '>':
            lvalue = str__from_char_code(self->lookahead);
            buffer = str__concat(old = buffer, lvalue);
            Ellie_free(old); Ellie_free(lvalue);
            lexer_forward(self);

            if (self->lookahead == '>') {
                lvalue = str__from_char_code(self->lookahead);
                buffer = str__concat(old = buffer, lvalue);
                Ellie_free(old); Ellie_free(lvalue);
                lexer_forward(self);
            }

            if (self->lookahead == '=') {
                lvalue = str__from_char_code(self->lookahead);
                buffer = str__concat(old = buffer, lvalue);
                Ellie_free(old); Ellie_free(lvalue);
                lexer_forward(self);
            }
            break;
        case '=':
        case '!':
            lvalue = str__from_char_code(self->lookahead);
            buffer = str__concat(old = buffer, lvalue);
            Ellie_free(old); Ellie_free(lvalue);
            lexer_forward(self);

            if (self->lookahead == '=') {
                lvalue = str__from_char_code(self->lookahead);
                buffer = str__concat(old = buffer, lvalue);
                Ellie_free(old); Ellie_free(lvalue);
                lexer_forward(self);
            }
            break;
        case '&':
            lvalue = str__from_char_code(self->lookahead);
            buffer = str__concat(old = buffer, lvalue);
            Ellie_free(old); Ellie_free(lvalue);
            lexer_forward(self);

            if (self->lookahead == '&' || self->lookahead == '=') {
                lvalue = str__from_char_code(self->lookahead);
                buffer = str__concat(old = buffer, lvalue);
                Ellie_free(old); Ellie_free(lvalue);
                lexer_forward(self);
            }
            break;
        case '|':
            lvalue = str__from_char_code(self->lookahead);
            buffer = str__concat(old = buffer, lvalue);
            Ellie_free(old); Ellie_free(lvalue);
            lexer_forward(self);

            if (self->lookahead == '|' || self->lookahead == '=') {
                lvalue = str__from_char_code(self->lookahead);
                buffer = str__concat(old = buffer, lvalue);
                Ellie_free(old); Ellie_free(lvalue);
                lexer_forward(self);
            }
            break;
        case '^':
            lvalue = str__from_char_code(self->lookahead);
            buffer = str__concat(old = buffer, lvalue);
            Ellie_free(old); Ellie_free(lvalue);
            lexer_forward(self);

            if (self->lookahead == '=') {
                lvalue = str__from_char_code(self->lookahead);
                buffer = str__concat(old = buffer, lvalue);
                Ellie_free(old); Ellie_free(lvalue);
                lexer_forward(self);
            }
            break;
        default:
            while (!lexer_is_eof(self) && !utf__isWhiteSpace(self->lookahead) && !lexer_is_identifier(self) && !lexer_is_digit(self) && !lexer_is_string(self)) {
                lvalue = str__from_char_code(self->lookahead);
                buffer = str__concat(old = buffer, lvalue);
                Ellie_free(old); Ellie_free(lvalue);
                lexer_forward(self);
            }
            throw_errorf(initial_pos, "unexpected token \"%s\"", buffer);
            break;
    }

    return token_create(TOKEN_SYMBOL, buffer, initial_pos);
}

static
token_t* lexer_eof(lexer_t* self) {
    char* buffer = str__new("eof");
    position_t* initial_pos = position_create(self->current_line, self->current_colm);
    return token_create(TOKEN_EOF, buffer, initial_pos);
}

void lexer_dump(lexer_t* self) {
    while (!lexer_is_eof(self)) {
        token_t* token = lexer_next_token(self);
        printf("%s\n", token_to_string(token));
    }
}

token_t* lexer_next_token(lexer_t* self) {
    while (!lexer_is_eof(self)) {
        if (lexer_is_whitespace(self)) {
            lexer_forward(self);
        } else if (lexer_is_identifier(self)) {
            return lexer_get_identifier(self);
        } else if (lexer_is_digit(self)) {
            return lexer_get_number(self);
        } else if (lexer_is_string(self)) {
            return lexer_get_string(self);
        } else {
            return lexer_get_symbol(self);
        }
    }
    return lexer_eof(self);
}