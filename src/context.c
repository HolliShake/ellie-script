#include "context.h"

context_t* context_create(context_t* parent, bool is_global, bool is_collection, bool is_awaitable, bool is_function, bool is_iteration) {
    context_t* context = (context_t*) Ellie_malloc(sizeof(context_t));
    assert_allocation(context);
    context->parent = parent;
    context->table = table_create(parent == NULL ? NULL : parent->table);
    context->is_global = is_global;
    context->is_local = !is_global;
    context->is_single = false;
    context->is_conditional = false;
    context->is_collection = is_collection;
    context->is_awaitable = is_awaitable;
    context->is_function = is_function;
    context->is_iteration = is_iteration;
    /**** Function prop ****/
    context->return_type = NULL;
    context->returned_a_value = false;
    /**** Default Types ****/
    void* any_t = NULL;
    void* number_t = NULL;
    void* int_t = NULL;
    void* string_t = NULL;
    void* bool_t = NULL;
    void* void_t = NULL;
    void* null_t = NULL;
    void* array_template_t = NULL;
    return context;
}

bool context_is_global(context_t* self) {
    return self->is_global;
}
bool context_is_collection(context_t* self) {
    return self->is_collection;
}

bool context_is_awaitable(context_t* self) {
    return self->is_function && self->is_awaitable;
}
bool context_is_function(context_t* self) {
    return self->is_function;
}

bool context_is_iteration(context_t* self) {
    return self->is_iteration;
}

bool context_is_local(context_t* self) {
    return self->is_local;
}

bool context_is_single(context_t* self) {
    return self->is_single;
}

bool context_is_conditional(context_t* self) {
    return self->is_conditional;
}

void context_to_single(context_t* self) {
    self->is_local = false;
    self->is_single = true;
}

void context_to_conditional(context_t* self) {
    self->is_conditional = true;
}

void context_initialize_default_types(context_t* context) {
    context->any_t = TYPETAG_ANY;
    context->number_t = TYPETAG_NUMBER;
    context->int_t = TYPETAG_INT;
    context->string_t = TYPETAG_STRING;
    context->bool_t = TYPETAG_BOOL;
    context->void_t = TYPETAG_VOID;
    context->null_t = TYPETAG_NULL;
    context->array_template_t = TYPETAG_ARRAY(context->any_t);
}

typetag_t* context_get_default_any_t(context_t* self) {
    if (self->any_t == NULL) {
        fprintf(stderr, "%s::%s: error any_t is not initialized.\n", __FILE__, __func__);
        exit(1);
    }
    return self->any_t;
}

typetag_t* context_get_default_number_t(context_t* self) {
    if (self->number_t == NULL) {
        fprintf(stderr, "%s::%s: error number_t is not initialized.\n", __FILE__, __func__);
        exit(1);
    }
    return self->number_t;
}

typetag_t* context_get_default_int_t(context_t* self) {
    if (self->int_t == NULL) {
        fprintf(stderr, "%s::%s: error int_t is not initialized.\n", __FILE__, __func__);
        exit(1);
    }
    return self->int_t;
}

typetag_t* context_get_default_string_t(context_t* self) {
    if (self->string_t == NULL) {
        fprintf(stderr, "%s::%s: error string_t is not initialized.\n", __FILE__, __func__);
        exit(1);
    }
    return self->string_t;
}

typetag_t* context_get_default_bool_t(context_t* self) {
    if (self->bool_t == NULL) {
        fprintf(stderr, "%s::%s: error bool_t is not initialized.\n", __FILE__, __func__);
        exit(1);
    }
    return self->bool_t;
}

typetag_t* context_get_default_void_t(context_t* self) {
    if (self->void_t == NULL) {
        fprintf(stderr, "%s::%s: error void_t is not initialized.\n", __FILE__, __func__);
        exit(1);
    }
    return self->void_t;
}

typetag_t* context_get_default_array_template_t(context_t* self) {
    if (self->array_template_t == NULL) {
        fprintf(stderr, "%s::%s: error array_template_t is not initialized.\n", __FILE__, __func__);
        exit(1);
    }
    return self->array_template_t;
}

typetag_t* context_get_default_null_t(context_t* self) {
    if (self->null_t == NULL) {
        fprintf(stderr, "%s::%s: error null_t is not initialized.\n", __FILE__, __func__);
        exit(1);
    }
    return self->null_t;
}

void context_bind_function_return(context_t* self, char* function_name, typetag_t* return_type) {
    self->function_name = function_name;
    self->return_type = return_type;
}