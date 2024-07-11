/* Copyright (C) 2024 by Federico Sauter <federicosauter@pm.me>. All rights reserved. */

// TODO: rename this functions to be architecture-specific

#include <arch/svha.h>
#include <sys/ccio.h>

extern volatile unsigned _eax, _ecx, _edx, _ebx;
extern volatile unsigned _ebp, _esi, _edi, _esp;
extern volatile unsigned _eip, _eflags;

void sv_dump_reg_state()
{
    cc_printf("================== SAVED REGISTER STATE ======================\n"
              "EAX=%x  ECX=%x  EDX=%x  EBX=%x\n"
              "EBP=%x  ESI=%x  EDI=%x  ESP=%x\n"
              "EIP=%x  EFLAGS=%x\n"
              "==============================================================",
              _eax,
              _ecx,
              _edx,
              _ebx,
              _ebp,
              _esi,
              _edi,
              _esp,
              _eip,
              _eflags);
}

void sv_panic(const char* msg)
{
    cc_cpos_set(0, 18);
    cc_printf("==============================================================\n");
    cc_printf(msg);
    sv_dump_reg_state();
    sv_get_ha()->halt();
}

void sv_handle_divide_error()
{
    sv_panic("FAULT: divide error\n");
}

void sv_handle_nmi()
{
    sv_panic("FAULT: nmi\n");
}

void sv_handle_debug()
{
    sv_panic("FAULT: debug\n");
}

void sv_handle_breakpoint()
{
    sv_panic("\nTRAP: breakpoint\n");
}

void sv_handle_overflow()
{
    sv_panic("\nTRAP: overflow\n");
}

void sv_handle_bounds()
{
    sv_panic("FAULT: bounds check\n");
}

void sv_handle_invalid_op()
{
    sv_panic("FAULT: invalid opcode\n");
}

void sv_handle_device_not_avaiable()
{
    sv_panic("FAULT: device not available\n");
}

void sv_handle_double_fault()
{
    sv_panic("\nABORT: double fault\n");
}

void sv_handle_coproc_seg_overrun()
{
    sv_panic("\nABORT: coprocessor segment overrun\n");
}

void sv_handle_inv_tss()
{
    sv_panic("FAULT: invalid TSS\n");
}

void sv_handle_seg_not_present()
{
    sv_panic("FAULT: segment not present\n");
}

void sv_handle_ss()
{
    sv_panic("FAULT: stack segment\n");
}

void sv_handle_gp()
{
    sv_panic("FAULT: general protection fault\n");
}

void sv_handle_pf()
{
    sv_panic("FAULT: page fault\n");
}

void sv_handle_fpe()
{
    sv_panic("FAULT: floating point error\n");
}

void sv_handle_alignment_check()
{
    sv_panic("FAULT: alignment check\n");
}

void sv_idt_handle_irq_glbl()
{
    cc_printf("\n*** Interrupt occurred ***\n");
}
