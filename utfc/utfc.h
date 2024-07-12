#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utf8proc/utf8proc.h"

// https://github.com/andydevs69420/atom-compiler-c/blob/main/identifier/utf8.c

#ifndef _UTF8C_H
#define _UTF8C_H

    #ifdef __cplusplus
    extern "C" {
    #endif

        #define invalidUtf() { fprintf(stderr, "UTF::%s:Error: Invalid utf!!\n", __func__); exit(1); }

        enum UTF {
            _BYTE1 = 0b00000000,
            _BYTE2 = 0b11000000,
            _BYTE3 = 0b11100000,
            _BYTE4 = 0b11110000,

            _2BYTE_FOLLOW = 0b00011111,
            _3BYTE_FOLLOW = 0b00001111,
            _4BYTE_FOLLOW = 0b00000111,

            _VALID_TRAILING = 0b10000000,
            _MAX_TRAILING   = 0b00111111
        };

        int utf__toCodePoint(unsigned char, unsigned char, unsigned char, unsigned char);
        int utf__encodeChar(int codePoint, unsigned char buffer[5]);
        int utf__sizeOfUtf(unsigned char);
        int utf__sizeOfCodepoint(int codePoint);
        int utf__isLetter(int codePoint);
        int utf__isDigit(int codePoint);
        int utf__isLetterOrDigit(int codePoint);
        int utf__isWhiteSpace(int codePoint);
        char* utf__runeToString(int codePoint);
        size_t utf8__charLength(char* __str);
        int utf__charCodeAt(char* __str, size_t index);
        char* utf__charAt(char* __str, size_t index);
        size_t utf__length(char* __str);

    #ifdef __cplusplus
    }
    #endif

#endif