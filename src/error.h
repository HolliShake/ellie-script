#include "global.h"
#include "position.h"

#ifndef ERROR_H
#define ERROR_H
    #ifndef FINFO
        #define FINFO
        #define FILE_PATH "[invalid_path]"
        #define FILE_DATA "[invalid_data]"
    #endif

    #define throw_errorf(position, message, ...) throw_error_raw(__FILE__, __func__, __LINE__, FILE_PATH, FILE_DATA, position, message, __VA_ARGS__)
    #define throw_errore(position,   message   ) throw_error_raw(__FILE__, __func__, __LINE__, FILE_PATH, FILE_DATA, position, message)

    void throw_error_raw(const char* dbg_file, const char* dbg_fn_name, size_t line, char* file_path, char* file_data, position_t* position, char* message, ...);
#endif