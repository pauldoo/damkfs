#include "paging.h"

#include "terminal/terminal.h"
#include "memory/memory.h"

const uint16_t paging_cache_disabled =
    0b00010000;
const uint16_t paging_write_through =
    0b00001000;
const uint16_t paging_access_from_all =
    0b00000100;
const uint16_t paging_is_writeable =
    0b00000010;
const uint16_t paging_is_present =
    0b00000001;

static const uint32_t address_mask =
    0xfffff000; // Upper 20 bits
static const uint32_t flag_mask =
    0x00000fff; // Lower 12 bits

static page_directory* current_page_directory = 0;


// from paging.asm
void paging_load_directory(page_directory* page_directory);

static page_table* make_page_table() {
    ASSERT(sizeof(page_directory) == 4096);
    page_table* const result = page_calloc(1);
    ASSERT((((uint32_t)result) & address_mask) == ((uint32_t)result));

    for (int i = 0; i < PAGING_ENTRIES_PER_TABLE; i++) {
        uint32_t entry = 0;
        result->entry[i] = entry;
    }    

    return result;
}

page_directory* page_directory_create() {
    ASSERT(sizeof(page_directory) == 4096);
    page_directory* const result = page_calloc(1);
    ASSERT((((uint32_t)result) & address_mask) == ((uint32_t)result));

    for (int i = 0; i < PAGING_ENTRIES_PER_TABLE; i++) {
        page_table* page_table = make_page_table();
        uint32_t entry = (uint32_t)(page_table) | paging_is_writeable;
        result->entry[i] = entry;
    }
    return result;
}

void page_directory_populate_linear(page_directory* page_directory, uint16_t flags) {
    ASSERT( (flags & flag_mask) == flags ); // Only lowest 12 bits can be set.

    for (int i = 0; i < PAGING_ENTRIES_PER_TABLE; i++) {
        page_table* pt = (page_table*)(page_directory->entry[i] & address_mask);
        ASSERT(pt != NULL);
        for (int j = 0; j < PAGING_ENTRIES_PER_TABLE; j++) {
            const int page_number = i * PAGING_ENTRIES_PER_TABLE + j;
            pt->entry[j] = (((uint32_t)page_number) * 4096) | flags;
        }
        page_directory->entry[i] = ((uint32_t)pt) | flags | paging_is_writeable;
    }
}

void page_directory_switch(page_directory* page_directory)
{
    paging_load_directory(page_directory);
    current_page_directory = page_directory;
}

static void page_directory_indexof(void* address, uint32_t* directory_index_out, uint32_t* table_index_out) {
    ASSERT((((uint32_t)address) % 4096) == 0)
    ASSERT(directory_index_out != 0)
    ASSERT(table_index_out != NULL)
    (*table_index_out) = (((uint32_t)address) >> 12) & 0x3ff;  
    (*directory_index_out) = (((uint32_t)address) >> 22) & 0x3ff;
    ASSERT(((uint32_t)address) == (((*directory_index_out)*1024) + (*table_index_out)) * 4096)
}

void page_set(page_directory* page_directory, void* address, uint32_t value) {
    uint32_t dir_index, table_index;
    page_directory_indexof(address, &dir_index, &table_index);
    page_table* pt = (page_table*)((page_directory->entry[dir_index]) & address_mask);
    pt->entry[table_index] = value;
}