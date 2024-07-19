#include "jsnumber.h"


void js_number_init(context_t *context) {
    typetag_t* any_t = context_get_default_any_t(context);
    typetag_t* number_t = context_get_default_number_t(context);
    typetag_t* int_t = context_get_default_int_t(context);
    typetag_t* string_t = context_get_default_string_t(context);
    typetag_t* bool_t = context_get_default_bool_t(context);
    typetag_t* void_t = context_get_default_void_t(context);
    typetag_t* null_t = context_get_default_null_t(context);
    /*******************************************************/

    /**** js console object *******/
    js_link_define_type_object(context, "Number", number_t);
}