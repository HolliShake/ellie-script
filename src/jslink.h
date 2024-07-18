#include "global.h"
#include "context.h"
#include "table.h"
#include "typetag.h"

#ifndef JS_H
#define JS_H
    void js_link_define_type_object(context_t* context, char* object_name, typetag_t* object_type);
    void js_link_define_method_to_object_name(context_t* context, char* object_name, char* method_name, typetag_t* method_type);
    void js_link_define_static_method_to_object_name(context_t* context, char* object_name, char* method_name, typetag_t* method_type);
    void js_link_define_method_to_object(context_t* context, typetag_t* object, char* method_name, typetag_t* method_type);
    void js_link_define_static_method_to_object(context_t* context, typetag_t* object, char* method_name, typetag_t* method_type);
    extern typetag_t** to_ptr_array(typetag_t** types, size_t size);
#endif