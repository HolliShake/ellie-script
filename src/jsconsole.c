#include "jsconsole.h"

void js_console_init(context_t* context) {
    typetag_t* any_t = context_get_default_any_t(context);
    typetag_t* number_t = context_get_default_number_t(context);
    typetag_t* int_t = context_get_default_int_t(context);
    typetag_t* string_t = context_get_default_string_t(context);
    typetag_t* bool_t = context_get_default_bool_t(context);
    typetag_t* void_t = context_get_default_void_t(context);
    typetag_t* null_t = context_get_default_null_t(context);
    /*******************************************************/

    /**** js console object *******/
    js_link_define_type_object(context, "console", TYPETAG_OBJECT(string_t, any_t));
        // console.log
        js_link_define_static_method_to_object_name(context, "console", "log", typetag_create_function_type(
            to_ptr_array((typetag_t*[]) { any_t, NULL }, 1),
            void_t,
            /********/ 1,
            /*****/ true,
            /*****/ false
        ));
        // console.warn
        js_link_define_static_method_to_object_name(context, "console", "warn", typetag_create_function_type(
            to_ptr_array((typetag_t*[]) { any_t, NULL }, 1),
            void_t,
            /********/ 1,
            /*****/ true,
            /*****/ false
        ));
        // console.error
        js_link_define_static_method_to_object_name(context, "console", "error", typetag_create_function_type(
            to_ptr_array((typetag_t*[]) { any_t, NULL }, 1),
            void_t,
            /********/ 1,
            /*****/ true,
            /*****/ false
        ));
        // console.scan
        js_link_define_static_method_to_object_name(context, "console", "scan", typetag_create_function_type(
            to_ptr_array((typetag_t*[]) { any_t, NULL }, 1),
            string_t,
            /********/ 1,
            /****/ false,
            /****/ false
        ));
}