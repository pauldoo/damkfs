#pragma once

#include <stdint.h>
#include <stddef.h>

struct heap {
    uint32_t block_size;
    uint32_t block_count;
    void* blocks;
    int cursor;
};

void heap_initialize(struct heap* heap, void* data, uint32_t length, uint32_t block_size);

void* heap_alloc(struct heap* heap, int block_count);

void heap_free(struct heap* heap, void* memory);
