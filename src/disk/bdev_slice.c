#include "bdev_slice.h"

#include "memory/memory.h"
#include "terminal/terminal.h"

typedef struct bdev_slice_t {
    bdev base;
    bdev* delegate;
    list* blocks;
    uint32_t logical_length;
} bdev_slice;

static uint32_t bdev_slice_block_count(bdev*);
static uint32_t bdev_slice_logical_length(bdev*);
static void bdev_slice_read(bdev*, uint32_t, uint32_t, void*);
static void bdev_slice_close(bdev*);


static const bdev_vtable bdev_slice_vtable = {
    .block_count = bdev_slice_block_count,
    .close = bdev_slice_close,
    .logical_length = bdev_slice_logical_length,
    .read = bdev_slice_read
};


static uint32_t bdev_slice_block_count(bdev* bdev) {
    ASSERT(bdev->vtable == &bdev_slice_vtable);
    bdev_slice* slice = (bdev_slice*)(bdev);

    return list_size(slice->blocks);
}

static uint32_t bdev_slice_logical_length(bdev* bdev) {
    ASSERT(bdev->vtable == &bdev_slice_vtable);
    bdev_slice* slice = (bdev_slice*)(bdev);
    return slice->logical_length;
}

static void bdev_slice_read(bdev* bdev, uint32_t start, uint32_t length, void* out) {
    ASSERT(bdev->vtable == &bdev_slice_vtable);
    bdev_slice* slice = (bdev_slice*)(bdev);
    ASSERT( slice->base.block_size == slice->delegate->block_size );

    for (uint32_t i = 0; i < length; i++) {
        bdev_read(
            slice->delegate,
            list_at(slice->blocks, start + i),
            1,
            out + (i * slice->base.block_size));
    }
}

static void bdev_slice_close(bdev* bdev) {
    ASSERT(bdev->vtable == &bdev_slice_vtable);
    bdev_slice* slice = (bdev_slice*)(bdev);

    list_free(slice->blocks);
    kfree( slice );
}


bdev* bdev_slice_create(bdev* delegate, list* blocks, uint32_t logical_length) {
    bdev_slice* result = kmalloc(sizeof(bdev_slice));
    result->base.vtable = &bdev_slice_vtable;
    result->base.block_size = delegate->block_size;

    result->delegate = delegate;
    result->blocks = blocks;
    result->logical_length = logical_length;

    bdev* bdev_result = &(result->base);
    ASSERT(bdev_logical_length(bdev_result) <= bdev_block_count(bdev_result) * bdev_result->block_size);
    return bdev_result;
}
