#include "global.h"
#include "keyword.h"

#ifndef TYPETAG_H
#define TYPETAG_H
    #define TYPETAG_ANY typetag_create(str__new("any"))
    #define TYPETAG_NUMBER typetag_create(str__new(KEYWORD_NUMBER))
    #define TYPETAG_INT typetag_create(str__new(KEYWORD_INT))
    #define TYPETAG_STRING typetag_create(str__new(KEYWORD_STRING))
    #define TYPETAG_BOOL typetag_create(str__new(KEYWORD_BOOL))
    #define TYPETAG_VOID typetag_create(str__new(KEYWORD_VOID))
    #define TYPETAG_NULL typetag_create(str__new(KEYWORD_NULL))

    typedef struct type_tag_struct typetag_t;
    typedef struct typetag_member_info_struct typetag_member_info_t;

    typedef struct type_tag_struct {
        char* name;
        bool is_nullable;
        /**** Members ****/
        size_t member_count;
        typetag_member_info_t** members;

        /**** For function types ****/
        int argc; // supported argument count or number of parameters
        bool is_variadict;
        bool is_callalble;
        typetag_t* return_type;
        typetag_t** param_types;
    } typetag_t;

    typedef struct typetag_member_info_struct {
        char* member;
        typetag_t* data_type;
        bool is_getter;
        bool is_accessible;
        bool is_mutable;
    } typetag_member_info_t;

    typetag_t* typetag_create(char* name);
    typetag_t* typetag_create_function_type(typetag_t** param_types, typetag_t* return_type, int argc, bool is_variadict);
    typetag_t* typetag_clone(typetag_t* typetag);
    char* typetag_get_name(typetag_t* self);
    // 
    void typetage_add_member(typetag_t* self, char* member, typetag_t* data_type, bool is_getter, bool is_accessible, bool is_mutable);
    bool typetag_has_member(typetag_t* self, char* member);
    typetag_member_info_t* typetag_get_member(typetag_t* self, char* member);
    // 
    bool typetag_is_any(typetag_t* self);
    bool typetag_is_number(typetag_t* self);
    bool typetag_is_int(typetag_t* self);
    bool typetag_is_string(typetag_t* self);
    bool typetag_is_bool(typetag_t* self);
    bool typetag_is_void(typetag_t* self);
    bool typetag_is_null(typetag_t* self);
    bool typetag_is_callable(typetag_t* self);
    void typetag_to_nullable(typetag_t* self);
    bool typetag_can_accept(typetag_t* variable_type, typetag_t* value_type);
    typetag_t* typetag_equivalent(typetag_t* lhs, typetag_t* rhs);
    typetag_t* typetag_mul(typetag_t* lhs, typetag_t* rhs);
    typetag_t* typetag_div(typetag_t* lhs, typetag_t* rhs);
    typetag_t* typetag_mod(typetag_t* lhs, typetag_t* rhs);
    typetag_t* typetag_add(typetag_t* left, typetag_t* right);
    typetag_t* typetag_sub(typetag_t* lhs, typetag_t* rhs);
    typetag_t* typetag_shift(typetag_t* lhs, typetag_t* rhs);
    typetag_t* typetag_bitwise(typetag_t* lhs, typetag_t* rhs);
    typetag_t* typetag_compare(typetag_t* lhs, typetag_t* rhs, bool numeric_only);
#endif