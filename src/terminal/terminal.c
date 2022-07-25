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

void terminal_clear() {
    for (int y = 0; y < terminal_height; y++) {
        for (int x = 0; x < terminal_width; x++) {
            terminal_put_char(x, y, ' ', White, Dark_Gray);
        }
    }
    terminal_x = 0;
    terminal_y = 0;
}

void terminal_put_string(int x, int y, const char* s, enum terminal_color fg, enum terminal_color bg) {
    while (*s != 0 && x < terminal_width) {
        terminal_put_char(x, y, *s, fg, bg);
        x++;
        s++;
    }
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