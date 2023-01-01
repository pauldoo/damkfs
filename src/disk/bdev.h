#pragma once
#include <stdint.h>

// Abstraction for "block device"


struct bdev_vtable_t;
typedef struct bdev_vtable_t bdev_vtable;

struct bdev_t;
typedef struct bdev_t bdev;

typedef void (*BdevRead)(bdev*, uint32_t, uint32_t, void*);

struct bdev_vtable_t {
    BdevRead read;
};

struct bdev_t {
    const bdev_vtable* vtable;
    const uint16_t block_size;
};


void bdev_read(bdev*, uint32_t start_block, uint32_t block_count, void* output);

