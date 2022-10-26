#include "idt.h"

#include <stdint.h>

#include "terminal/terminal.h"
#include "io/io.h"

#define IDT_TOTAL_INTERRUPTS (256)


// https://wiki.osdev.org/Interrupt_Descriptor_Table

typedef struct idt_desc_t {
  uint16_t offset_1; // offset bits 0..15
  uint16_t selector; // selector in GDT
  uint8_t unused; // should be zero
  uint8_t type_attribute; // descriptor type and attributes
  uint16_t offset_2; // offset bits 16..31
} __attribute__((packed)) idt_desc;

typedef struct idtr_desc_t {
  uint16_t limit; // size of IDT table in bytes minus 1
  uint32_t base; // Address of start of table
} __attribute__((packed)) idtr_desc;



// Constants from kernel.asm
static const uint8_t kernel_code_selector = 0x08;
// temporarily unused:
//static const uint8_t kernel_data_selector = 0x10;

static idt_desc idt_descriptors[IDT_TOTAL_INTERRUPTS] = { 0 };


// from idt.asm
extern void enable_interrupts();

extern void idt_load(const idtr_desc * idtr_descriptor);

extern void int21h();

extern void no_interrupt();


static void idt_set(int interrupt_number, void* address) {
     idt_desc* desc = &(idt_descriptors[interrupt_number]);
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
    dprint_str("Interrupt Zero!\n");
}


static void call_lidt() {
    idtr_desc idtr_descriptor = {
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
    uint8_t c = in_b(0x60);
    dprint_str("Keyboard pressed: ");
    dprint_hex(c);
    dprint_str("\n");
    out_b(0x20, 0x20); // acknowledge
}

void no_interrupt_handler() {
    out_b(0x20, 0x20);
 }