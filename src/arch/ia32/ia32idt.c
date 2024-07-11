/* Copyright (C) 2024 by Federico Sauter <federicosauter@pm.me>. All rights reserved. */

#include "ds.h"
#include <string.h>

/* the Interrupt Descriptor Table (IDT) */
#define IDT_LEN 256
idt_elem_t idt[IDT_LEN];
load_ptr_t idt_ptr;

extern void sv_idt_handle_e0();
extern void sv_idt_handle_e1();
extern void sv_idt_handle_e2();
extern void sv_idt_handle_e3();
extern void sv_idt_handle_e4();
extern void sv_idt_handle_e5();
extern void sv_idt_handle_e6();
extern void sv_idt_handle_e7();
extern void sv_idt_handle_e8();
extern void sv_idt_handle_e9();
extern void sv_idt_handle_e10();
extern void sv_idt_handle_e11();
extern void sv_idt_handle_e12();
extern void sv_idt_handle_e13();
extern void sv_idt_handle_e14();
extern void sv_idt_handle_e15();
extern void sv_idt_handle_e16();
extern void sv_idt_handle_e17();

extern void sv_irq_handle_irq0();
extern void sv_irq_handle_irq1();
extern void sv_irq_handle_irq2();
extern void sv_irq_handle_irq3();
extern void sv_irq_handle_irq4();
extern void sv_irq_handle_irq5();
extern void sv_irq_handle_irq6();
extern void sv_irq_handle_irq7();
extern void sv_irq_handle_irq8();
extern void sv_irq_handle_irq9();
extern void sv_irq_handle_irq10();
extern void sv_irq_handle_irq11();
extern void sv_irq_handle_irq12();
extern void sv_irq_handle_irq13();
extern void sv_irq_handle_irq14();
extern void sv_irq_handle_irq15();

extern void sv_irq_handle_syscall_irq();

extern void ia32_idt_load_idtreg();

void sv_idt_init_elem(idt_elem_t* idt_elem, unsigned offset, unsigned short selector)
{
    idt_elem->offset_l  = offset & 0xffff;
    idt_elem->offset_h  = (offset >> 16) & 0xffff;
    idt_elem->selector  = selector;
    idt_elem->reserved  = 0;   /* this is always set to zero */
    idt_elem->gate_type = 0xe; /* constant */
    idt_elem->ring      = 0;   /* supervisor ring zero */
    idt_elem->present   = 1;
}

void ia32_idt_init()
{
    /* initialize the IDT with zeros */
    memset(idt, 0, sizeof(idt_elem_t) * IDT_LEN);

    /* set the pointer to the IDT */
    idt_ptr.base  = (unsigned)idt;
    idt_ptr.limit = (sizeof(idt_elem_t) * IDT_LEN) - 1;

    /* initialize each idt entry */
    sv_idt_init_elem(idt + 0, (unsigned)sv_idt_handle_e0, SV_CODE_SEGMENT_SEL);
    sv_idt_init_elem(idt + 1, (unsigned)sv_idt_handle_e1, SV_CODE_SEGMENT_SEL);
    sv_idt_init_elem(idt + 2, (unsigned)sv_idt_handle_e2, SV_CODE_SEGMENT_SEL);
    sv_idt_init_elem(idt + 3, (unsigned)sv_idt_handle_e3, SV_CODE_SEGMENT_SEL);
    sv_idt_init_elem(idt + 4, (unsigned)sv_idt_handle_e4, SV_CODE_SEGMENT_SEL);
    sv_idt_init_elem(idt + 5, (unsigned)sv_idt_handle_e5, SV_CODE_SEGMENT_SEL);
    sv_idt_init_elem(idt + 6, (unsigned)sv_idt_handle_e6, SV_CODE_SEGMENT_SEL);
    sv_idt_init_elem(idt + 7, (unsigned)sv_idt_handle_e7, SV_CODE_SEGMENT_SEL);
    sv_idt_init_elem(idt + 8, (unsigned)sv_idt_handle_e8, SV_CODE_SEGMENT_SEL);
    sv_idt_init_elem(idt + 9, (unsigned)sv_idt_handle_e9, SV_CODE_SEGMENT_SEL);
    sv_idt_init_elem(idt + 10, (unsigned)sv_idt_handle_e10, SV_CODE_SEGMENT_SEL);
    sv_idt_init_elem(idt + 11, (unsigned)sv_idt_handle_e11, SV_CODE_SEGMENT_SEL);
    sv_idt_init_elem(idt + 12, (unsigned)sv_idt_handle_e12, SV_CODE_SEGMENT_SEL);
    sv_idt_init_elem(idt + 13, (unsigned)sv_idt_handle_e13, SV_CODE_SEGMENT_SEL);
    sv_idt_init_elem(idt + 14, (unsigned)sv_idt_handle_e14, SV_CODE_SEGMENT_SEL);
    /*  entry 15: reserved          */
    sv_idt_init_elem(idt + 16, (unsigned)sv_idt_handle_e16, SV_CODE_SEGMENT_SEL);
    sv_idt_init_elem(idt + 17, (unsigned)sv_idt_handle_e17, SV_CODE_SEGMENT_SEL);
    /*  entries 18 - 31: reserved   */
}

void ia32_irq_init()
{
    sv_idt_init_elem(idt + 32, (unsigned)sv_irq_handle_irq0, SV_CODE_SEGMENT_SEL);
    sv_idt_init_elem(idt + 33, (unsigned)sv_irq_handle_irq1, SV_CODE_SEGMENT_SEL);
    sv_idt_init_elem(idt + 34, (unsigned)sv_irq_handle_irq2, SV_CODE_SEGMENT_SEL);
    sv_idt_init_elem(idt + 35, (unsigned)sv_irq_handle_irq3, SV_CODE_SEGMENT_SEL);
    sv_idt_init_elem(idt + 36, (unsigned)sv_irq_handle_irq4, SV_CODE_SEGMENT_SEL);
    sv_idt_init_elem(idt + 37, (unsigned)sv_irq_handle_irq5, SV_CODE_SEGMENT_SEL);
    sv_idt_init_elem(idt + 38, (unsigned)sv_irq_handle_irq6, SV_CODE_SEGMENT_SEL);
    sv_idt_init_elem(idt + 39, (unsigned)sv_irq_handle_irq7, SV_CODE_SEGMENT_SEL);
    sv_idt_init_elem(idt + 40, (unsigned)sv_irq_handle_irq8, SV_CODE_SEGMENT_SEL);
    sv_idt_init_elem(idt + 41, (unsigned)sv_irq_handle_irq9, SV_CODE_SEGMENT_SEL);
    sv_idt_init_elem(idt + 42, (unsigned)sv_irq_handle_irq10, SV_CODE_SEGMENT_SEL);
    sv_idt_init_elem(idt + 43, (unsigned)sv_irq_handle_irq11, SV_CODE_SEGMENT_SEL);
    sv_idt_init_elem(idt + 44, (unsigned)sv_irq_handle_irq12, SV_CODE_SEGMENT_SEL);
    sv_idt_init_elem(idt + 45, (unsigned)sv_irq_handle_irq13, SV_CODE_SEGMENT_SEL);
    sv_idt_init_elem(idt + 46, (unsigned)sv_irq_handle_irq14, SV_CODE_SEGMENT_SEL);
    sv_idt_init_elem(idt + 47, (unsigned)sv_irq_handle_irq15, SV_CODE_SEGMENT_SEL);

    sv_idt_init_elem(idt + 128, (unsigned)sv_irq_handle_syscall_irq, SV_CODE_SEGMENT_SEL);

    ia32_idt_load_idtreg();
}
