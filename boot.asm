org 0x0
bits 16

_start:
  jmp short start
  nop

bios_parameter_block:
  times 33 db 0x00

start:
  jmp 0x7C0:main

message_start:
  db 'Reading 2nd sector of disk.', 0

message_read_failed:
  db 'Failed to read sector.', 0

main:
  cli ; clear interrupts
  mov ax, 0x7C0
  mov ds, ax
  mov es, ax
  mov ax, 0x00
  mov ss, ax
  mov sp, 0x7C00
  sti ; enables interrupts

  call read_and_print_second_sector
  call fin

read_and_print_second_sector:
  mov si, message_start
  call print

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
.okay
  mov si, buffer
  call print
  jmp .end
.error
  mov si, message_read_failed
  call print
  jmp .end

.end
  ret

print:
  mov bx, 0
.loop:
  lodsb
  cmp al, 0
  je .done
  call print_char
  jmp .loop
.done:
  ret
   
print_char:
  mov ah, 0x0E
  int 0x10
  ret

fin:
.loop:
  hlt
  jmp .loop


times 510-($ - $$) db 0x00
dw 0xAA55

buffer:
