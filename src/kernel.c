
#include "kernel.h"

#include "terminal/terminal.h"
#include "idt/idt.h"
#include "memory/memory.h"
#include "io/io.h"
#include "memory/paging.h"
#include "disk/disk.h"
#include "fs/pparser.h"
#include "fs/stream.h"

static void excercise_heap() {
    int* a = kmalloc(4);
    int* b = kmalloc(4);
    *a = 'A';
    *b = 'B';

    if (*a == 'A' && *b == 'B') {
        dprint_str("success?!\n");
    } else {
        dprint_str("no success?!\n");
    }

    kfree(a);
    a = NULL;

    a = kmalloc(100);
    *a = 'C';
    if (*a == 'C' && *b == 'B') {
        dprint_str("success2?!\n");
    } else {
        dprint_str("no success?!\n");
    }
    kfree(b);
    b = NULL;

    int *c = kmalloc(4);
    *c = 'K';
    if (*a == 'C' && *c == 'K') {
        dprint_str("success3?!\n");
    } else {
        dprint_str("no success?!\n");
    }


    dprint_str("End.\n");    
}

static page_directory* kernel_pd = 0;

void kernel_main() {
    // Hello
    dprint_clear(); 
    dprint_str("Hello world!\n");

    // Heaps
    initialize_heaps();
    dprint_str("Heaps initialized.\n");

    // Enable interrupts
    idt_init();

    // Paging
    kernel_pd = page_directory_create();
    dprint_str("Kernel page directory allocated.\n");
    page_directory_populate_linear(kernel_pd, paging_is_present | paging_is_writeable | paging_access_from_all);
    dprint_str("Kernel page directory populated.\n");
    page_directory_switch(kernel_pd);
    dprint_str("Kernel page directory selected.\n");
    enable_paging();
    dprint_str("Paging enabled.\n");




    // Test
    excercise_heap();

    uint8_t buf[512] = {0};
    bdev_read(default_disk, 1, 1, buf);
    ASSERT(default_disk->block_size == 512);

    dprint_str("A:\n");
    dprint_hex(buf[0]);
    dprint_str("\n");
    dprint_hex(buf[1]);
    dprint_str("\n");
    dprint_hex(buf[2]);
    dprint_str("\n");
    dprint_hex(buf[3]);
    dprint_str("\n");

    // Disk stream test
    istream* stream = istream_buffer(istream_bdev(default_disk));
    istream_seek(stream, 512);
    uint8_t small_buf[4] = {0};
    istream_read(stream, 4, small_buf);
    istream_free(stream);
    dprint_str("B:\n");
    dprint_hex(small_buf[0]);
    dprint_str("\n");
    dprint_hex(small_buf[1]);
    dprint_str("\n");
    dprint_hex(small_buf[2]);
    dprint_str("\n");
    dprint_hex(small_buf[3]);
    dprint_str("\n");

    // Path parsing test
    fs_path* const path = path_parse("/flim/flam/flom");
    for (fs_path* p = path; p != 0; p = p->next) {
        dprint_str("'");
        dprint_str(p->part);
        dprint_str("'");
        dprint_str("\n");
    }
    path_free(path);

    // Fin.
    dprint_str("Finished.\n");
    halt();
}
