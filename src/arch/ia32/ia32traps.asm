; Copyright (C) 2024 by Federico Sauter <federicosauter@pm.me>. All rights reserved.

%include "ia32macros.asm"

[BITS 32]


;==============================================================================
;==============================================================================
extern sv_handle_divide_error
ENTRY sv_idt_handle_e0
    cli
    SAVE_CTX
    mov eax, sv_handle_divide_error
    call eax
    RETR_CTX
    iret

;==============================================================================
;==============================================================================
extern sv_handle_debug
ENTRY sv_idt_handle_e1
    cli
    SAVE_CTX
    mov eax, sv_handle_debug
    call eax
    RETR_CTX
    iret

;==============================================================================
;==============================================================================
extern sv_handle_nmi
ENTRY sv_idt_handle_e2
    cli
    SAVE_CTX
    mov eax, sv_handle_nmi
    call eax
    RETR_CTX
    iret

;==============================================================================
;==============================================================================
extern sv_handle_breakpoint
ENTRY sv_idt_handle_e3
    cli
    SAVE_CTX
    mov eax, sv_handle_breakpoint
    call eax
    RETR_CTX
    iret

;==============================================================================
;==============================================================================
extern sv_handle_overflow
ENTRY sv_idt_handle_e4
    cli
    SAVE_CTX
    mov eax, sv_handle_overflow
    call eax
    RETR_CTX
    iret

;==============================================================================
;==============================================================================
extern sv_handle_bounds
ENTRY sv_idt_handle_e5
    cli
    SAVE_CTX
    mov eax, sv_handle_bounds
    call eax
    RETR_CTX
    iret

;==============================================================================
;==============================================================================
extern sv_handle_invalid_op
ENTRY sv_idt_handle_e6
    cli
    SAVE_CTX
    mov eax, sv_handle_invalid_op
    call eax
    RETR_CTX
    iret

;==============================================================================
;==============================================================================
extern sv_handle_device_not_avaiable
ENTRY sv_idt_handle_e7
    cli
    SAVE_CTX
    mov eax, sv_handle_device_not_avaiable
    call eax
    RETR_CTX
    iret

;==============================================================================
;==============================================================================
extern sv_handle_double_fault
ENTRY sv_idt_handle_e8
    cli
    SAVE_CTX
    mov eax, sv_handle_double_fault
    call eax
    RETR_CTX
    iret

;==============================================================================
;==============================================================================
extern sv_handle_coproc_seg_overrun
ENTRY sv_idt_handle_e9
    cli
    SAVE_CTX
    mov eax, sv_handle_coproc_seg_overrun
    call eax
    RETR_CTX
    iret

;==============================================================================
;==============================================================================
extern sv_handle_inv_tss
ENTRY sv_idt_handle_e10
    cli
    SAVE_CTX
    mov eax, sv_handle_inv_tss
    call eax
    RETR_CTX
    iret

;==============================================================================
;==============================================================================
extern sv_handle_seg_not_present
ENTRY sv_idt_handle_e11
    cli
    SAVE_CTX
    mov eax, sv_handle_seg_not_present
    call eax
    RETR_CTX
    iret

;==============================================================================
;==============================================================================
extern sv_handle_ss
ENTRY sv_idt_handle_e12
    cli
    SAVE_CTX
    mov eax, sv_handle_ss
    call eax
    RETR_CTX
    iret

;==============================================================================
;==============================================================================
extern sv_handle_gp
ENTRY sv_idt_handle_e13
    cli
    SAVE_CTX
    mov eax, sv_handle_gp
    call eax
    RETR_CTX
    iret

;==============================================================================
;==============================================================================
extern sv_handle_pf
ENTRY sv_idt_handle_e14
    cli
    SAVE_CTX
    mov eax, sv_handle_pf
    call eax
    RETR_CTX
    iret

;==============================================================================
;==============================================================================
extern sv_handle_fpe
ENTRY sv_idt_handle_e16
    cli
    SAVE_CTX
    mov eax, sv_handle_fpe
    call eax
    RETR_CTX
    iret

;==============================================================================
;==============================================================================
extern sv_handle_alignment_check
ENTRY sv_idt_handle_e17
    cli
    SAVE_CTX
    mov eax, sv_handle_alignment_check
    call eax
    RETR_CTX
    iret

