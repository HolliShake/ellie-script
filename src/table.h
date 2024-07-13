#include "global.h"
#include "typetag.h"

#ifndef TABLE_H
#define TABLE_H
    typedef struct table_struct table_t;
    typedef struct symbol_info_struct symbol_info_t;

    typedef struct table_struct {
        table_t* parent;
        size_t content;
        symbol_info_t** symbol_info;
    } table_t;

    table_t* table_create(table_t* parent);
    table_t* table_create_no_parent();
    void table_insert(table_t* self, symbol_info_t* symbol_info);
    bool table_exists(table_t* self, char* name);
    bool table_defined(table_t* self, char* name);
    symbol_info_t* table_lookup(table_t* self, char* name);

    typedef struct symbol_info_struct {
        char* name;
        typetag_t* data_type;
        /**** Default prop ****/ 
        bool is_global;
        bool is_constant;
        bool is_exported; // optional

        /**** For function ****/
        int argc; // supported argument count or number of parameters
        char** param_names;
        typetag_t** param_types;
        typetag_t* return_type;
        bool is_asynchronous;
    } symbol_info_t;

    symbol_info_t* symbol_info_create_variable(char* name, typetag_t* data_type, bool is_global, bool is_constant);
    symbol_info_t* symbol_info_create_function(char* name, typetag_t* data_type, int argc, char** parameter_names, typetag_t** parameter_types, typetag_t* return_type, bool is_asynchronous);
    void symbol_info_mark_as_exported(symbol_info_t* self);
#endif
