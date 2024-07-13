#include "jslink.h"

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
    if (object == NULL) {
        fprintf(stderr, "%s::%s: error object %s not found.\n", __FILE__, __func__, object_name);
        exit(1);
    }
    if (!typetag_is_callable(method_type)) {
        fprintf(stderr, "%s::%s: error method %s is not callable.\n", __FILE__, __func__, method_name);
        exit(1);
    }
    typetage_add_member(object->data_type, method_name, method_type, false, true, false);
}


static typetag_t** to_ptr_array(typetag_t** types, size_t size) {
    typetag_t** ptr_array = (typetag_t**) Ellie_malloc((size + 1) * sizeof(typetag_t*));
    assert_allocation(ptr_array);
    for (size_t i = 0; i < size; i++)
        ptr_array[i] = types[i];
    ptr_array[size] = NULL;
    return ptr_array;
}

void js_link_init(context_t* context) {
    /**** js console object *******/
    js_link_define_type_object(context, "console", typetag_create("JsConsole"));
        js_link_define_method_to_object_name(context, "console", "log", typetag_create_function_type(
            to_ptr_array((typetag_t*[]) { TYPETAG_ANY, NULL }, 1),
            TYPETAG_VOID,
            /********/ 1,
            /*****/ true,
            /*****/ false
        ));
        js_link_define_method_to_object_name(context, "console", "warn", typetag_create_function_type(
            to_ptr_array((typetag_t*[]) { TYPETAG_ANY, NULL }, 1),
            TYPETAG_VOID,
            /********/ 1,
            /*****/ true,
            /*****/ false
        ));
        js_link_define_method_to_object_name(context, "console", "error", typetag_create_function_type(
            to_ptr_array((typetag_t*[]) { TYPETAG_ANY, NULL }, 1),
            TYPETAG_VOID,
            /********/ 1,
            /*****/ true,
            /*****/ false
        ));
        js_link_define_method_to_object_name(context, "console", "scan", typetag_create_function_type(
            to_ptr_array((typetag_t*[]) { TYPETAG_ANY, NULL }, 1),
            TYPETAG_STRING,
            /********/ 1,
            /****/ false,
            /****/ false
        ));

    // other
}