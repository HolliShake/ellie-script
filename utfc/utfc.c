#include "utfc.h"

#ifndef _UTFC_CC_H
#define _UTFC_CC_H

    #ifdef __cplusplus
    extern "C" {
    #endif

        int utf__toCodePoint(unsigned char b1, unsigned char b2, unsigned char b3, unsigned char b4) {
            int ord = 0;
            switch (utf__sizeOfUtf(b1)) {
                case 1:
                    return b1;

                case 2:
                    ord  = ((b1 & _2BYTE_FOLLOW) << 6);
                    ord |= ((b2 & _MAX_TRAILING));
                    break;
                case 3:
                    ord  = ((b1 & _3BYTE_FOLLOW) << 12);
                    ord |= ((b2 & _MAX_TRAILING) <<  6);
                    ord |= ((b3 & _MAX_TRAILING));
                    break;
                case 4:
                    ord  = ((b1 & _4BYTE_FOLLOW) << 18);
                    ord |= ((b2 & _MAX_TRAILING) << 12);
                    ord |= ((b3 & _MAX_TRAILING) <<  6);
                    ord |= ((b4 & _MAX_TRAILING));
                    break;
                default:
                    printf("DEFAULT: %c(size: %d)\n", b1, utf__sizeOfUtf(b1));
                    invalidUtf();
                    break;
            }

            if (ord == 0) {
                printf("ORD: ZERO(size: %d) [%lu, %lu, %lu, %lu]!!\n", utf__sizeOfUtf(b1), b1, b2, b3, b4);
                invalidUtf();
            }
            
            return ord;
        }

        int utf__encodeChar(int codePoint, unsigned char buffer[5]) {
            int size = utf__sizeOfCodepoint(codePoint);

            switch (size) {
                case 1:
                    buffer[0] = codePoint;
                    buffer[1] = '\0';
                    break;
                case 2:
                    buffer[0] = (codePoint >> 6) | _BYTE2;
                    buffer[1] = (codePoint & _MAX_TRAILING) | _VALID_TRAILING;
                    buffer[2] = '\0';
                    break;
                case 3:
                    buffer[0] = (codePoint >> 12) | _BYTE3;
                    buffer[1] = ((codePoint >> 6) & _MAX_TRAILING) | _VALID_TRAILING;
                    buffer[2] = (codePoint & _MAX_TRAILING) | _VALID_TRAILING;
                    buffer[3] = '\0';
                    break;
                case 4:
                    buffer[0] = (codePoint >> 18) | _BYTE4;
                    buffer[1] = ((codePoint >> 12) & _MAX_TRAILING) | _VALID_TRAILING;
                    buffer[2] = ((codePoint >> 6) & _MAX_TRAILING) | _VALID_TRAILING;
                    buffer[3] = (codePoint & _MAX_TRAILING) | _VALID_TRAILING;
                    buffer[4] = '\0';
                    break;
                default:
                    return 0;
            }

            return size;
        }

        int utf__sizeOfUtf(unsigned char firstByte) {
            if ((firstByte & _BYTE4) == _BYTE4)
                return 4;
            else if ((firstByte & _BYTE3) == _BYTE3)
                return 3;
            else if ((firstByte & _BYTE2) == _BYTE2)
                return 2;
            else if ((firstByte & _BYTE1) == 0)
                return 1;

            invalidUtf();
            return 0;
        }

        int utf__sizeOfCodepoint(int codePoint) {
            if (codePoint < 0x80)
                return 1;
            else if (codePoint < 0x000800)
                return 2;
            else if (codePoint < 0x010000)
                return 3;
            else if (codePoint < 0x110000)
                return 4;
            invalidUtf();
            return 0;
        }

        int utf__isLetter(int codePoint) {
            if (codePoint < 0x80) {
                if (codePoint == '_') {
                    return true;
                }
                return (((unsigned int) codePoint | 0x20) - 0x61) < 26;
            }
            switch (utf8proc_category(codePoint)) {
                case UTF8PROC_CATEGORY_LU:
                case UTF8PROC_CATEGORY_LL:
                case UTF8PROC_CATEGORY_LT:
                case UTF8PROC_CATEGORY_LM:
                case UTF8PROC_CATEGORY_LO:
                    return true;
            }

            return false;
        }

        int utf__isDigit(int codePoint) {
            if (codePoint < 0x80) {
                return ((unsigned int) codePoint - '0') < 10;
            }
            return utf8proc_category(codePoint) == UTF8PROC_CATEGORY_ND;
        }

        int utf__isLetterOrDigit(int codePoint) {
            if (codePoint < 0x80) {
                if (codePoint == '_') {
                    return true;
                }
                if ((((unsigned int) codePoint | 0x20) - 0x61) < 26) {
                    return true;
                }
                return ((unsigned int) codePoint - '0') < 10;
            }
            switch (utf8proc_category(codePoint)) {
                case UTF8PROC_CATEGORY_LU:
                case UTF8PROC_CATEGORY_LL:
                case UTF8PROC_CATEGORY_LT:
                case UTF8PROC_CATEGORY_LM:
                case UTF8PROC_CATEGORY_LO:
                    return true;
                case UTF8PROC_CATEGORY_ND:
                    return true;
            }
            return false;
        }

        int utf__isWhiteSpace(int codePoint) {
            switch (codePoint) {
                case ' ':
                case '\b':
                case '\t':
                case '\n':
                case '\r':
                    return true;
            }
            return false;
        }

        char* utf__runeToString(int codePoint) {
            size_t size = 
                utf__sizeOfCodepoint(codePoint);

            unsigned char buffer[5] = {0, 0, 0, 0, 0};

            int res = utf8proc_encode_char(codePoint, buffer); buffer[size] = '\0';
            if (res != size) {
                fprintf(stderr, "Failed to create string from rune.\n");
                exit(1);
            }

            // assert
            int code;
            if ((code = utf__toCodePoint(buffer[0], buffer[1], buffer[2], buffer[3])) != codePoint) {
                fprintf(stderr, "Mismatch codepoint.\n");
                exit(1);
            }

            char* final = malloc(sizeof(unsigned char) * (strlen(buffer) + 1)); final[size] = '\0';

            if (final == NULL) {
                fprintf(stderr, "Failed to create string from rune.\n");
                exit(1);
            }

            strcpy(final, buffer);
            return final;
        }

        size_t utf8__charLength(char* __str) {
            unsigned char c = (unsigned char) *__str;
            return utf__sizeOfUtf(c);
        }

        int utf__charCodeAt(char* __str, size_t index) {
            char* str = __str;

            for (size_t i = 0; i < index && *str; i++) {
                str += utf8__charLength(str);
            }

            if (!*str) {
                return 0;
            }

            int len = utf8__charLength(str);

            switch (len) {
                case 1:
                    return utf__toCodePoint(
                        (unsigned char) str[0], 
                        0, 
                        0, 
                        0
                    );

                case 2:
                    return utf__toCodePoint(
                        (unsigned char) str[0], 
                        (unsigned char) str[1], 
                        0, 
                        0
                    );
                
                case 3:
                    return utf__toCodePoint(
                        (unsigned char) str[0], 
                        (unsigned char) str[1], 
                        (unsigned char) str[2], 
                        0
                    );
                
                case 4:
                    return utf__toCodePoint(
                        (unsigned char) str[0], 
                        (unsigned char) str[1], 
                        (unsigned char) str[2], 
                        (unsigned char) str[3]
                    );
                default:
                    return 0;
            }
        }

        char* utf__charAt(char* __str, size_t index) {
            return utf__runeToString(utf__charCodeAt(__str, index));
        }

        size_t utf__length(char* __str) {
            size_t length = 0;
            char* str = __str;
            while (*str) {
                str += utf8__charLength(str);
                length++;
            }
            return length;
        }

    #ifdef __cplusplus
    }
    #endif

#endif