
#include "kernel.h"

#include "terminal/terminal.h"
#include "idt/idt.h"
#include "memory/memory.h"
#include "io/io.h"

static void excercise_heap() {
    int* a = kmalloc(4);
    int* b = kmalloc(4);
    *a = 'A';
    *b = 'B';

    if (*a == 'A' && *b == 'B') {
        terminal_print("success?!\n", White, Black);
    } else {
        terminal_print("no success?!\n", White, Black);
    }

    kfree(a);
    a = NULL;

    a = kmalloc(100);
    *a = 'C';
    if (*a == 'C' && *b == 'B') {
        terminal_print("success2?!\n", White, Black);
    } else {
        terminal_print("no success?!\n", White, Black);
    }
    kfree(b);
    b = NULL;

    int *c = kmalloc(4);
    *c = 'K';
    if (*a == 'C' && *c == 'K') {
        terminal_print("success3?!\n", White, Black);
    } else {
        terminal_print("no success?!\n", White, Black);
    }


    terminal_print("End.\n", White, Black);    
}

void kernel_main() {
    terminal_clear();

    const char* const message = "Hello World!!";
    terminal_put_string(
        (terminal_width - strlen(message)) / 2,
        terminal_height / 2,
        message,
        Yellow,
        Blue);
    terminal_print("Hello\nWorld!\n", Light_Green, Blue);

    idt_init();

    initialize_heap();

    excercise_heap();
}
