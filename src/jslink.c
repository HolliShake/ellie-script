/**
 * Defines public attribute or member 
 */
#include "jslink.h"

#define ASSERT_OBJECT_NOT_NULL(object_name) \
    if (object == NULL) { \
        fprintf(stderr, "%s::%s: error object %s not found.\n", __FILE__, __func__, object_name); \
        exit(1); \
    }\

#define ASSERT_IS_CALLABLE(method_type) \
    if (!typetag_is_callable(method_type)) { \
        fprintf(stderr, "%s::%s: error method %s is not callable.\n", __FILE__, __func__, method_name); \
        exit(1); \
    }\

#define ASSERT_MEMBER_NOT_DEFINED(object, method_name) \
    if (typetag_has_member(object, method_name)) { \
        fprintf(stderr, "%s::%s: error meember %s already exists.\n", __FILE__, __func__, method_name); \
        exit(1); \
    }\

void js_link_define_type_object(context_t* context, char* object_name, typetag_t* object_type) {
    table_insert(context->table, symbol_info_create_variable(
        object_name, 
        object_type, 
        context_is_global(context), 
        true
    ));
}

void js_link_define_method_to_object_name(context_t* context, char* object_name, char* method_name, typetag_t* method_type) {
    symbol_info_t* object = table_lookup(context->table, object_name);
    ASSERT_OBJECT_NOT_NULL(object);
    ASSERT_IS_CALLABLE(method_type);
    ASSERT_MEMBER_NOT_DEFINED(object->data_type, method_name);
    typetag_add_member(object->data_type, method_name, method_type, 
        /* is_getter     = */ false, 
        /* is_accessible = */ true , 
        /* is_mutable    = */ false, 
        /* is_static     = */ false
    );
}

void js_link_define_static_method_to_object_name(context_t* context, char* object_name, char* method_name, typetag_t* method_type) {
    symbol_info_t* object = table_lookup(context->table, object_name);
    ASSERT_OBJECT_NOT_NULL(object);
    ASSERT_IS_CALLABLE(method_type);
    ASSERT_MEMBER_NOT_DEFINED(object->data_type, method_name);
    typetag_add_member(
        object->data_type, method_name, method_type, 
        /* is_getter     = */ false, 
        /* is_accessible = */ true , 
        /* is_mutable    = */ false, 
        /* is_static     = */ true
    );
}

void js_link_define_method_to_object(context_t* context, typetag_t* object, char* method_name, typetag_t* method_type) {
    ASSERT_IS_CALLABLE(method_type);
    ASSERT_MEMBER_NOT_DEFINED(object, method_name);
    typetag_add_member(object, method_name, method_type, 
        /* is_getter     = */ false, 
        /* is_accessible = */ true , 
        /* is_mutable    = */ false, 
        /* is_static     = */ false
    );
}

void js_link_define_static_method_to_object(context_t* context, typetag_t* object, char* method_name, typetag_t* method_type) {
    ASSERT_IS_CALLABLE(method_type);
    ASSERT_MEMBER_NOT_DEFINED(object, method_name);
    typetag_add_member(
        object, method_name, method_type, 
        /* is_getter     = */ false, 
        /* is_accessible = */ true , 
        /* is_mutable    = */ false, 
        /* is_static     = */ true
    );
}

typetag_t** to_ptr_array(typetag_t** types, size_t size) {
    typetag_t** ptr_array = (typetag_t**) Ellie_malloc((size + 1) * sizeof(typetag_t*));
    assert_allocation(ptr_array);
    for (size_t i = 0; i < size; i++)
        ptr_array[i] = types[i];
    ptr_array[size] = NULL;
    return ptr_array;
}