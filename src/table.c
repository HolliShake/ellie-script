#include "table.h"


table_t* table_create(table_t* parent) {
    table_t* table = (table_t*) Ellie_malloc(sizeof(table_t));
    assert_allocation(table);
    table->parent = parent;
    table->symbol_info = (symbol_info_t**) Ellie_malloc(sizeof(symbol_info_t*));
    assert_allocation(table->symbol_info);
    return table;
}

table_t* table_create_no_parent() {
    return table_create(NULL);
}

void table_insert(table_t* self, symbol_info_t* symbol_info) {
    self->symbol_info[self->content] = symbol_info;
    self->content++;
    self->symbol_info = (symbol_info_t**) Ellie_realloc(self->symbol_info, (1 + self->content) * sizeof(symbol_info_t*));
    assert_allocation(self->symbol_info);
}

bool table_exists(table_t* self, char* name) {
    size_t i = 0;
    while (i < self->content) {
        if (str__equals(self->symbol_info[i]->name, name))
            return true;
        i++;
    }
    return false;
}

bool table_defined(table_t* self, char* name) {
    table_t* current = self;
    while (current != NULL) {
        if (table_exists(current, name))
            return true;
        current = current->parent;
    }
    return false;
}

symbol_info_t* table_lookup(table_t* self, char* name) {
    table_t* current = self;
    while (current != NULL) {
        size_t i = 0;
        while (i < current->content) {
            if (str__equals(current->symbol_info[i]->name, name))
                return current->symbol_info[i];
            i++;
        }
        current = current->parent;
    }
    fprintf(stderr, "%s::%s: error symbol %s not found.\n", __FILE__, __func__, name);
    exit(1);
    return NULL;
}

//

symbol_info_t* symbol_info_create_variable(char* name, typetag_t* data_type, bool is_global, bool is_constant) {
    symbol_info_t* symbol_info = (symbol_info_t*) Ellie_malloc(sizeof(symbol_info_t));
    assert_allocation(symbol_info);
    symbol_info->name = name;
    symbol_info->data_type = data_type;
    symbol_info->is_global = is_global;
    symbol_info->is_constant = is_constant;
    symbol_info->is_exported = false;
    return symbol_info;
}

symbol_info_t* symbol_info_create_function(char* name, typetag_t* data_type, int argc, char** parameter_names, typetag_t** parameter_types, typetag_t* return_type, bool is_asynchronous) {
    symbol_info_t* symbol_info = (symbol_info_t*) Ellie_malloc(sizeof(symbol_info_t));
    assert_allocation(symbol_info);
    symbol_info->name = name;
    symbol_info->data_type = data_type;
    symbol_info->argc = argc;
    symbol_info->param_names = parameter_names;
    symbol_info->param_types = parameter_types;
    symbol_info->return_type = return_type;
    symbol_info->is_asynchronous = is_asynchronous;
    symbol_info->is_exported = false;
    return symbol_info;
}

void symbol_info_mark_as_constant(symbol_info_t* self) {
    self->is_constant = true;
}

void symbol_info_mark_as_exported(symbol_info_t* self) {
    self->is_exported = true;
}