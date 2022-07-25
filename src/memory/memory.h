#pragma once

#include <stddef.h>
#include <stdint.h>

void memzero(void* p, size_t len);

int strlen(const char* str);

void initialize_heap();

void* kmalloc(size_t len);

void kfree(void* p);
