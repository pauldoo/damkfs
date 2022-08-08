#include "idt.h"

#include <stdint.h>

#include "terminal/terminal.h"
#include "io/io.h"

#define IDT_TOTAL_INTERRUPTS ((int)(256))


// https://wiki.osdev.org/Interrupt_Descriptor_Table

struct idt_desc {
  uint16_t offset_1; // offset bits 0..15
  uint16_t selector; // selector in GDT
  uint8_t unused; // should be zero
  uint8_t type_attribute; // descriptor type and attributes
  uint16_t offset_2; // offset bits 16..31
} __attribute__((packed));

struct idtr_desc {
  uint16_t limit; // size of IDT table in bytes minus 1
  uint32_t base; // Address of start of table
} __attribute__((packed));



// Constants from kernel.asm
static const uint8_t kernel_code_selector = 0x08;
static const uint8_t kernel_data_selector = 0x10;

static struct idt_desc idt_descriptors[IDT_TOTAL_INTERRUPTS] = { 0 };


// from idt.asm
extern void enable_interrupts();

extern void idt_load(const struct idtr_desc * idtr_descriptor);

extern void int21h();

extern void no_interrupt();


static void idt_set(int interrupt_number, void* address) {
     struct idt_desc* desc = &(idt_descriptors[interrupt_number]);
     desc->offset_1 = ((uint32_t)address) & 0x0000ffff;
     desc->offset_2 = ((uint32_t)address) >> 16;

     desc->selector = kernel_code_selector;
     desc->unused = 0;

     // 0xEE = 1110 1110
     // present bit
     // 2 bit DPL
     // unused bit
     // gate type (1110 -> 32-bit interrupt gate)
     desc->type_attribute = 0xEE;
}

static void idt_zero() {
    terminal_print("Interrupt Zero!\n", White, Black);
}


static void call_lidt() {
    struct idtr_desc idtr_descriptor = {
        .base = (uint32_t)idt_descriptors,
        .limit = sizeof(idt_descriptors) - 1
    };

    idt_load(&idtr_descriptor);
    // Can alternatively be performed with inline asm:
    //__asm__ ("lidt %0" :: "m"(idtr_descriptor));
}

void idt_init() {
    for (int i = 0; i < IDT_TOTAL_INTERRUPTS; i++) {
        idt_set(i, no_interrupt);
    }

    idt_set(0, idt_zero); // needs a proper handler using "iret"
    idt_set(0x21, int21h);

    call_lidt();
    enable_interrupts();
}

void int21h_handler() {
    terminal_print("Keyboard pressed!\n", Magenta, Black);
    out_b(0x20, 0x20);
}

void no_interrupt_handler() {
    out_b(0x20, 0x20);
 }