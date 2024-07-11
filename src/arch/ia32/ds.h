/* Copyright (C) 2024 by Federico Sauter <federicosauter@pm.me>. All rights reserved. */

#if !defined(_RADON_SYS_SV_DS__H_)
#define _RADON_SYS_SV_DS__H_

/* constants */
#define CODE_SEGMENT_R_X 0xa
#define DATA_SEGMENT_RW_ 0x2
#define TSS_SEGMENT_R_X  0x9

#define SV_CODE_SEGMENT_SEL 0x8

/* data structures */

/* represents an entry in the GDT */
typedef struct gdt_elem_st
{
    unsigned short limit_l;

    unsigned short base_l;

    unsigned char base_m;
    unsigned      type : 4;
    unsigned      system : 1;
    unsigned      ring : 2; /* DPL */
    unsigned      seg_present : 1;

    unsigned limit_h : 4;
    unsigned avl : 1;
    unsigned reserved : 1;   /* always zero */
    unsigned offset_len : 1; /* D or B flag */
    unsigned granularity : 1;

    unsigned char base_h;
} __attribute__((packed)) gdt_elem_t;

/* represents an entry in the IDT */
typedef struct idt_elem_st
{
    unsigned short offset_l;
    unsigned short selector;

    unsigned char reserved;
    unsigned      gate_type : 5;
    unsigned      ring : 2; /* DPL */
    unsigned      present : 1;

    unsigned short offset_h;
} __attribute__((packed)) idt_elem_t;

/* represents an entry in the TSS */
typedef struct tss_elem_st
{
    unsigned short link;
    unsigned short reserved0;

    unsigned int   esp0;
    unsigned short ss0;
    unsigned short reserved1;

    unsigned int   esp1;
    unsigned short ss1;
    unsigned short reserved2;

    unsigned int   esp2;
    unsigned short ss2;
    unsigned short reserved3;

    unsigned int cr3;
    unsigned int eip;
    unsigned int eflags;
    unsigned int eax;
    unsigned int ecx;
    unsigned int edx;
    unsigned int ebx;
    unsigned int esp;
    unsigned int ebp;
    unsigned int esi;
    unsigned int edi;

    unsigned short es;
    unsigned short reserved4;
    unsigned short cs;
    unsigned short reserved5;
    unsigned short ss;
    unsigned short reserved6;
    unsigned short ds;
    unsigned short reserved7;
    unsigned short fs;
    unsigned short reserved8;
    unsigned short gs;
    unsigned short reserved9;
    unsigned short ldtr;
    unsigned short reserved10;

    unsigned short reserved11;
    unsigned short iopb_off;
} __attribute__((packed)) tss_elem_t;

/* a pointer to either the GDT or the IDT */
typedef struct load_ptr_st
{
    unsigned short limit;
    unsigned int   base;
} __attribute__((packed)) load_ptr_t;

#endif /* _RADON_SYS_SV_DS__H_ */
