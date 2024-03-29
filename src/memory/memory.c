#include "memory.h"

#include "terminal/terminal.h"
#include "memory/heap.h"

static void * const heap_base = ((void*)0x01000000); // starts at 16M
static const size_t heap_size = 0x1000000; // is 16M long

static heap malloc_heap = {0};
static heap page_heap = {0};

void memzero(void* p, size_t len) {
    ASSERT(p != 0);
    for (size_t i = 0; i < len; i++) {
        ((uint8_t*)p)[i] = 0;
    }
}

void memcpy(const void* src, void* dst, size_t len) {
    ASSERT(src != 0);
    ASSERT(dst != 0);
    for (size_t i = 0; i < len; i++) {
        ((uint8_t*)dst)[i] = ((uint8_t*)src)[i];
    }
}


int strlen(const char* str) {
    int len = 0;
    while (str[len] != 0) {
        len++;
    }
    return len;
}

void initialize_heaps() {
    heap_initialize(&malloc_heap, heap_base, heap_size, 32);
    heap_initialize(&page_heap, heap_base + heap_size, heap_size, 4096);
}

void* kmalloc(size_t len) {
    return heap_alloc(
        &malloc_heap,
        (len + malloc_heap.block_size - 1) / malloc_heap.block_size);
}

void* kcalloc(size_t len) {
    void* const result = kmalloc(len);
    if (result != NULL) {
        memzero(result, len);
    }
    return result;
}

void kfree(const void* p) {
    return heap_free(&malloc_heap, p);
}

void* page_alloc(int page_count) {
    void* result = heap_alloc(
        &page_heap,
        page_count
    );
    return result;
}

void* page_calloc(int page_count) {
    void* const result = page_alloc(page_count);
    if (result != NULL) {
        memzero(result, page_count * page_heap.block_size);
    }
    return result;
}

void page_free(const void* p) {
    return heap_free(
        &page_heap,
        p);
}
