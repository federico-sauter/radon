; Copyright (C) 2024 by Federico Sauter <federicosauter@pm.me>. All rights reserved.

;==============================================================================
; Supervisor segment selectors (data and code)
; Use if a reference to one of the segments is required, but only after loading
; the GDT!
;==============================================================================
%define SV_CODE_SEG_SEL 0x08
%define SV_DATA_SEG_SEL 0x10

;==============================================================================
; ENTRY <subroutineName> - defines the entry point for a public subroutine
;==============================================================================
%macro ENTRY 1
global %1
%1:
%endmacro

;==============================================================================
; SAVE_CTX and RETR_CTX save and retrieve the current task's context
;==============================================================================
extern _eax, _ecx, _edx, _ebx   ; defined in svsched.c
extern _ebp, _esi, _edi, _esp
extern _eip, _eflags

extern sv_save_current_task_context

%macro SAVE_CTX 0
    mov [_eax], eax
    mov [_ecx], ecx
    mov [_edx], edx
    mov [_ebx], ebx

    mov [_ebp], ebp
    mov [_esi], esi
    mov [_edi], edi

    pop dword [_eip]
    pop eax         ; code segment is constant (for now)
    pop dword [_eflags]

    mov [_esp], esp

    call sv_save_current_task_context
%endmacro

%macro RETR_CTX 0
    mov eax, [_eax]
    mov ecx, [_ecx]
    mov edx, [_edx]
    mov ebx, [_ebx]

    mov ebp, [_ebp]
    mov esi, [_esi]
    mov edi, [_edi]
    mov esp, [_esp]

    push dword [_eflags]
    push SV_CODE_SEG_SEL
    push dword [_eip]
%endmacro

