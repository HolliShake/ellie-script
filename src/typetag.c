#include "typetag.h"

typetag_t* typetag_create(char* name) {
    typetag_t* typetag = (typetag_t*) Ellie_malloc(sizeof(typetag_t));
    assert_allocation(typetag);
    typetag->name = name;
    typetag->is_nullable = false;
    typetag->is_array = false;
    typetag->is_object = false;
    typetag->is_type = false;
    typetag->is_array_init = false;
    typetag->is_object_init = false;
    /**** Child ******/
    typetag->inner_0 = NULL;
    typetag->inner_1 = NULL;

    /**** Members ****/
    typetag->members = (typetag_member_info_t**) Ellie_malloc(sizeof(typetag_member_info_t*));
    assert_allocation(typetag->members);
    typetag->members[0] = NULL;
    typetag->member_count = 0;

    /**** For function ****/
    typetag->argc = 0;
    typetag->is_variadict = false;
    typetag->is_callalble = false;
    typetag->is_asynchronous = false;
    typetag->return_type = NULL;
    typetag->param_types = NULL;
    return typetag;
}

typetag_t* typetag_create_typed_object(typetag_t* key, typetag_t* val) {
    char* str = str__format("%s<%s, %s>", TYPETAG_KEYWORD_OBJECT, typetag_get_name(key), typetag_get_name(val));
    typetag_t* obj = typetag_create(str);
    obj->is_object = true;
    obj->inner_0 = key;
    obj->inner_1 = val;
    return obj;
}

typetag_t* typetag_create_typed_array(typetag_t* element_type) {
    char* str = str__format("%s<%s>", TYPETAG_KEYWORD_ARRAY, typetag_get_name(element_type));
    typetag_t* arr = typetag_create(str);
    arr->is_array = true;
    arr->inner_0 = element_type;
    return arr;
}

typetag_t* typetag_create_function_type(typetag_t** param_types, typetag_t* return_type, int argc, bool is_variadict, bool is_async) {
    char* type_strings = str__new(""), *old;
    for (int i = 0; i < argc; i++) {
        type_strings = str__concat(old = type_strings, typetag_get_name(param_types[i]));
        Ellie_free(old);
        if (i < argc - 1) {
            type_strings = str__concat(old = type_strings, ", ");
            Ellie_free(old);
        }
    }

    char* header = (is_async) ? "async " : "";
    char* str = str__format("%s%s(%s) -> %s", header, KEYWORD_FN, type_strings, typetag_get_name(return_type));
    Ellie_free(type_strings);
    
    typetag_t* typetag = typetag_create(str);
    typetag->argc = argc;
    typetag->is_variadict = is_variadict;
    typetag->is_callalble = true;
    typetag->is_asynchronous = is_async;
    typetag->return_type = return_type;
    typetag->param_types = param_types;
    return typetag;
}


static
void** copy_array(void** arr, bool is_member) {
    if (arr == NULL) {
        return NULL;
    }

    void** array_copy = (void**) Ellie_malloc(sizeof(void*));
    assert_allocation(array_copy);
    array_copy[0] = NULL;

    for (size_t i = 0; arr[i] != NULL; i++) {
        array_copy[i] = (is_member) 
            ? (void*) typetag_member_info_clone((typetag_member_info_t*) arr[i]) 
            : (void*) typetag_clone((typetag_t*) arr[i]);
        // Resize the array
        array_copy = (void**) Ellie_realloc(array_copy, sizeof(void*) * (i + 2));
        assert_allocation(array_copy);
        array_copy[i + 1] = NULL;
    }
    return array_copy;
}

typetag_t* typetag_clone(typetag_t* typetag) {
    // deep copy
    if (typetag_is_any(typetag))
        return typetag;
    if (typetag_is_number(typetag))
        return typetag;
    if (typetag_is_int(typetag))
        return typetag;
    if (typetag_is_string(typetag))
        return typetag;
    if (typetag_is_bool(typetag))
        return typetag;
    if (typetag_is_void(typetag))
        return typetag;
    if (typetag_is_null(typetag))
        return typetag;

    /****************************/
    
    typetag_t* clone = (typetag_t*) Ellie_malloc(sizeof(typetag_t));
    assert_allocation(typetag);
    clone->name = str__new(typetag->name);
    clone->is_nullable = typetag->is_nullable;
    clone->is_array = typetag->is_array;
    clone->is_object = typetag->is_object;
    clone->is_type = typetag->is_type;
    typetag->is_array_init = typetag->is_array_init;
    typetag->is_object_init = typetag->is_object_init;
    /**** Child *******/
    clone->inner_0 = (typetag->inner_0 != NULL) 
        ? typetag_clone(typetag->inner_0) 
        : NULL;
    clone->inner_1 = (typetag->inner_1 != NULL) 
        ? typetag_clone(typetag->inner_1) 
        : NULL;

    /**** Members *****/
    clone->member_count = typetag->member_count;
    clone->members = (typetag->members != NULL)
        ? (typetag_member_info_t**) copy_array((void**) typetag->members, true)
        : NULL;

    /**** For function ****/
    clone->argc = typetag->argc;
    clone->is_variadict = typetag->is_variadict;
    clone->is_callalble = typetag->is_callalble;
    clone->is_asynchronous = typetag->is_asynchronous;
    clone->return_type = (typetag->return_type != NULL) 
        ? typetag_clone(typetag->return_type) 
        : NULL;
    clone->param_types = (typetag->param_types != NULL) 
        ? (typetag_t**) copy_array((void**) typetag->param_types, false) 
        : NULL;
    return clone;
}

