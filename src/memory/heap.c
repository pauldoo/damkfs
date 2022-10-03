#include "heap.h"

#include "terminal/terminal.h"
#include "memory/memory.h"

static const uint8_t in_use_bit = 0x1;
static const uint8_t is_first_bit = 0x2;
static const uint8_t is_last_bit = 0x4;

static uint8_t* table_entry(struct heap* heap, int i) {
    ASSERT( 0 <= i && i < heap->block_count );
    return ((uint8_t*)(heap->blocks)) + i;
}

static int range_is_free(struct heap* heap, int begin, int end) {
    ASSERT( 0 <= begin && begin < end && end <= heap->block_count );
    for (int i = begin; i < end; i++) {
        if ((*(table_entry(heap, i)) & in_use_bit) == in_use_bit) {
            return 0;
        }
    }
    return 1;
}

// marks [begin, end) blocks as allocated.
static void mark_blocks_used(struct heap* heap, int begin, int end) {
    ASSERT( 0 <= begin && begin < end && end <= heap->block_count );
    ASSERT(range_is_free(heap, begin, end));
    for (int i = begin; i < end; i++) {
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

void heap_initialize(struct heap* heap, void* data, uint32_t length, uint32_t block_size) {
    ASSERT( ((uint32_t)data) % block_size == 0 );
    memzero(data, length);
    heap->block_size = block_size;
    heap->block_count = length / block_size;
    heap->blocks = data;
    heap->cursor = 0;

    for (int i = 0; i < heap->block_count; i++) {
        *(table_entry(heap, i)) = 0x0;
    }
    // We use the first few blocks to store the entry table
    // so mark those as used.
    mark_blocks_used(heap, 0, ceil_div(heap->block_count, heap->block_size));
}

void* heap_alloc(struct heap* heap, int block_count) {
    ASSERT( heap->cursor >= 0 && heap->cursor < heap->block_count );
    const int orig_cursor = heap->cursor;
    do {
        const int end = heap->cursor + block_count;
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

void heap_free(struct heap* heap, void* memory) {
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
