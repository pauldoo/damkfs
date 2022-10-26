#pragma once
#include <stdint.h>

typedef enum disk_type_t {
    Real
} disk_type;

typedef struct disk_t {
    disk_type type;
    uint32_t sector_size;
} disk;

extern const disk default_disk;

int disk_read_sector(const disk* disk, uint32_t start, uint32_t sector_count, void* output_buffer);

void disk_init();
