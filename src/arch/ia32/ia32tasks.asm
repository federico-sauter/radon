; Copyright (C) 2024 by Federico Sauter <federicosauter@pm.me>. All rights reserved.

%include "ia32macros.asm"

[BITS 32]

;==============================================================================
; Utility: set up the programmable interval timer (PIT)
;==============================================================================
ENTRY ia32_setup_pli
    xor eax, eax
    mov al, 0x36
    out 0x43, al

    xor eax, eax
    mov ax, 11931   ; 100Hz, see util/pit-freq-util.c for more info
    out 0x40, al
    mov al, ah
    out 0x40, al
    ret

;==============================================================================
; Handles the timer interrupt and calls the scheduler
;==============================================================================
extern sv_timer_handler
ENTRY sv_irq_handle_irq0
    SAVE_CTX
    cli             ; clear interrupts, as the kernel is not re-entrant
    cld

    ; notify the PIC that the request has been serviced
    mov al, 0x20
    out 0x20, al

    ; call scheduler
    mov eax, sv_timer_handler
    call eax

    ; change the current stack pointer and pop task context
    RETR_CTX
    sti             ; enable interrupts
    iret

;==============================================================================
; Supervisor idle task - run when no other task is set to run
;==============================================================================
ENTRY ia32_idle_task
    sti
    nop
loop:
    nop
    hlt
    jmp loop
