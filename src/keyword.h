#include "global.h"

#ifndef KEYWORD_H
#define KEYWORD_H

    #define KEYWORD_IMPORT "import"
    #define KEYWORD_FROM "from"
    #define KEYWORD_PUB "pub"
    #define KEYWORD_CLASS "class"
    #define KEYWORD_ENUM "enum"
    #define KEYWORD_ASYNC "async"
    #define KEYWORD_FN "fn"
    #define KEYWORD_VAR "var"
    #define KEYWORD_CONST "const"
    #define KEYWORD_LET "let"
    // 
    #define KEYWORD_IF "if"
    #define KEYWORD_ELSE "else"
    #define KEYWORD_WHILE "while"
    // 
    #define KEYWORD_NUMBER "number"
    #define KEYWORD_INT "int"
    #define KEYWORD_STRING "string"
    #define KEYWORD_BOOL "Bbool"
    #define KEYWORD_VOID "void"
    #define KEYWORD_OBJECT "Object"
    #define KEYWORD_ARRAY "Array"
    // 
    #define KEYWORD_RETURN "return"
    // 
    #define KEYWORD_TRUE "true"
    #define KEYWORD_FALSE "false"
    #define KEYWORD_NULL "null"

    extern const char* KEYWORD[];
    bool is_keyword(char* str);

#endif

