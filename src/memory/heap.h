#pragma once

#include <stdint.h>
#include <stddef.h>

typedef struct heap_t {
    uint32_t block_size;
    uint32_t block_count;
    void* blocks;
    int cursor;
} heap;

void heap_initialize(heap* heap, void* data, uint32_t length, uint32_t block_size);

void* heap_alloc(heap* heap, int block_count);

void heap_free(heap* heap, void* memory);
