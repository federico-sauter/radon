; Copyright (C) 2024 by Federico Sauter <federicosauter@pm.me>. All rights reserved.

%include "ia32macros.asm"

[BITS 32]

;==============================================================================
;==============================================================================
ENTRY ia32_remap_irq
    mov al, 0x11
    out 0x20, al
    mov al, 0x11
    out 0xA0, al
    mov al, 0x20
    out 0x21, al
    mov al, 0x28
    out 0xA1, al
    mov al, 0x04
    out 0x21, al
    mov al, 0x02
    out 0xA1, al
    mov al, 0x01
    out 0x21, al
    mov al, 0x01
    out 0xA1, al
    mov al, 0x00
    out 0x21, al
    mov al, 0x00
    out 0xA1, al
    ret

;==============================================================================
;==============================================================================
extern sv_handle_keybd_irq
ENTRY sv_irq_handle_irq1
    cli
    SAVE_CTX
    mov eax, sv_handle_keybd_irq
    call eax
    RETR_CTX
    iret

;==============================================================================
;==============================================================================
extern sv_idt_handle_irq_glbl
ENTRY sv_irq_handle_irq2
    cli
    SAVE_CTX
    mov ebx, 0
    push ebx
    push ebx
    mov eax, sv_idt_handle_irq_glbl
    call eax
    RETR_CTX
    iret

;==============================================================================
;==============================================================================
extern sv_idt_handle_irq_glbl
ENTRY sv_irq_handle_irq3
    cli
    SAVE_CTX
    mov ebx, 0
    push ebx
    push ebx
    mov eax, sv_idt_handle_irq_glbl
    call eax
    RETR_CTX
    iret

;==============================================================================
;==============================================================================
extern sv_idt_handle_irq_glbl
ENTRY sv_irq_handle_irq4
    cli
    SAVE_CTX
    mov ebx, 0
    push ebx
    push ebx
    mov eax, sv_idt_handle_irq_glbl
    call eax
    RETR_CTX
    iret

;==============================================================================
;==============================================================================
extern sv_idt_handle_irq_glbl
ENTRY sv_irq_handle_irq5
    cli
    SAVE_CTX
    mov ebx, 0
    push ebx
    push ebx
    mov eax, sv_idt_handle_irq_glbl
    call eax
    RETR_CTX
    iret

;==============================================================================
;==============================================================================
extern sv_idt_handle_irq_glbl
ENTRY sv_irq_handle_irq6
    cli
    SAVE_CTX
    mov ebx, 0
    push ebx
    push ebx
    mov eax, sv_idt_handle_irq_glbl
    call eax
    RETR_CTX
    iret

;==============================================================================
;==============================================================================
extern sv_idt_handle_irq_glbl
ENTRY sv_irq_handle_irq7
    cli
    SAVE_CTX
    mov ebx, 0
    push ebx
    push ebx
    mov eax, sv_idt_handle_irq_glbl
    call eax
    RETR_CTX
    iret

;==============================================================================
;==============================================================================
extern sv_idt_handle_irq_glbl
ENTRY sv_irq_handle_irq8
    cli
    SAVE_CTX
    mov ebx, 0
    push ebx
    push ebx
    mov eax, sv_idt_handle_irq_glbl
    call eax
    RETR_CTX
    iret

;==============================================================================
;==============================================================================
extern sv_idt_handle_irq_glbl
ENTRY sv_irq_handle_irq9
    cli
    SAVE_CTX
    mov ebx, 0
    push ebx
    push ebx
    mov eax, sv_idt_handle_irq_glbl
    call eax
    RETR_CTX
    iret

;==============================================================================
;==============================================================================
extern sv_idt_handle_irq_glbl
ENTRY sv_irq_handle_irq10
    cli
    SAVE_CTX
    mov ebx, 0
    push ebx
    push ebx
    mov eax, sv_idt_handle_irq_glbl
    call eax
    RETR_CTX
    iret

;==============================================================================
;==============================================================================
extern sv_idt_handle_irq_glbl
ENTRY sv_irq_handle_irq11
    cli
    SAVE_CTX
    mov ebx, 0
    push ebx
    push ebx
    mov eax, sv_idt_handle_irq_glbl
    call eax
    RETR_CTX
    iret

;==============================================================================
;==============================================================================
extern sv_idt_handle_irq_glbl
ENTRY sv_irq_handle_irq12
    cli
    SAVE_CTX
    mov ebx, 0
    push ebx
    push ebx
    mov eax, sv_idt_handle_irq_glbl
    call eax
    RETR_CTX
    iret

;==============================================================================
;==============================================================================
extern sv_idt_handle_irq_glbl
ENTRY sv_irq_handle_irq13
    cli
    SAVE_CTX
    mov ebx, 0
    push ebx
    push ebx
    mov eax, sv_idt_handle_irq_glbl
    call eax
    RETR_CTX
    iret

;==============================================================================
;==============================================================================
extern sv_idt_handle_irq_glbl
ENTRY sv_irq_handle_irq14
    cli
    SAVE_CTX
    mov ebx, 0
    push ebx
    push ebx
    mov eax, sv_idt_handle_irq_glbl
    call eax
    RETR_CTX
    iret

;==============================================================================
;==============================================================================
extern sv_idt_handle_irq_glbl
ENTRY sv_irq_handle_irq15
    cli
    SAVE_CTX
    mov ebx, 0
    push ebx
    push ebx
    mov eax, sv_idt_handle_irq_glbl
    call eax
    RETR_CTX
    iret

;==============================================================================
; The return code is stored in
;==============================================================================
extern _ecx, _edx
extern sv_syscall_dispatch
extern syscall_rc
ENTRY sv_irq_handle_syscall_irq
    SAVE_CTX
    cli
    cld

    ; system call arguments
    mov eax, [_edx]
    push eax
    mov eax, [_ebx]
    push eax
    mov eax, [_ecx]
    push eax

    call sv_syscall_dispatch
    mov [syscall_rc], eax
    RETR_CTX
    sti
    iret
