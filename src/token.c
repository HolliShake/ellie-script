#include "token.h"

token_t* token_create(token_type_t type, char* value, position_t* position) {
    token_t* token = (token_t*) Ellie_malloc(sizeof(token_t));
    assert_allocation(token);
    token->type = type;
    token->value = value;
    token->position = position;
    return token;
}

char* token_to_string(token_t* self) {
    return str__format("token_t { type: \"%s\", value: \"%s\", position: %s }", TOKEN_XNAME(self->type), self->value, position_to_string(self->position) );
}