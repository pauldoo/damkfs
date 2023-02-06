#include "fat16.h"

#include "disk/bdev_slice.h"
#include "memory/memory.h"
#include "terminal/terminal.h"
#include "util/list.h"

#include <stdbool.h>

typedef struct fat16_header_t {
    uint8_t jump[3];
    uint8_t oem[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sector_count;
    uint8_t fat_count;
    uint16_t root_directory_entries_count;
    uint16_t sector_count;
    uint8_t media_descriptor_type;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t head_count;
    uint32_t hidden_sector_count;
    uint32_t large_sector_count;
    uint8_t drive_number;
    uint8_t winnt_flags;
    uint8_t signature;
    uint32_t volume_id;
    uint8_t volume_label[11];
    uint8_t system_identifier[8];
} __attribute__((packed)) fat16_header;

typedef struct fat16_directory_entry_t {
    uint8_t filename_8[8];
    uint8_t filename_3[3];
    uint8_t attributes;
    uint8_t winnt_reserved;
    uint8_t create_time_fraction; // 100ths of a second, [0-199]
    uint16_t create_time; // hour (5bits), mins (6 bits), 2seconds (5 bits)
    uint16_t create_date; // year (7bits, 0 = 1980), month (4 bits), day (5 bits)
    uint16_t accessed_date;
    uint16_t first_cluster_high; // zero on fat16
    uint16_t modification_time;
    uint16_t modification_date;
    uint16_t first_cluster_low;
    uint32_t file_length;
} __attribute__((packed)) fat16_directory_entry;

typedef struct fs_fat16_t {
    filesystem base;
    bdev* dev;
    fat16_header header;
} fs_fat16;

//
typedef struct cluster_chain_bdev_t {
    bdev base;
    fs_fat16* f16;
    list* clusters;
} cluster_chain_bdev;

static const int DIRS_PER_SECTOR = 512 / sizeof(fat16_directory_entry);

static bdev* fat16_open_file(filesystem*, fs_path*);
static void fat16_close(filesystem*);

const filesystem_vtable fat16_vtable = {
    .open_file = fat16_open_file,
    .close = fat16_close
};

static uint32_t total_sectors(fs_fat16* fs) {
    return (fs->header.sector_count == 0) ? fs->header.large_sector_count : fs->header.sector_count;
}

static uint32_t root_dir_sector_count(fs_fat16* fs) {
    return
        ((fs->header.root_directory_entries_count * sizeof(fat16_directory_entry)) + fs->header.bytes_per_sector - 1) /
        fs->header.bytes_per_sector;
}

static uint32_t root_dir_sector_start(fs_fat16* fs) {
    return fs->header.reserved_sector_count + (fs->header.sectors_per_fat * fs->header.fat_count);
}

static uint32_t first_data_sector(fs_fat16* fs) {
    return fs->header.reserved_sector_count + (fs->header.sectors_per_fat * fs->header.fat_count) + root_dir_sector_count(fs);
}

static uint32_t first_fat_sector(fs_fat16* fs) {
    return fs->header.reserved_sector_count;
}

static uint32_t data_clusters(fs_fat16* fs) {
    return (total_sectors(fs) - first_data_sector(fs)) / fs->header.sectors_per_cluster;
}

static bool name_matches(const char* name, fat16_directory_entry* entry) {
    int n = 0;
    
    for (int i = 0; i < 8; i += 1) {
        if (entry->filename_8[i] == ' ') {
            break;
        }
        if (name[n] != entry->filename_8[i]) {
            return false;
        }
        n += 1;
    }

    if (name[n] != '.') {
        return false;
    }
    n += 1;

    for (int i = 0; i < 3; i += 1) {
        if (entry->filename_3[i] == ' ') {
            break;
        }
        if (name[n] != entry->filename_3[i]) {
            return false;
        }
        n += 1;
    }

    if (name[n] != 0) {
        return false;
    }
    return true;
}

static fat16_directory_entry traverse(fs_fat16* f16, bdev* directory, fs_path* path) {
    ASSERT(f16->header.bytes_per_sector == 512);
    ASSERT(directory->block_size == 512);

    for (uint32_t i = 0; i < bdev_block_count(directory); i++) {
        fat16_directory_entry dirs[DIRS_PER_SECTOR];
        bdev_read(directory, i, 1, &(dirs[0]));
        
        for (int j = 0; j < DIRS_PER_SECTOR; j++) {
            if (dirs[j].filename_8[0] == 0) {
                ASSERT(0); // TODO: entry not found
            }

            if (name_matches(path->part, &(dirs[j]))) {
                ASSERT(path->next == 0); // TODO: recursive..

                return dirs[j];
            }
        }
    }
    ASSERT(0);
}

static uint16_t next_cluster(fs_fat16* f16, uint16_t cluster_index) {
    ASSERT( f16->header.bytes_per_sector == 512 );
    ASSERT( cluster_index < data_clusters(f16) );

    uint32_t sector = first_fat_sector(f16) + (((uint32_t)cluster_index) * 2) / f16->header.bytes_per_sector;
    uint32_t offset = ((uint32_t)cluster_index) % (f16->header.bytes_per_sector / 2);
    uint16_t sector_data[512 / 2];
    bdev_read(f16->dev, sector, 1, sector_data);

    uint16_t result = sector_data[offset];
    return result;
}

static list* cluster_chain_to_blocks(fs_fat16* f16, uint16_t start_cluster) {
    list* result = list_create();

    uint16_t cluster_index = start_cluster;
    while (1) {
        ASSERT(cluster_index != 0); // reserved (index means "free")
        ASSERT(cluster_index != 1); // reserved (future use)
        ASSERT(cluster_index != 0xFFF7); // bad cluster
        if (cluster_index >= 0xFFF8) {
            break;
        }
        ASSERT( cluster_index < data_clusters(f16) );

        uint32_t start_sector_of_cluster = first_data_sector(f16) + (cluster_index - 2) * f16->header.sectors_per_cluster;
        for (int s = 0; s < f16->header.sectors_per_cluster; s++) {
            uint32_t sector = start_sector_of_cluster + s;
            list_append(result, sector);
        }

        cluster_index = next_cluster(f16, cluster_index);
    }

    return result;
}


static bdev* fat16_open_file(filesystem* fs, fs_path* path) {
    ASSERT(fs->vtable == &fat16_vtable);
    ASSERT(path != 0);
    fs_fat16* f16 = (fs_fat16*)(fs);

    uint32_t start = root_dir_sector_start(f16);
    uint32_t count = root_dir_sector_count(f16);
    list* root_dir_sector_list = list_create();
    for (uint32_t s = start; s < start + count; s += 1) {
        list_append(root_dir_sector_list, s);
    }

    bdev* directory = bdev_slice_create(f16->dev, root_dir_sector_list, count * f16->header.bytes_per_sector);
    fat16_directory_entry entry = traverse(f16, directory, path);
    bdev_close(directory);

    dprint_str("File length: ");
    dprint_dec(entry.file_length);
    dprint_str("\n");

    ASSERT(entry.first_cluster_high == 0);
    list* file_blocks = cluster_chain_to_blocks(f16, entry.first_cluster_low);

    return bdev_slice_create(f16->dev, file_blocks, entry.file_length);
}

static void fat16_close(filesystem* fs) {
    ASSERT(fs->vtable == &fat16_vtable);
    fs_fat16* f16 = (fs_fat16*)(fs);
    kfree(f16);
}


filesystem* fat16_open(bdev* dev) {
    ASSERT(dev != 0);
    ASSERT(dev->block_size == 512);
    ASSERT(sizeof(fat16_header) == 62);
    ASSERT(sizeof(fat16_directory_entry) == 32);

    fs_fat16* result = kcalloc(sizeof(fs_fat16));
    result->base.vtable = &fat16_vtable;
    result->dev = dev;

    uint8_t header[512] = {0};;
    bdev_read(dev, 0, 1, header);
    memcpy(header, &(result->header), sizeof(fat16_header));
    ASSERT(dev->block_size == result->header.bytes_per_sector);

    dprint_str("Opened FAT16 filesystem, total sectors: ");
    dprint_dec(total_sectors(result));
    dprint_str("\n");

    return &(result->base);
}
