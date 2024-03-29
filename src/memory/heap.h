#pragma once

#include <stdint.h>
#include <stddef.h>

typedef struct heap_t {
    uint32_t block_size;
    uint32_t block_count;
    void* blocks;
    uint32_t cursor;
} heap;

void heap_initialize(heap* heap, void* data, uint32_t length, uint32_t block_size);

void* heap_alloc(heap* heap, uint32_t block_count);

void heap_free(heap* heap, const void* memory);
