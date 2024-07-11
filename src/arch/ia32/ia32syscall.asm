; Copyright (C) 2024 by Federico Sauter <federicosauter@pm.me>. All rights reserved.

[BITS 32]

%include "ia32macros.asm"

;==============================================================================
;   Performs a system call using the INT 0x80.
;   The calling convention is:
;       ecx -> system call code
;       ebx -> argument0
;       edx -> argument1
;
;   The return value is placed into eax by the IRQ handler
;==============================================================================
extern syscall_rc
ENTRY ia32_do_syscall
    push ebp

    mov ebp, esp
    mov ecx, [ebp + 8]
    mov ebx, [ebp + 12]
    mov edx, [ebp + 16]

    int 0x80

    pop ebp
    mov eax, [syscall_rc]
    ret
