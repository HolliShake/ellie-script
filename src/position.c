#include "position.h"

position_t* position_create(size_t line, size_t colm) {
    position_t* position = (position_t*) Ellie_malloc(sizeof(position_t));
    assert_allocation(position);
    position->line = line;
    position->colm = colm;
    position->end_line = 0;
    position->end_colm = 0;
    return position;
}

position_t* position_merge(position_t* start, position_t* end) {
    position_t* position = (position_t*) Ellie_malloc(sizeof(position_t));
    assert_allocation(position);
    position->line = start->line;
    position->colm = start->colm;
    position->end_line = end->end_line;
    position->end_colm = end->end_colm;
    return position;
}

char* position_to_string(position_t* self) {
    return str__format("position_t { line: %zu, colm: %zu, end_line: %zu, end_colm: %zu }", self->line, self->colm, self->end_line, self->end_colm);
}