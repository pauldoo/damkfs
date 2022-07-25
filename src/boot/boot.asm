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

.load_protected:
  mov si, message.jumping
  call println
  cli
  lgdt[gdt_descriptor]
  mov eax, cr0
  or eax, 0x01
  mov cr0, eax
  jmp CODE_SEG:load32

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
   

[BITS 32]
load32:
  mov eax, 1 ; start sector
  mov ecx, 100 ; number of sectors to read
  mov edi, 0x00100000 ; location in ram to read into
  call ata_lba_read
  jmp CODE_SEG:0x00100000

; https://wiki.osdev.org/ATA_read/write_sectors#Read_in_LBA_mode
ata_lba_read:
  mov ebx, eax ; backup the LBA
  
  ; WIP..
  ; https://www.udemy.com/course/developing-a-multithreaded-kernel-from-scratch/learn/lecture/23972412
  ; at 25 minutes.

  ; Is the order of doing these port writes important?
  ; why send the highest byte first?

  ; Send 4th byte of the LBA ..
  mov eax, ebx
  shr eax, 24
  or eax, 0xE0 ; Select master drive
  mov dx, 0x01F6
  out dx, al

  ; Send total number of sectors to read
  mov eax, ecx
  mov dx, 0x01F2
  out dx, al

  ; Send 1st byte of LBA ..
  mov eax, ebx
  mov dx, 0X01F3
  out dx, al

  ; Send 2nd byte of the LBA ..
  mov eax, ebx
  shr eax, 8
  mov dx, 0x01F4
  out dx, al

  ; Send 3rd byte of the LBA ..
  mov eax, ebx
  shr eax, 16
  mov dx, 0x01F5
  out dx, al

  ; Initiate read
  mov dx, 0x01F7
  mov al, 0x20
  out dx, al

  ; Read all sectors
.next_sector:
  push ecx ; save number of sectors to read

  ; Check if we need to read
.try_again:
  mov dx, 0x01F7
  in al, dx
  test al, 8
  ;hlt ; shoud not be here?
  jz .try_again

  ; Read sector (256 words)
  mov ecx, 256
  mov dx, 0x01F0
  rep insw

  ; restore number of sectors to read, and loop
  pop ecx
  loop .next_sector

  ret


message:
.hello:
  db 'Hello!', 0
.jumping:
  db 'Jumping to protected.', 0
.crlf:
  db 0xD, 0xA, 0x0

times 510-($ - $$) db 0x00
dw 0xAA55

