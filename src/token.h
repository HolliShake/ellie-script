#include "global.h"
#include "position.h"

#ifndef TOKEN_H
#define TOKEN_H
    typedef enum token_type {
        TOKEN_ID,
        TOKEN_KEY,
        TOKEN_INT,
        TOKEN_BIGINT,
        TOKEN_FLOAT,
        TOKEN_STRING,
        TOKEN_SYMBOL,
        TOKEN_EOF
    } token_type_t;

    #define TOKEN_NAME(token_type) #token_type
    #define TOKEN_XNAME(token_type)  (token_type == TOKEN_ID    ) \
        ? TOKEN_NAME(TOKEN_ID    ) : (token_type == TOKEN_KEY   ) \
        ? TOKEN_NAME(TOKEN_KEY   ) : (token_type == TOKEN_INT   ) \
        ? TOKEN_NAME(TOKEN_INT   ) : (token_type == TOKEN_BIGINT) \
        ? TOKEN_NAME(TOKEN_BIGINT) : (token_type == TOKEN_FLOAT ) \
        ? TOKEN_NAME(TOKEN_FLOAT ) : (token_type == TOKEN_STRING) \
        ? TOKEN_NAME(TOKEN_STRING) : (token_type == TOKEN_SYMBOL) \
        ? TOKEN_NAME(TOKEN_SYMBOL) : (token_type == TOKEN_EOF   ) \
        ? TOKEN_NAME(TOKEN_EOF   ) : "UNKNOWN_TOKEN_TYPE"         \

    typedef struct token_struct {
        token_type_t type;
        char* value;
        position_t* position;
    } token_t;

    token_t* token_create(token_type_t type, char* value, position_t* position);
    char* token_to_string(token_t* token);
#endif