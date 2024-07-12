#include "error.h"

void throw_error_raw(const char* dbg_file, const char* dbg_fn_name, size_t line, char* file_path, char* file_data, position_t* position, char* message, ...) {
    // begin fmt
    va_list args;
    va_list acpy;
    va_copy(acpy, args);
    // duplicate
    va_start(acpy, message);
    va_start(args, message);
    size_t size = vsnprintf(NULL, 0, message, args) + 1;
    va_end(args);
    char* buffer = (char*) Ellie_malloc(size);
    assert_allocation(buffer);
    vsprintf(buffer, message, acpy);
    va_end(acpy);
    // end

    char error_header[1024];
    sprintf(error_header, "\033[31mdebug(%s::%s[%zu]):[%s:%zu:%zu]::Error: %s.\n\033[0m", dbg_file, dbg_fn_name, line, file_path, position->line, position->colm, buffer);

    char* final_message = str__new(error_header);
    fprintf(stderr, "%s", final_message);
    
    GC_gcollect();
    exit(1);
}