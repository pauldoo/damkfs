[bits 32]

global _start
extern kernel_main

CODE_SEG equ 0x08
DATA_SEG equ 0x10 

_start:
  mov ax, DATA_SEG
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax
  mov ebp, 0x00200000
  mov esp, ebp

  ; Enable A20 line
  in al, 0x92
  or al, 0x02
  out 0x92, al

  ; remap master PIC
  mov al, 00010001b
  out 0x20, al
  mov al, 0x20
  out 0x21, al
  mov al, 00000001b
  out 0x21, al

  sti ; a tiny bit too early, as IDT has yet to be initialized, but addressed later in course.

  call kernel_main

.loop:
  hlt
  jmp .loop


; kernel.asm file is first in the kernel image, so pad to ensure alignment with following C code.
; other asm files are linked to the end of the kernel image so don't need this.
; IOW, kernel.asm is in the .text section, not .asm.
times 512-($ - $$) db 0x00

