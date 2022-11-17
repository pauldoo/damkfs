#include "disk.h"

#include "io/io.h"
#include "terminal/terminal.h"

const disk default_disk = {
    .type = Real,
    .sector_size = 512
};

// https://wiki.osdev.org/ATA_read/write_sectors

void disk_read_sector(const disk* disk, uint32_t start, uint32_t sector_count, void* output_buffer) {
    ASSERT(disk == &default_disk);
    ASSERT(sector_count >= 1 && sector_count <= 255);
    ASSERT(output_buffer != 0);

    out_b(0x1F6, (start >> 24) | 0xE0);
    out_b(0x1F2, sector_count);
    out_b(0x1F3, start & 0xFF);
    out_b(0x1F4, (start >> 8) & 0xFF);
    out_b(0x1F5, (start >> 16) & 0xFF);
    out_b(0x1F7, 0x20);

    uint16_t* ptr = output_buffer;
    for (uint32_t b = 0; b < sector_count; b++) {
        uint8_t status;
        while (!((status = in_b(0x1F7)) & 0x08)) {
        }

        for (uint32_t i = 0; i < (disk->sector_size/2); i++) {
            *ptr = in_w(0x1F0);
            ptr += 1;
        }
    }
}

void disk_init() {
}
