#include "global.h"

#ifndef ELLIE_UTIL_C
#define ELLIE_UTIL_C

    /******* UTILITY ****/
    int hash32(char* key) {
        if (strlen(key) <= 0) return 0;
        int hash = 0;
        for (size_t i = 0; i < strlen(key); i++) {
            hash = 31 * hash + key[i];
            hash |= 0;
        }
        return hash;
    }

    /******** STRING ****/ 
    char* str__new(char* __str) {
        char* new_str = (char*) Ellie_malloc(strlen(__str) + 1);
        assert_allocation(new_str);
        new_str[0] = '\0';
        sprintf(new_str, "%s", __str);
        return new_str;
    }

    char* str__from_char_code(int __code) {
        char* cmalloced_str = utf__runeToString(__code);
        char* strwithcg = str__new(cmalloced_str);
        free(cmalloced_str);
        return strwithcg;
    }

    int str__char_code_at(char* __str, size_t __index) {
        return utf__charCodeAt(__str, __index);;
    }

    char* str__char_at(char* __str, size_t __index) {
        return str__from_char_code(str__char_code_at(__str, __index));
    }

    char* str__format(char* __fmt, ...) {
        va_list args;
        va_start(args, __fmt);

        char* buffer = (char*) Ellie_malloc(1024 * sizeof(char));
        assert_allocation(buffer);

        vsprintf(buffer, __fmt, args);
        va_end(args);

        return buffer;
    }

    char* str__concat(char* __dst, char* __src) {
        size_t size1 = strlen(__dst);
        size_t size2 = strlen(__src);
        size_t total = size1 + size2 + 1;

        char *new_str = (char*) Ellie_malloc(total * sizeof(char));
        assert_allocation(new_str);

        strcpy(new_str, __dst);
        strcat(new_str, __src);
        return new_str;
    }

    char* str__concat_char(char* __dst, char __chr) {
        size_t size1 = strlen(__dst);
        size_t size2 = 2;
        size_t total = size1 + size2 + 1;

        char *new_str = (char*) Ellie_malloc(total * sizeof(char));
        assert_allocation(new_str);

        strcpy(new_str, __dst);
        strcat(new_str, (char[2]) { __chr, '\0' });
        return new_str;
    }

    bool str__equals(char* __str0, char* __str1) {
        if (__str0 == NULL || __str1 == NULL) return false;
        return strcmp(__str0, __str1) == 0;
    }

    size_t str__length(char* __str) {
        return utf__length(__str);
    }

    /******** FILEIO ****/ 
    char* fileio__read_file(char* __path) {
        FILE *fp;
        char *buffer;
        long length;

        fp = fopen(__path, "r");
        if (fp == NULL) {
            fprintf(stderr, "%s::%s: Failed to open file \"%s\"...\n", __FILE__, __func__, __path);
            exit(1);
        }

        buffer = str__new("");

        char chr;
        char* old;
        while ((chr = fgetc(fp)) != EOF) {
            buffer = str__concat_char((old = buffer), chr);
            Ellie_free(old);
        }

        return buffer;
    }

#endif