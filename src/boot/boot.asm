org 0x7C00
bits 16

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

_start:
  ; jump over the bios parameter block
  jmp short start
  nop

bios_parameter_block:
  times 33 db 0x90

start:
  ; long jump sets code segment register
  jmp 0x0:main

main:
  ; Initalize segment registers (excluding code segment already set by long jump)
  cli ; clear interrupts
  mov ax, 0x0000
  mov ds, ax
  mov es, ax
  mov ss, ax
  mov sp, 0x7C00
  sti ; enables interrupts

  mov si, message.hello
  call println

  call read_and_print_second_sector

.load_protected:
  mov si, message.jumping
  call println
  cli
  lgdt[gdt_descriptor]
  mov eax, cr0
  or eax, 0x01
  mov cr0, eax
  jmp CODE_SEG:load32


read_and_print_second_sector:
  mov si, message.start
  call println

  ; http://www.ctyme.com/intr/rb-0607.htm
  mov ah, 0x2 ; "read sector" command
  mov al, 0x1 ; sectors to read
  mov ch, 0x0 ; cylinder number (low eight bits)
  mov cl, 0x2 ; sector index to read (sectors are numbered starting at one)
  mov dh, 0x0 ; head number
  mov bx, buffer
  int 0x13

  jc .error
  jmp .okay
.okay:
  mov si, buffer
  call println
  jmp .end
.error:
  mov si, message.failed
  call println
  jmp .end

.end:
  ret

gdt_start:
gdt_null:
  dd 0x00000000
  dd 0x00000000
gdt_code:       ; CS
  ; offset 0x08
  dw 0xFFFF     ; segment limit (bits 0 to 15)
  dw 0x0000     ; base (bits 0 to 15)
  db 0x00       ; base (bits 16 to 23)
  db 10011010b  ; access byte
  db 11001111b  ; flags : bits 16 to 19 of segment limit
  db 0x00       ; base (bits 24 to 31)
gdt_data:       ; DS, SS, ES, FS, GS
  ; offset 0x10
  dw 0xFFFF     ; segment limit (bits 0 to 15)
  dw 0x0000     ; base (bits 0 to 15)
  db 0x00       ; base (bits 16 to 23)
  db 10010010b  ; access byte
  db 11001111b  ; flags : bits 16 to 19 of segment limit
  db 0x00       ; base (bits 24 to 31)
gdt_end:

gdt_descriptor:
  dw gdt_end - gdt_start - 1
  dd gdt_start


println:
  call print
  mov si, message.crlf
  call print
  ret

print:
  mov bx, 0
  mov ah, 0x0E
.loop:
  lodsb
  cmp al, 0
  je .done
  int 0x10
  jmp .loop
.done:
  ret
   
fin:
.loop:
  hlt
  jmp .loop

message:
.hello:
  db 'Hello!', 0
.start:
  db 'Reading 2nd sector of disk.', 0
.failed:
  db 'Failed to read sector.', 0
.jumping:
  db 'Jumping to protected.', 0
.crlf:
  db 0xD, 0xA, 0x0

[bits 32]
load32:
  mov ax, DATA_SEG
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax
  mov ebp, 0x00200000
  mov esp, ebp

.loop:
  hlt
  jmp .loop

times 510-($ - $$) db 0x00
dw 0xAA55

buffer:
