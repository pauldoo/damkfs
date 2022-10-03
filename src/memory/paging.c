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

static struct page_directory* current_page_directory = 0;


// from paging.asm
void paging_load_directory(struct page_directory* page_directory);

static struct page_table* make_page_table() {
    ASSERT(sizeof(struct page_directory) == 4096);
    struct page_table* const result = page_calloc(1);
    ASSERT((((uint32_t)result) & address_mask) == ((uint32_t)result));

    for (int i = 0; i < PAGING_ENTRIES_PER_TABLE; i++) {
        uint32_t entry = 0;
        result->entry[i] = entry;
    }    

    return result;
}

struct page_directory* page_directory_create() {
    ASSERT(sizeof(struct page_directory) == 4096);
    struct page_directory* const result = page_calloc(1);
    ASSERT((((uint32_t)result) & address_mask) == ((uint32_t)result));

    for (int i = 0; i < PAGING_ENTRIES_PER_TABLE; i++) {
        struct page_table* page_table = make_page_table();
        uint32_t entry = (uint32_t)(page_table) | paging_is_writeable;
        result->entry[i] = entry;
    }
    return result;
}

void page_directory_populate_linear(struct page_directory* page_directory, uint16_t flags) {
    ASSERT( (flags & flag_mask) == flags ); // Only lowest 12 bits can be set.

    for (int i = 0; i < PAGING_ENTRIES_PER_TABLE; i++) {
        struct page_table* pt = (struct page_table*)(page_directory->entry[i] & address_mask);
        ASSERT(pt != NULL);
        for (int j = 0; j < PAGING_ENTRIES_PER_TABLE; j++) {
            const int page_number = i * PAGING_ENTRIES_PER_TABLE + j;
            pt->entry[j] = (((uint32_t)page_number) * 4096) | flags;
        }
        page_directory->entry[i] = ((uint32_t)pt) | flags | paging_is_writeable;
    }
}

void page_directory_switch(struct page_directory* page_directory)
{
    paging_load_directory(page_directory);
    current_page_directory = page_directory;
}

