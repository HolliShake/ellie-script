#include "global.h"
#include "keyword.h"

#ifndef TYPETAG_H
#define TYPETAG_H
    #define TYPETAG_KEYWORD_OBJECT "Object"
    #define TYPETAG_KEYWORD_ARRAY "Array"
    #define TYPETAG_OBJECT(k,v) typetag_create_typed_object(k, v)
    #define TYPETAG_ARRAY(e) typetag_create_typed_array(e)
    #define TYPETAG_ARRAY_FROM_TEMPLATE(t, e) typetag_create_typed_array_from_template(t, e)
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
        bool is_array;
        bool is_object;
        bool is_type;
        bool is_array_init;
        bool is_object_init;
        /**** Child ******/
        typetag_t* inner_0;
        typetag_t* inner_1;
        /**** Members ****/
        size_t member_count;
        typetag_member_info_t** members;
        /**** For function types ****/
        int argc; // supported argument count or number of parameters
        bool is_variadict;
        bool is_callalble;
        bool is_asynchronous;
        typetag_t* return_type;
        typetag_t** param_types;
    } typetag_t;

    typedef struct typetag_member_info_struct {
        char* member;
        typetag_t* data_type;
        bool is_getter;
        bool is_accessible;
        bool is_mutable;
        bool is_static;
    } typetag_member_info_t;

    typetag_member_info_t* typetag_member_info_clone(typetag_member_info_t* self);

    typetag_t* typetag_create(char* name);
    typetag_t* typetag_create_typed_object(typetag_t* key, typetag_t* val);
    typetag_t* typetag_create_typed_array(typetag_t* element_type);
    typetag_t* typetag_create_function_type(typetag_t** param_types, typetag_t* return_type, int argc, bool is_variadict, bool is_asynchronous);
    typetag_t* typetag_clone(typetag_t* typetag);
    char* typetag_get_name(typetag_t* self);
    char* typetag_get_default_value_string(typetag_t* self);
    void typetag_to_type(typetag_t* self);
    // 
    void typetag_add_member(typetag_t* self, char* member, typetag_t* data_type, bool is_getter, bool is_accessible, bool is_mutable, bool is_static);
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
    bool typetag_is_nullable(typetag_t* self);
    bool typetag_is_callable(typetag_t* self);
    bool typetag_is_object(typetag_t* self);
    bool typetag_is_array(typetag_t* self);
    bool typetag_is_equal(typetag_t* lhs, typetag_t* rhs);
    void typetag_to_nullable(typetag_t* self);
    bool typetag_can_accept(typetag_t* variable_type, typetag_t* value_type);

    /**********************/
    #ifndef CONTEXT_H
        typedef struct context_struct context_t;
    #endif

    extern typetag_t* context_get_default_any_t(context_t* self);
    extern typetag_t* context_get_default_number_t(context_t* self);
    extern typetag_t* context_get_default_int_t(context_t* self);
    extern typetag_t* context_get_default_string_t(context_t* self);
    extern typetag_t* context_get_default_bool_t(context_t* self);
    extern typetag_t* context_get_default_void_t(context_t* self);
    extern typetag_t* context_get_default_null_t(context_t* self);

    typetag_t* typetag_equivalent(context_t* global, typetag_t* lhs, typetag_t* rhs);
    typetag_t* typetag_bitwise_not(context_t* global, typetag_t* typetag);
    typetag_t* typetag_not(context_t* global, typetag_t* typetag);
    typetag_t* typetag_plus_minus(context_t* global, typetag_t* typetag);
    typetag_t* typetag_incdec(context_t* global, typetag_t* typetag);
    typetag_t* typetag_mul(context_t* global, typetag_t* lhs, typetag_t* rhs);
    typetag_t* typetag_div(context_t* global, typetag_t* lhs, typetag_t* rhs);
    typetag_t* typetag_mod(context_t* global, typetag_t* lhs, typetag_t* rhs);
    typetag_t* typetag_add(context_t* global, typetag_t* left, typetag_t* right);
    typetag_t* typetag_sub(context_t* global, typetag_t* lhs, typetag_t* rhs);
    typetag_t* typetag_shift(context_t* global, typetag_t* lhs, typetag_t* rhs);
    typetag_t* typetag_bitwise(context_t* global, typetag_t* lhs, typetag_t* rhs);
    typetag_t* typetag_logical(context_t* global, typetag_t* lhs, typetag_t* rhs);
    typetag_t* typetag_compare(context_t* global, typetag_t* bool_type, typetag_t* lhs, typetag_t* rhs, bool numeric_only);
#endif