char* typetag_get_name(typetag_t* self) {
    if (self == NULL) {
        return str__new("null");
    }
    if (self->name == NULL) {
        return str__new("null");
    }
    return str__equals(self->name, KEYWORD_VOID) ? str__new(KEYWORD_NULL) : self->name;
}

char* typetag_get_default_value_string(typetag_t* self) {
    if (typetag_is_number(self))
        return str__new("0");
    else if (typetag_is_int(self))
        return str__new("0n");
    else if (typetag_is_string(self))
        return str__new("\"\"");
    else if (typetag_is_bool(self))
        return str__new("false");
    else if (typetag_is_void(self))
        return str__new("void");
    else if (typetag_is_null(self))
        return str__new("null");
    else
        // Just put it here :P
        return str__new("null");
}

void typetag_to_type(typetag_t* self) {
    self->is_type = true;
}

/*****************************************/

void typetag_add_member(typetag_t* self, char* member, typetag_t* data_type, bool is_getter, bool is_accessible, bool is_mutable, bool is_static) {
    typetag_member_info_t* member_info = (typetag_member_info_t*) Ellie_malloc(sizeof(typetag_member_info_t));
    assert_allocation(member_info);
    member_info->member = member;
    member_info->data_type = data_type;
    member_info->is_getter = is_getter;
    member_info->is_accessible = is_accessible;
    member_info->is_mutable = is_mutable;
    member_info->is_static = is_static;

    self->members[self->member_count++] = member_info;
    self->members = (typetag_member_info_t**) Ellie_realloc(self->members, sizeof(typetag_member_info_t*) * (self->member_count + 1));
    assert_allocation(self->members);
    self->members[self->member_count] = NULL;
}

bool typetag_has_member(typetag_t* self, char* member) {
    for (size_t i = 0; i < self->member_count; i++) {
        if (str__equals(self->members[i]->member, member))
            return true;
    }
    return false;
}

typetag_member_info_t* typetag_get_member(typetag_t* self, char* member) {
    for (size_t i = 0; i < self->member_count; i++) {
        if (str__equals(self->members[i]->member, member))
            return self->members[i];
    }
    return NULL;
}

/*****************************************/

bool typetag_is_any(typetag_t* self) {
    return str__equals(self->name, "any");
}

bool typetag_is_number(typetag_t* self) {
    return str__equals(self->name, KEYWORD_NUMBER);
}

bool typetag_is_int(typetag_t* self) {
    return str__equals(self->name, KEYWORD_INT);
}

bool typetag_is_string(typetag_t* self) {
    return str__equals(self->name, KEYWORD_STRING);
}
bool typetag_is_bool(typetag_t* self) {
    return str__equals(self->name, KEYWORD_BOOL);
}

bool typetag_is_void(typetag_t* self) {
    return str__equals(self->name, KEYWORD_VOID);
}

bool typetag_is_null(typetag_t* self) {
    return str__equals(self->name, KEYWORD_NULL);
}

bool typetag_is_nullable(typetag_t* self) {
    return typetag_is_void(self) || typetag_is_null(self) || self->is_nullable;
}

bool typetag_is_callable(typetag_t* self) {
    return self->is_callalble;
}

bool typetag_is_object(typetag_t* self) {
    return self->is_object && self->inner_0 != NULL && self->inner_1 != NULL;
}

bool typetag_is_array(typetag_t* self) {
    return self->is_array && self->inner_0 != NULL;
}

bool typetag_is_equal(typetag_t* lhs, typetag_t* rhs) {
    return str__equals(typetag_get_name(lhs), typetag_get_name(rhs));
}

void typetag_to_nullable(typetag_t* self) {
    self->is_nullable = true;
}

bool typetag_can_accept(typetag_t* variable_type, typetag_t* value_type) {
    if (typetag_is_any(variable_type))
        return true;
    else if (typetag_is_nullable(variable_type) && typetag_is_null(value_type))
        return true;
    else if (typetag_is_number(variable_type) && typetag_is_number(value_type))
        return true;
    else if (typetag_is_int(variable_type) && typetag_is_int(value_type))
        return true;
    else if (typetag_is_string(variable_type) && typetag_is_string(value_type))
        return true;
    else if (typetag_is_bool(variable_type) && typetag_is_bool(value_type))
        return true;
    else if (typetag_is_void(variable_type) && typetag_is_void(value_type))
        return true;
    else if (typetag_is_void(variable_type) && (typetag_is_void(value_type) || typetag_is_null(value_type)))
        return true;
    else if (typetag_is_array(variable_type) && typetag_is_array(value_type))
        return typetag_can_accept(variable_type->inner_0, value_type->inner_0);
    return false;
}

