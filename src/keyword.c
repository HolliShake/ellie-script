#include "keyword.h"


#define KEYWORD_SIZE 22
const char* KEYWORD[] = {
    KEYWORD_IMPORT,
    KEYWORD_FROM,
    KEYWORD_PUB,
    KEYWORD_CLASS,
    KEYWORD_ENUM,
    KEYWORD_ASYNC,
    KEYWORD_FN,
    KEYWORD_VAR,
    KEYWORD_CONST,
    KEYWORD_LET,
    // typing
    KEYWORD_NUMBER,
    KEYWORD_INT,
    KEYWORD_STRING,
    KEYWORD_BOOL,
    KEYWORD_VOID,
    KEYWORD_OBJECT,
    KEYWORD_ARRAY
    // 
    KEYWORD_IF,
    KEYWORD_ELSE,
    // 
    KEYWORD_RETURN,
    // 
    KEYWORD_TRUE,
    KEYWORD_FALSE,
    KEYWORD_NULL
};

bool is_keyword(char* str) {
    for (int i = 0; i < KEYWORD_SIZE; i++) {
        if (str__equals(str, (char*) KEYWORD[i])) {
            return true;
        }
    }
    return false;
}