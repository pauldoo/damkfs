[BITS 32]
section .asm

extern int21h_handler
extern no_interrupt_handler

global enable_interrupts
global idt_load
global int21h
global no_interrupt

enable_interrupts:
  push ebp
  mov ebp, esp

  sti

  pop ebp
  ret

idt_load:
  push ebp
  mov ebp, esp

  mov ebx, [ebp+8] ; obtain first argument
  lidt [ebx]

  pop ebp
  ret

int21h:
  cli
  pushad

  call int21h_handler

  popad
  sti
  iret


no_interrupt:
  cli
  pushad

  call no_interrupt_handler

  popad
  sti
  iret
