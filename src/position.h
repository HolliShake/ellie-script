#include "global.h"

#ifndef POSITION_H
#define POSITION_H
    typedef struct position_struct {
        size_t line;
        size_t colm;
        size_t end_line;
        size_t end_colm;
    } position_t;

    position_t* position_create(size_t line, size_t colm);
    position_t* position_merge(position_t* start, position_t* end);
    char* position_to_string(position_t* self);
#endif