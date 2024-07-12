#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include "../utfc/utfc.h"
#include "../libgc/gc.h"

#define Ellie_malloc(sz) GC_MALLOC(sz)
#define Ellie_realloc(ptr, sz) GC_REALLOC(ptr, sz)
#define Ellie_calloc(m,n) GC_MALLOC((m)*(n))
#define Ellie_free(ptr) GC_FREE(ptr)

#ifndef ELLIE_UTIL_H
#define ELLIE_UTIL_H
    #define MAX_IDENTIFIER_LENGTH 75

    #define LABEL() printf("\033[32m%s[%zu]::%s(...)\033[0m\n", __FILE__, __LINE__, __func__);

    #define debug0_and_exit(msg) {\
        fprintf(stderr, "\033[31m%s[%zu]::%s(...): %s.\033[0m\n", __FILE__, __LINE__, __func__, msg);\
        GC_gcollect();\
        exit(1);\
    }\

    #define debug1_and_exit(msg, ...) {\
        fprintf(stderr, "\033[31m%s[%zu]::%s(...): %s.\033[0m\n", __FILE__, __LINE__, __func__, str__format(msg, __VA_ARGS__));\
        GC_gcollect();\
        exit(1);\
    }\

    #define assert_allocation(ptr)\
        if (ptr == NULL) {\
            fprintf(stderr, "%s[%zu]::%s(...): Failed to allocate memory!!!\n", __FILE__, __LINE__, __func__);\
            GC_gcollect();\
            exit(1);\
        }\

    /******** STRING ****/ 
    char* str__new(char* __str);
    char* str__from_char_code(int __code);
    int str__char_code_at(char* __str, size_t __index);
    char* str__char_at(char* __str, size_t __index);
    char* str__format(char* __fmt, ...);
    char* str__concat(char* __dst, char* __chr);
    char* str__concat_char(char* __dst, char __chr);
    bool str__equals(char* __str0, char* __str1);
    size_t str__length(char* __str);

    /******** FILEIO ****/ 
    char* fileio__read_file(char* __path);

#endif