#include "terminal.h"

const int terminal_width = 80;
const int terminal_height = 25;

static int terminal_x = 0;
static int terminal_y = 0;

static volatile uint16_t* const video_mem = (uint16_t*)(0xB8000);

static uint16_t make_char(char c, enum terminal_color fg, enum terminal_color bg) {
    return c | (bg << 12) | fg << 8;
}


void terminal_put_char(int x, int y, char c, enum terminal_color fg, enum terminal_color bg) {
    if (0 <= x && x < terminal_width &&
        0 <= y && y < terminal_height) {

        video_mem[x + y * terminal_width] = make_char(c, fg, bg);
    }
}

void terminal_clear(enum terminal_color bg) {
    for (int y = 0; y < terminal_height; y++) {
        for (int x = 0; x < terminal_width; x++) {
            terminal_put_char(x, y, ' ', White, bg);
        }
    }
    terminal_x = 0;
    terminal_y = 0;
}

void terminal_print(const char* s, enum terminal_color fg, enum terminal_color bg) {
    while (*s != 0) {
        if (*s != '\n') {
            terminal_put_char(terminal_x, terminal_y, *s, fg, bg);
            terminal_x++;
        }

        if (*s == '\n' || terminal_x >= terminal_width) {
            terminal_x = 0;

            terminal_y++;
            if (terminal_y >= terminal_height) {
                terminal_y = 0;
            }
        }

        s++;
    }
}

static void print_failure(const char* label, const char* expr, const char* file, int line, const char* func) {
    terminal_print("\n", White, Red);
    terminal_print(label, White, Red);
    terminal_print(": ", White, Red);
    terminal_print(expr, White, Red);
    terminal_print(" failed\n", White, Red);
    terminal_print(func, White, Red);
    terminal_print(" (", White, Red);
    terminal_print(file, White, Red);
    terminal_print(":", White, Red);
    terminal_print_dec(line, White, Red);
    terminal_print(")\n", White, Red);
}

void halt() {
    while (1) {
        asm("hlt");
    }
}

void __prefer_fail(const char* expr, const char* file, int line, const char* func) {
    print_failure("WARNING", expr, file, line, func);
}

void __assert_fail(const char* expr, const char* file, int line, const char* func) {
    print_failure("ERROR", expr, file, line, func);
    halt();
}


void terminal_print_dec(uint32_t num, enum terminal_color fg, enum terminal_color bg) {
    if (num == 0) {
        terminal_print("0", fg, bg);
        return;
    }
    if (num / 10 != 0) {
        terminal_print_dec(num / 10, fg, bg);
    }
    char str[2];
    str[0] = '0' + (num % 10);
    str[1] = 0;
    terminal_print(str, fg, bg);
}

void terminal_print_hex_imp(uint32_t num, enum terminal_color fg, enum terminal_color bg) {
    if (num == 0) {
        terminal_print("0", fg, bg);
        return;
    }
    if (num / 16 != 0) {
        terminal_print_hex_imp(num / 16, fg, bg);
    }
    char str[2];
    const int digit = num % 16;
    str[0] = (digit < 10) ? ('0' + digit) : ('a' + (digit - 10));
    str[1] = 0;
    terminal_print(str, fg, bg);
}

void terminal_print_hex(uint32_t num, enum terminal_color fg, enum terminal_color bg) {
    terminal_print("0x", fg, bg);
    terminal_print_hex_imp(num, fg, bg);
}

