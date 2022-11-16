#include "heap.h"

#include "terminal/terminal.h"
#include "memory/memory.h"

#include <stdbool.h>

static const uint8_t in_use_bit = 0x1;
static const uint8_t is_first_bit = 0x2;
static const uint8_t is_last_bit = 0x4;

static uint8_t* table_entry(heap* heap, uint32_t i) {
    ASSERT( i < heap->block_count );
    return ((uint8_t*)(heap->blocks)) + i;
}

static bool range_is_free(heap* heap, uint32_t begin, uint32_t end) {
    ASSERT( begin < end && end <= heap->block_count );
    for (uint32_t i = begin; i < end; i++) {
        if ((*(table_entry(heap, i)) & in_use_bit) == in_use_bit) {
            return false;
        }
    }
    return true;
}

// marks [begin, end) blocks as allocated.
static void mark_blocks_used(heap* heap, uint32_t begin, uint32_t end) {
    ASSERT( begin < end && end <= heap->block_count );
    ASSERT(range_is_free(heap, begin, end));
    for (uint32_t i = begin; i < end; i++) {
        *(table_entry(heap, i)) =
            in_use_bit |
            (i == begin ? is_first_bit : 0x0) |
            (i == (end-1) ? is_last_bit : 0x0); 
    }
}

// ceil(a/b)
static int ceil_div(int a, int b) {
    return (a+(b-1)) / b;
}

void heap_initialize(heap* heap, void* data, uint32_t length, uint32_t block_size) {
    ASSERT( ((uint32_t)data) % block_size == 0 );
    memzero(data, length);
    heap->block_size = block_size;
    heap->block_count = length / block_size;
    heap->blocks = data;
    heap->cursor = 0;

    for (uint32_t i = 0; i < heap->block_count; i++) {
        *(table_entry(heap, i)) = 0x0;
    }
    // We use the first few blocks to store the entry table
    // so mark those as used.
    mark_blocks_used(heap, 0, ceil_div(heap->block_count, heap->block_size));
}

void* heap_alloc(heap* heap, uint32_t block_count) {
    ASSERT( heap->cursor < heap->block_count );
    const uint32_t orig_cursor = heap->cursor;
    do {
        const uint32_t end = heap->cursor + block_count;
        if (end <= heap->block_count &&
            range_is_free(heap, heap->cursor, end)) {
            mark_blocks_used(heap, heap->cursor, end);
            void* const result = heap->blocks + (heap->cursor * heap->block_size);
            ASSERT( ((uint32_t)result) % heap->block_size == 0 );
            return result;
        }
        heap->cursor = (heap->cursor+1) % heap->block_count;
    } while (heap->cursor != orig_cursor);

    ASSERT(0);
}

void heap_free(heap* heap, const void* memory) {
    ASSERT( heap->blocks <= memory &&
        memory <= heap->blocks + (heap->block_size * (heap->block_count-1)));
    ASSERT( ( memory - heap->blocks ) % heap->block_size == 0 );

    int c = (memory - heap->blocks) / heap->block_size;
    ASSERT((*table_entry(heap, c) & in_use_bit) == in_use_bit);
    ASSERT((*table_entry(heap, c) & is_first_bit) == is_first_bit);
    while (1) {
        int is_last = (*(table_entry(heap, c)) & is_last_bit) == is_last_bit;
        *(table_entry(heap, c)) = 0x0;
        if (is_last) {
            break;
        }
        c = c+1;
    }
};
