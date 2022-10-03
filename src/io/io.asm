[BITS 32]
section .asm

global in_b
global in_w
global out_b
global out_w

;uint8_t in_b(uint16_t port);
in_b:
    push ebp
    mov ebp, esp

    xor eax, eax
    mov edx, [ebp+8]
    in al, dx

    pop ebp
    ret

;uint16_t in_w(uint16_t port);
in_w:
    push ebp
    mov ebp, esp

    xor eax, eax
    mov edx, [ebp+8]
    in ax, dx

    pop ebp
    ret

;void out_b(uint16_t port, uint8_t value);
out_b:
    push ebp
    mov ebp, esp

    mov eax, [ebp+12]
    mov edx, [ebp+8]
    out dx, al

    pop ebp
    ret

;void out_w(uint16_t port, uint16_t value);
out_w:
    push ebp
    mov ebp, esp

    mov eax, [ebp+12]
    mov edx, [ebp+8]
    out dx, ax

    pop ebp
    ret