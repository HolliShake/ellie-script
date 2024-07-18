#include "global.h"
#include "error.h"
#include "parser.h"
#include "context.h"
#include "typetag.h"
#include "jslink.h"
#include "jsconsole.h"
#include "jsarray.h"

#ifndef GENERATOR_H
#define GENERATOR_H

    #ifdef FINFO
        #undef FILE_PATH
        #undef FILE_DATA
        #define FILE_PATH self->parser->lexer->file_path_or_name
        #define FILE_DATA self->parser->lexer->source
    #else
        #define FINFO
        #define FILE_PATH self->parser->lexer->file_path_or_name
        #define FILE_DATA self->parser->lexer->source
    #endif

    typedef struct generator_struct {
        parser_t* parser;
        context_t* global;
        char* js_code;
        size_t nesting_level;
    } generator_t;

    generator_t* generator_create(char* file_path_or_name, char* source);
    void* generator_generate(generator_t* self);
#endif