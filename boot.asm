org 0x7C00
bits 16

jmp start
  
message:
  db 'Hello World!', 0
  
start:
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

times 510-($ - $$) db 0
dw 0xAA55
