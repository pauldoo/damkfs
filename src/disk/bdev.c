#include "bdev.h"

#include "terminal/terminal.h"

uint32_t bdev_block_count(bdev* bdev) {
    ASSERT(bdev != 0);
    return bdev->vtable->block_count(bdev);
}

uint32_t bdev_logical_length(bdev* bdev) {
    ASSERT(bdev != 0);
    return bdev->vtable->logical_length(bdev);
}


void bdev_read(bdev* bdev, uint32_t start_block, uint32_t block_count, void* output) {
    ASSERT(bdev != 0);
    bdev->vtable->read(bdev, start_block, block_count, output);
}

void bdev_close(bdev* bdev) {
    ASSERT(bdev != 0);
    bdev->vtable->close(bdev);
}
