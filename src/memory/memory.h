#pragma once

#include <stddef.h>
#include <stdint.h>

void memzero(void* p, size_t len);

int strlen(const char* str);

void initialize_heaps();

void* kmalloc(size_t len);

void kfree(void* p);

void* page_alloc(int page_count);

void page_free(void* p);
