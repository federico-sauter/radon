; Copyright (C) 2024 by Federico Sauter <federicosauter@pm.me>. All rights reserved.

%include "ia32macros.asm"

[BITS 32]

;==============================================================================
; constants
;==============================================================================
; This is the size for the initial stack for the supervisor process (64KB)
STACKSIZE equ 65536

;==============================================================================
; multiboot header
;==============================================================================
FLAGS       equ  0x00
MAGIC       equ  0x1BADB002
CHECKSUM    equ -(MAGIC + FLAGS)

section .text
ALIGN 4
mbheader:
   dd MAGIC
   dd FLAGS
   dd CHECKSUM

;==============================================================================
; This is the entry point to the supervisor, the first function called upon
; booting up.
; Initializes the low-level hardware settings and then calls the sv_init
; to finish setting up the scheduler and other higher-level data structures.
; Finally, control is transfered to the idle task.
;==============================================================================
extern sv_init
extern ia32_video_clear
extern ia32_sysinit
extern ia32_idle_task

ENTRY sv_startup
    mov  esp, sv_stack              ; set up the stack

    push dword 0                    ; reset EFLAGS
    popf

    push eax                        ; multiboot information
    push ebx                        ; multibook magic number

    cli                             ; disable interrupts during initialization
                                    ; interrupts are reenabled in sv_task()

    call ia32_video_clear           ; clear the screen

    call ia32_sysinit               ; general system initialization

    ; hardware setup finished - initialize the high-level supervisor state
    call sv_init

    mov ax, SV_DATA_SEG_SEL
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov ebp, esp
    push SV_CODE_SEG_SEL
    push dword ia32_idle_task

    retf

;==============================================================================
; load the GDT register from the location at gdt_ptr (external variable)
;==============================================================================
extern gdt_ptr
ENTRY ia32_gdt_load_gdtreg
    lgdt [gdt_ptr]

    mov ax, SV_DATA_SEG_SEL  ; set all segment registers
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    jmp SV_CODE_SEG_SEL:r0  ; far jump to set CS to the new code segment selector
r0:
    ret

;==============================================================================
; load the IDT register from the location at idt_ptr
;==============================================================================
extern idt_ptr
ENTRY ia32_idt_load_idtreg
    lidt [idt_ptr]
    ret

;==============================================================================
; Halts the system
;==============================================================================
ENTRY ia32_halt
    cli
loop0:
    hlt
    jmp loop0

;==============================================================================
; supervisor stack
;==============================================================================
SECTION .bss
align 4
   resb STACKSIZE
sv_stack:

