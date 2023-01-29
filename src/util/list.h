#pragma once

#include <stdint.h>

// Really crap dynamically allocated and resizing list of 32-bit values.

typedef struct list_t {
    uint32_t* _data;
    uint32_t _allocated;
    uint32_t _used;
} list;

list* list_create();

void list_free(list*);

uint32_t list_size(list*);

void list_append(list*, uint32_t value);

uint32_t list_at(list*, uint32_t index);

