
#include "kernel.h"

#include "terminal/terminal.h"
#include "idt/idt.h"
#include "memory/memory.h"
#include "io/io.h"

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

}
