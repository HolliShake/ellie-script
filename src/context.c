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

void context_bind_function_return(context_t* self, char* function_name, typetag_t* return_type) {
    self->function_name = function_name;
    self->return_type = return_type;
}