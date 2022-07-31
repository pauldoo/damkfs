#include "memory.h"

#include "terminal/terminal.h"
#include "memory/heap.h"

static void * const heap_base = ((void*)0x01000000); // starts at 16M
static const size_t heap_size = 0x10000000; // is 256M long

static struct heap malloc_heap = {0};
static struct heap page_heap = {0};

void memzero(void* p, size_t len) {
    for (size_t i = 0; i < len; i++) {
        ((char*)p)[i] = 0;
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

void kfree(void* p) {
    return heap_free(&malloc_heap, p);
}

void* page_alloc(int page_count) {
    return heap_alloc(
        &page_heap,
        page_count
    );
}

void page_free(void* p) {
    return heap_free(
        &page_heap,
        p);
}
