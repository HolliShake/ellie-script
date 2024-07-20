#include "global.h"
#include "table.h"

#ifndef CONTEXT_H
#define CONTEXT_H
    #define CONTEXT_GLOBAL() context_create(NULL, true, false, false, false, false)
    #define CONTEXT_COLLECTION(parent_context) context_create(parent_context, false, true, false, false, false)
    #define CONTEXT_AWAITABLE_FUNCTION(parent_context) context_create(parent_context, false, false, true, true, false)
    #define CONTEXT_FUNCTION(parent_context) context_create(parent_context, false, false, false, true, false)
    #define CONTEXT_ITERATION(parent_context) context_create(parent_context, false, false, false, false, true)
    #define CONTEXT_LOCAL(parent_context) context_create(parent_context, false, false, false, false, false)
    
    typedef struct context_struct context_t;
    typedef struct context_struct {
        context_t* parent;
        table_t* table;
        bool is_global;
        // Begin Optional 
        bool is_local;
        bool is_single;
        bool is_conditional;
        // End Optional
        bool is_collection;
        bool is_awaitable; 
        bool is_function;
        bool is_iteration;
        /**** Function prop ****/
        char* function_name;
        typetag_t* return_type;
        bool returned_a_value;
        /**** Array[T] prop ****/
        typetag_t* array_type;
        /**** Default Types ****/
        typetag_t* any_t;
        typetag_t* number_t;
        typetag_t* int_t;
        typetag_t* string_t;
        typetag_t* bool_t;
        typetag_t* void_t;
        typetag_t* null_t;
    } context_t;

    context_t* context_create(context_t* parent, bool is_global, bool is_collection, bool is_awaitable, bool is_function, bool is_iteration);
    bool context_is_global(context_t* self);
    bool context_is_collection(context_t* self);
    bool context_is_awaitable(context_t* self);
    bool context_is_function(context_t* self);
    bool context_is_iteration(context_t* self);
    bool context_is_local(context_t* self);
    bool context_is_single(context_t* self);
    bool context_is_conditional(context_t* self);
    void context_to_single(context_t* self);
    void context_to_conditional(context_t* self);
    void context_initialize_default_types(context_t* context);
    typetag_t* context_get_default_any_t(context_t* self);
    typetag_t* context_get_default_number_t(context_t* self);
    typetag_t* context_get_default_int_t(context_t* self);
    typetag_t* context_get_default_string_t(context_t* self);
    typetag_t* context_get_default_bool_t(context_t* self);
    typetag_t* context_get_default_void_t(context_t* self);
    typetag_t* context_get_default_null_t(context_t* self);
    void context_bind_function_return(context_t* self, char* function_name, typetag_t* return_type);
    void context_bind_array_type(context_t* self, typetag_t* array_type);
#endif