typetag_t* typetag_equivalent(context_t* global, typetag_t* lhs, typetag_t* rhs) {
    if (str__equals(typetag_get_name(lhs), typetag_get_name(rhs)))
        return typetag_clone(lhs);
    return NULL;
}

typetag_t* typetag_bitwise_not(context_t* global, typetag_t* typetag) {
    if (typetag_is_number(typetag))
        return context_get_default_number_t(global);
    else if (typetag_is_int(typetag))
        return context_get_default_int_t(global);
    return NULL;
}

typetag_t* typetag_not(context_t* global, typetag_t* typetag) {
    if (typetag_is_bool(typetag) || typetag_is_nullable(typetag) || typetag_is_null(typetag))
        return context_get_default_bool_t(global);
    return NULL;
}

typetag_t* typetag_plus_minus(context_t* global, typetag_t* typetag) {
    if (typetag_is_number(typetag))
        return context_get_default_number_t(global);
    else if (typetag_is_int(typetag))
        return context_get_default_int_t(global);
    return NULL;
}

typetag_t* typetag_incdec(context_t* global, typetag_t* typetag) {
    if (typetag_is_number(typetag))
        return context_get_default_number_t(global);
    else if (typetag_is_int(typetag))
        return context_get_default_int_t(global);
    return NULL;
}

typetag_t* typetag_mul(context_t* global, typetag_t* lhs, typetag_t* rhs) {
    if (typetag_is_number(lhs) && typetag_is_number(rhs))
        return context_get_default_number_t(global);
    else if (typetag_is_int(lhs) && typetag_is_int(rhs))
        return context_get_default_int_t(global);
    return NULL;
}

typetag_t* typetag_div(context_t* global, typetag_t* lhs, typetag_t* rhs) {
    if (typetag_is_number(lhs) && typetag_is_number(rhs))
        return context_get_default_number_t(global);
    else if (typetag_is_int(lhs) && typetag_is_int(rhs))
        return context_get_default_int_t(global);
    return NULL;
}

typetag_t* typetag_mod(context_t* global, typetag_t* lhs, typetag_t* rhs) {
    if (typetag_is_number(lhs) && typetag_is_number(rhs))
        return context_get_default_number_t(global);
    else if (typetag_is_int(lhs) && typetag_is_int(rhs))
        return context_get_default_int_t(global);
    return NULL;
}

typetag_t* typetag_add(context_t* global, typetag_t* lhs, typetag_t* rhs) {
    if (typetag_is_string(lhs) && typetag_is_string(rhs))
        return context_get_default_string_t(global);
    else if (typetag_is_number(lhs) && typetag_is_number(rhs))
        return context_get_default_number_t(global);
    else if (typetag_is_int(lhs) && typetag_is_int(rhs))
        return context_get_default_int_t(global);
    return NULL;
}

typetag_t* typetag_sub(context_t* global, typetag_t* lhs, typetag_t* rhs) {
    if (typetag_is_number(lhs) && typetag_is_number(rhs))
        return context_get_default_number_t(global);
    else if (typetag_is_int(lhs) && typetag_is_int(rhs))
        return context_get_default_int_t(global);
    return NULL;
}

typetag_t* typetag_shift(context_t* global, typetag_t* lhs, typetag_t* rhs) {
    if (typetag_is_number(lhs) && typetag_is_number(rhs))
        return context_get_default_number_t(global);
    else if (typetag_is_int(lhs) && typetag_is_int(rhs))
        return context_get_default_int_t(global);
    return NULL;
}

typetag_t* typetag_bitwise(context_t* global, typetag_t* lhs, typetag_t* rhs) {
    if (typetag_is_number(lhs) && typetag_is_number(rhs))
        return context_get_default_number_t(global);
    else if (typetag_is_int(lhs) && typetag_is_int(rhs))
        return context_get_default_int_t(global);
    return NULL;
}

typetag_t* typetag_logical(context_t* global, typetag_t* lhs, typetag_t* rhs) {
    return typetag_equivalent(global, lhs, rhs);
}

typetag_t* typetag_compare(context_t* global, typetag_t* bool_type, typetag_t* lhs, typetag_t* rhs, bool numeric_only)  {
    if ((typetag_is_string(lhs) && typetag_is_string(rhs)) && !numeric_only)
        return context_get_default_bool_t(global);
    else if ((typetag_is_number(lhs) && typetag_is_number(rhs)) && numeric_only)
        return context_get_default_bool_t(global);
    else if ((typetag_is_int(lhs) && typetag_is_int(rhs)) && numeric_only)
        return context_get_default_bool_t(global);
    return NULL;
}

//
typetag_member_info_t* typetag_member_info_clone(typetag_member_info_t* self) {
    typetag_member_info_t* clone = (typetag_member_info_t*) Ellie_malloc(sizeof(typetag_member_info_t));
    assert_allocation(clone);
    clone->member = str__new(self->member);
    clone->data_type = typetag_clone(self->data_type);
    clone->is_getter = self->is_getter;
    clone->is_accessible = self->is_accessible;
    clone->is_mutable = self->is_mutable;
    clone->is_static = self->is_static;
    return self;
}