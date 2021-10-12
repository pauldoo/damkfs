
org 100h
            
mov [0xff], 0x30

mov ax, 0x300
mov ds, ax            
mov [0xff], 0x40

; nonsense jump, to another segment.
jmp 0x7c0:0xff

ret




