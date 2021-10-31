org 0x0
bits 16

_start:
  jmp short start
  nop

bios_parameter_block:
  times 33 db 0x00

start:
  jmp 0x7C0:main

message:
  db 'Hello World!', 0

main:
  cli ; clear interrupts
  mov ax, 0x7C0
  mov ds, ax
  mov es, ax
  mov ax, 0x00
  mov ss, ax
  mov sp, 0x7C00
  sti ; enables interrupts

  ; Example code for setting up interrupt handlers
  ;mov word[ss:0x00], handle_zero
  ;mov word[ss:0x02], 0x7C0
  ;mov word[ss:0x04], handle_one
  ;mov word[ss:0x06], 0x7C0

  ; Trigger division by zero
  ;mov ax, 0x00
  ;div ax

  ; Trigger interrupt 1
  ;int 1

  mov si, message
  call print
  call fin

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

handle_zero:
  mov ah, 0x0E
  mov al, 'A'
  mov bx, 0x00
  int 0x10
  iret

handle_one:
  mov ah, 0x0E
  mov al, 'V'
  mov bx, 0x00
  int 0x10
  iret


times 510-($ - $$) db 0x00
dw 0xAA55
