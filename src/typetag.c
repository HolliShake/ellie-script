#include "typetag.h"

typetag_t* typetag_create(char* name) {
    typetag_t* typetag = (typetag_t*) Ellie_malloc(sizeof(typetag_t));
    assert_allocation(typetag);
    typetag->name = name;
    typetag->is_nullable = false;

    /**** Members ****/
    typetag->members = (typetag_member_info_t**) Ellie_malloc(sizeof(typetag_member_info_t*));
    assert_allocation(typetag->members);
    typetag->members[0] = NULL;
    typetag->member_count = 0;

    /**** For function ****/
    typetag->argc = 0;
    typetag->is_variadict = false;
    typetag->is_callalble = false;
    typetag->return_type = NULL;
    typetag->param_types = NULL;
    return typetag;
}

typetag_t* typetag_create_function_type(typetag_t** param_types, typetag_t* return_type, int argc, bool is_variadict) {
    char* type_strings = str__new(""), *old;
    for (int i = 0; i < argc; i++) {
        type_strings = str__concat(old = type_strings, typetag_get_name(param_types[i]));
        Ellie_free(old);
        if (i < argc - 1) {
            type_strings = str__concat(old = type_strings, ", ");
            Ellie_free(old);
        }
    }

    char* str = str__format("%s(%s) -> %s", KEYWORD_FN, type_strings, typetag_get_name(return_type));
    Ellie_free(type_strings);
    
    typetag_t* typetag = typetag_create(str);
    typetag->argc = argc;
    typetag->is_variadict = is_variadict;
    typetag->is_callalble = true;
    typetag->return_type = return_type;
    typetag->param_types = param_types;
    return typetag;
}

typetag_t* typetag_clone(typetag_t* typetag) {
    typetag_t* clone = typetag_create(str__new(typetag->name));
    clone->is_nullable = typetag->is_nullable;

    /**** Members ****/
    clone->member_count = typetag->member_count;
    clone->members = typetag->members; // shallow copy

    /**** For function ****/
    clone->argc = typetag->argc;
    clone->is_variadict = typetag->is_variadict;
    clone->is_callalble = typetag->is_callalble;
    clone->return_type = typetag->return_type;
    clone->param_types = typetag->param_types;    
    return clone;
}

char* typetag_get_name(typetag_t* self) {
    return self->name;
}

/*****************************************/

void typetage_add_member(typetag_t* self, char* member, typetag_t* data_type, bool is_getter, bool is_accessible, bool is_mutable) {
    typetag_member_info_t* member_info = (typetag_member_info_t*) Ellie_malloc(sizeof(typetag_member_info_t));
    assert_allocation(member_info);
    member_info->member = member;
    member_info->data_type = data_type;
    member_info->is_getter = is_getter;
    member_info->is_accessible = is_accessible;
    member_info->is_mutable = is_mutable;

    self->members[self->member_count++] = member_info;
    self->members = (typetag_member_info_t**) Ellie_realloc(self->members, sizeof(typetag_member_info_t*) * (self->member_count + 1));
    assert_allocation(self->members);
    self->members[self->member_count] = NULL;
}

bool typetag_has_member(typetag_t* self, char* member) {
    for (size_t i = 0; i < self->member_count; i++) {
        if (str__equals(self->members[i]->member, member))
            return true;
    }
    return false;
}

typetag_member_info_t* typetag_get_member(typetag_t* self, char* member) {
    for (size_t i = 0; i < self->member_count; i++) {
        if (str__equals(self->members[i]->member, member))
            return self->members[i];
    }
    return NULL;
}

/*****************************************/

bool typetag_is_any(typetag_t* self) {
    return str__equals(self->name, "any");
}

bool typetag_is_number(typetag_t* self) {
    return str__equals(self->name, KEYWORD_NUMBER);
}

bool typetag_is_int(typetag_t* self) {
    return str__equals(self->name, KEYWORD_INT);
}

bool typetag_is_string(typetag_t* self) {
    return str__equals(self->name, KEYWORD_STRING);
}
bool typetag_is_bool(typetag_t* self) {
    return str__equals(self->name, KEYWORD_BOOL);
}

bool typetag_is_void(typetag_t* self) {
    return str__equals(self->name, KEYWORD_VOID);
}

