#include "jsarray.h"


void js_array_init(context_t* context, typetag_t* array_type) {
    typetag_t* any_t = context_get_default_any_t(context);
    typetag_t* number_t = context_get_default_number_t(context);
    typetag_t* int_t = context_get_default_int_t(context);
    typetag_t* string_t = context_get_default_string_t(context);
    typetag_t* bool_t = context_get_default_bool_t(context);
    typetag_t* void_t = context_get_default_void_t(context);
    typetag_t* null_t = context_get_default_null_t(context);
    /*******************************************************/
    if (!typetag_is_array(array_type)) {
        fprintf(stderr, "%s::%s: error array_type is not an array.\n", __FILE__, __func__);
        exit(1);
    }

    if (array_type->is_array_init) {
        return;
    }

    array_type->is_array_init = true;

    // Array.push
    js_link_define_method_to_object(context, array_type, "push", typetag_create_function_type(
        to_ptr_array((typetag_t*[]) { array_type->inner_0, NULL }, 1),
        array_type->inner_0,
        /*********/ 1,
        /*****/ false,
        /*****/ false
    ));

    // Array.pop
    js_link_define_method_to_object(context, array_type, "pop", typetag_create_function_type(
        to_ptr_array((typetag_t*[]) { NULL }, 0),
        array_type->inner_0,
        /*********/ 0,
        /*****/ false,
        /*****/ false
    ));

    // Array.shift
    js_link_define_method_to_object(context, array_type, "shift", typetag_create_function_type(
        to_ptr_array((typetag_t*[]) { NULL }, 0),
        array_type->inner_0,
        /*********/ 0,
        /*****/ false,
        /*****/ false
    ));

    // Array.unshift
    js_link_define_method_to_object(context, array_type, "unshift", typetag_create_function_type(
        to_ptr_array((typetag_t*[]) { array_type->inner_0, NULL }, 1),
        array_type->inner_0,
        /********/ 1,
        /*****/ true,
        /*****/ false
    ));
}