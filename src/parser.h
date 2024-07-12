#include "global.h"
#include "keyword.h"
#include "lexer.h"
#include "error.h"
#include "ast.h"

#ifndef PARSER_H
#define PARSER_H

    #ifdef FINFO
        #undef FILE_PATH
        #undef FILE_DATA
        #define FILE_PATH self->lexer->file_path_or_name
        #define FILE_DATA self->lexer->source
    #else
        #define FINFO
        #define FILE_PATH self->lexer->file_path_or_name
        #define FILE_DATA self->lexer->source
    #endif

    typedef struct parser_struct {
        lexer_t* lexer;
        token_t* curr_token;
        token_t* prev_token;
    } parser_t;

    parser_t* parser_create(char* file_path_or_name, char* source);
    ast_t* parser_parse(parser_t* self);

#endif