bool typetag_is_null(typetag_t* self) {
    return str__equals(self->name, KEYWORD_NULL);
}

bool typetag_is_callable(typetag_t* self) {
    return self->is_callalble;
}

void typetag_to_nullable(typetag_t* self) {
    self->is_nullable = true;
}

bool typetag_can_accept(typetag_t* variable_type, typetag_t* value_type) {
    if (typetag_is_any(variable_type))
        return true;
    else if (typetag_is_number(variable_type) && typetag_is_number(value_type))
        return true;
    else if (typetag_is_int(variable_type) && typetag_is_int(value_type))
        return true;
    else if (typetag_is_string(variable_type) && typetag_is_string(value_type))
        return true;
    else if (typetag_is_bool(variable_type) && typetag_is_bool(value_type))
        return true;
    else if (typetag_is_void(variable_type) && typetag_is_void(value_type))
        return true;
    else if (typetag_is_void(variable_type) && (typetag_is_void(value_type) || typetag_is_null(value_type)))
        return true;
    return false;
}

typetag_t* typetag_equivalent(typetag_t* lhs, typetag_t* rhs) {
    if (str__equals(typetag_get_name(lhs), typetag_get_name(rhs)))
        return typetag_clone(lhs);
    return NULL;
}

typetag_t* typetag_mul(typetag_t* lhs, typetag_t* rhs) {
    if (typetag_is_number(lhs) && typetag_is_number(rhs))
        return TYPETAG_NUMBER;
    else if (typetag_is_int(lhs) && typetag_is_int(rhs))
        return TYPETAG_INT;
    return NULL;
}

typetag_t* typetag_div(typetag_t* lhs, typetag_t* rhs) {
    if (typetag_is_number(lhs) && typetag_is_number(rhs))
        return TYPETAG_NUMBER;
    else if (typetag_is_int(lhs) && typetag_is_int(rhs))
        return TYPETAG_INT;
    return NULL;
}

typetag_t* typetag_mod(typetag_t* lhs, typetag_t* rhs) {
    if (typetag_is_number(lhs) && typetag_is_number(rhs))
        return TYPETAG_NUMBER;
    else if (typetag_is_int(lhs) && typetag_is_int(rhs))
        return TYPETAG_INT;
    return NULL;
}

typetag_t* typetag_add(typetag_t* lhs, typetag_t* rhs) {
    if (typetag_is_string(lhs) && typetag_is_string(rhs))
        return TYPETAG_STRING;
    else if (typetag_is_number(lhs) && typetag_is_number(rhs))
        return TYPETAG_NUMBER;
    else if (typetag_is_int(lhs) && typetag_is_int(rhs))
        return TYPETAG_INT;
    return NULL;
}

typetag_t* typetag_sub(typetag_t* lhs, typetag_t* rhs) {
    if (typetag_is_number(lhs) && typetag_is_number(rhs))
        return TYPETAG_NUMBER;
    else if (typetag_is_int(lhs) && typetag_is_int(rhs))
        return TYPETAG_INT;
    return NULL;
}

typetag_t* typetag_shift(typetag_t* lhs, typetag_t* rhs) {
    if (typetag_is_number(lhs) && typetag_is_number(rhs))
        return TYPETAG_NUMBER;
    else if (typetag_is_int(lhs) && typetag_is_int(rhs))
        return TYPETAG_INT;
    return NULL;
}

typetag_t* typetag_bitwise(typetag_t* lhs, typetag_t* rhs) {
    if (typetag_is_number(lhs) && typetag_is_number(rhs))
        return TYPETAG_NUMBER;
    else if (typetag_is_int(lhs) && typetag_is_int(rhs))
        return TYPETAG_INT;
    return NULL;
}

typetag_t* typetag_compare(typetag_t* lhs, typetag_t* rhs, bool numeric_only)  {
    if ((typetag_is_string(lhs) && typetag_is_string(rhs)) && !numeric_only)
        return TYPETAG_BOOL;
    else if ((typetag_is_number(lhs) && typetag_is_number(rhs)) && numeric_only)
        return TYPETAG_BOOL;
    else if ((typetag_is_int(lhs) && typetag_is_int(rhs)) && numeric_only)
        return TYPETAG_INT;
    return NULL;
}