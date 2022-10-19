#pragma once
#include <stdint.h>

#define PAGING_ENTRIES_PER_TABLE (1024)

extern const uint16_t paging_cache_disabled;
extern const uint16_t paging_write_through;
extern const uint16_t paging_access_from_all;
extern const uint16_t paging_is_writeable;
extern const uint16_t paging_is_present;

typedef struct page_directory_t {
    uint32_t entry[PAGING_ENTRIES_PER_TABLE];
} page_directory;

typedef struct page_table_t {
    uint32_t entry[PAGING_ENTRIES_PER_TABLE];
} page_table;

page_directory* page_directory_create();

void page_directory_populate_linear(page_directory *page_directory, uint16_t flags);

void page_directory_switch(page_directory *page_directory);

// kernel.asm
void enable_paging();

