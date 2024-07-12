#include "global.h"
#include "context.h"
#include "table.h"
#include "typetag.h"

#ifndef JS_H
#define JS_H
    void js_link_define_type_object(context_t* context, char* object_name, typetag_t* object_type);
    void js_link_define_method_to_object_name(context_t* context, char* object_name, char* method_name, typetag_t* method_type);
    void js_link_init(context_t* context);
#endif