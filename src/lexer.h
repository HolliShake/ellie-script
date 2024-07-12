#include "global.h"
#include "keyword.h"
#include "token.h"
#include "position.h"
#include "error.h"

#ifndef ELLIE_LEXER_H
#define ELLIE_LEXER_H

    #ifdef FINFO
        #undef FILE_PATH
        #undef FILE_DATA
        #define FILE_PATH self->file_path_or_name
        #define FILE_DATA self->source
    #else
        #define FINFO
        #define FILE_PATH self->file_path_or_name
        #define FILE_DATA self->source
    #endif

    typedef struct lexer_struct {
        char* file_path_or_name;
        char* source;
        int lookahead;
        size_t source_length;
        size_t index;
        size_t current_line;
        size_t current_colm;
    } lexer_t;

    lexer_t* lexer_create(char* file_path_or_name, char* source);
    bool lexer_is_eof(lexer_t* self);
    void lexer_dump(lexer_t* self);
    token_t* lexer_next_token(lexer_t* self);
#endif
