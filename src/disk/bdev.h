#pragma once
#include <stdint.h>

// Abstraction for "block device"


struct bdev_vtable_t;
typedef struct bdev_vtable_t bdev_vtable;

struct bdev_t;
typedef struct bdev_t bdev;

typedef uint32_t (*BdevBlockCount)(bdev*);
typedef uint32_t (*BdevLogicalLength)(bdev*);
typedef void (*BdevRead)(bdev*, uint32_t, uint32_t, void*);
typedef void (*BdevClose)(bdev*);

struct bdev_vtable_t {
    BdevBlockCount block_count;
    BdevLogicalLength logical_length;
    BdevRead read;
    BdevClose close;
};

struct bdev_t {
    const bdev_vtable* vtable;
    const uint16_t block_size;    
};


uint32_t bdev_block_count(bdev*);

uint32_t bdev_logical_length(bdev*);

void bdev_read(bdev*, uint32_t start_block, uint32_t block_count, void* output);

void bdev_close(bdev*);
