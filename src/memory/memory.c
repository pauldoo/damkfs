#include "memory.h"

#include "terminal/terminal.h"

static void * const heap_base = ((void*)0x01000000); // starts at 16M
static const size_t heap_size = 0x10000000; // is 256M long

enum allocated_state {
    Unallocated = 1,
    Allocated = 2
};

struct heap_entry_header {
    struct heap_entry_header* previous;
    struct heap_entry_header* next;
    enum allocated_state state;
};

// A reference into the heap of where we did the last allocate
// or deallocate.  Saves us always doing a linear scan from
// the start.
static struct heap_entry_header* heap_cursor = heap_base;

static void dp(const char* m, uint32_t v) {
    return; // disabled for now

    terminal_print(m, White, Black);
    terminal_print_num(v, White, Black);
    terminal_print("\n", White, Black);
}

// Validates entire heap - for debug
static void validate_heap() {
    int found_cursor = 0;
    int loop_count = 0;

    const struct heap_entry_header * curr = heap_base;
    do {
        if (curr == heap_cursor) {
            found_cursor = 1;
        }
        if (loop_count++ >= heap_size) {
            panic("heap: broken next links");
        }

        if (curr->previous->next != curr) {
            panic("heap: previous next isn't me");
        }
        if (curr->next->previous != curr) {
            panic("heap: next previous isn't me");
        }

        if (curr != heap_base) {
            if (curr->previous > curr) {
                panic("heap: entries out of order");
            }
            if (curr->previous->state == Unallocated && curr->state == Unallocated) {
                panic("heap: entries should have been merged");
            }
        }

        if (curr->next != heap_base) {
            if( curr->next < curr ) {
                panic("heap: entries out of order");
            }
            if (curr->next->state == Unallocated && curr->state == Unallocated) {
                panic("heap: entries should have been merged");
            }
        }

        curr = curr->next;
    } while (curr != heap_base);

    if (!found_cursor) {
        panic("heap: cursor lost!\n");
    }
}

// number of bytes from the start of this header, to the next
// (or the end of the heap if that's closer)
static size_t entry_size( struct heap_entry_header* a ) {
    if ( a->next > a ) {
        return ((uint8_t*)(a->next)) - ((uint8_t*)(a));
    } else {
        return ((uint8_t*)heap_base) + heap_size - ((uint8_t*)(a));
    }
}

// Optionally merge a's successor into a, if the conditions are right
static void optionally_merge_with_successor( struct heap_entry_header* a ) {
    struct heap_entry_header* b = a->next;

    if (a->state == Unallocated && b->state == Unallocated && a < b) {
        // First, move the cursor out of harms way
        if (heap_cursor == b) {
            heap_cursor = a;
        }

        struct heap_entry_header* new_next = a->next->next;
        a->next = new_next;
        new_next->previous = a;
    }
}

void memzero(void* p, size_t len) {
    for (size_t i = 0; i < len; i++) {
        ((char*)p)[i] = 0;
    }
}

int strlen(const char* str) {
    int len = 0;
    while (str[len] != 0) {
        len++;
    }
    return len;
}

void initialize_heap() {
    heap_cursor = heap_base;

    heap_cursor->previous = heap_cursor;
    heap_cursor->next = heap_cursor;
    heap_cursor->state = Unallocated;

    validate_heap();
}


void* kmalloc(size_t len) {
    dp("kmalloc len=", len);
    if (len > heap_size) {
        panic("kmalloc: allocation request too large");
    }
    if (len == 0) {
        len = 1;
    }
    len = ((len + 7) / 8) * 8; // round up to multiple of 8 bytes
    const size_t required_entry_size = len + sizeof(struct heap_entry_header);
    dp("kmalloc required_entry_size=", required_entry_size);


    validate_heap();


    struct heap_entry_header* curr = heap_cursor;

    while (1) {
        if (curr->state == Unallocated && entry_size(curr) >= required_entry_size) {
            break;
        }
        curr = curr->next;
        if (curr == heap_cursor) {
            // We have looped without finding space
            panic("kmalloc: no more memory");
        }
    }

    dp("kmalloc found, size=", entry_size(curr));

    curr->state = Allocated;

    if (entry_size(curr) >= (required_entry_size + sizeof(struct heap_entry_header) + 8)) {
        // entry is large enough that it's worth representing the remainder as a new unallocated entry.
        struct heap_entry_header* new_next = (struct heap_entry_header*)( ((uint8_t*)curr) + required_entry_size );

        new_next->state = Unallocated;
        new_next->previous = curr;
        new_next->next = curr->next;

        new_next->previous->next = new_next;
        new_next->next->previous = new_next;
    }
    heap_cursor = curr;

    validate_heap();

    void * const result = ((uint8_t*)curr) + sizeof(struct heap_entry_header);
    dp("kmalloc result=", ((uint32_t)result));

    return result;
}


void kfree(void* p) {
    validate_heap();

    struct heap_entry_header* header = p - sizeof(struct heap_entry_header);
    header->state = Unallocated;
    heap_cursor = header;

    optionally_merge_with_successor( header );
    optionally_merge_with_successor( header->previous );

    validate_heap();
}
