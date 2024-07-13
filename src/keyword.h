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
    #define KEYWORD_NUMBER "Number"
    #define KEYWORD_INT "Int"
    #define KEYWORD_STRING "String"
    #define KEYWORD_BOOL "Bool"
    #define KEYWORD_VOID "Void"
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

