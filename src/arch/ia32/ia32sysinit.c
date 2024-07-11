/* Copyright (C) 2024 by Federico Sauter <federicosauter@pm.me>. All rights reserved. */
#include "ds.h"
#include "multiboot.h"

#include <arch/svha.h>
#include <string.h>
// TODO: remove this dependency
#include <sys/ccio.h>

/* This is the start physical address where the supervisor is loaded. */
#define SV_START_ADDR 0x00100000
// TODO: improve memory requirements when development is more advanced
#define SV_REQUIRED_MEMORY 2097152 /* 2MB (1MB code + 1MB data) */
#define SV_TEXT_LEN        1048576 /* 1MB */

/* the GDT */
#define GDT_LEN 3

gdt_elem_t gdt[GDT_LEN];
load_ptr_t gdt_ptr;

extern uint32_t ia32_do_syscall(unsigned, unsigned, unsigned);
extern void     ia32_halt();

struct svha ha = {.do_syscall = ia32_do_syscall, .halt = ia32_halt};

volatile uint32_t syscall_rc;

void        ia32_gdt_init();
extern void ia32_gdt_load_gdtreg();
extern void ia32_idt_init();
extern void ia32_remap_irq();
extern void ia32_irq_init();
extern void ia32_setup_pli();
void        ia32_memmapinit(const multiboot_info_t* multiboot_header);
void        sv_panic(const char* msg);

void ia32_sysinit(const multiboot_info_t* multiboot_header, unsigned int magic)
{
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        sv_panic("ERROR: Invalid multiboot magic number.");
    }

    // setup global descriptor table
    ia32_gdt_init();

    // setup the interrupt descriptor table
    ia32_idt_init();

    // IRQ remapping and initialization
    ia32_remap_irq();
    ia32_irq_init();
    ia32_setup_pli();

    // memory map
    ia32_memmapinit(multiboot_header);
}

const struct svha* sv_get_ha()
{
    return &ha;
}

void ia32_gdt_init_elem(gdt_elem_t* gdt_elem, unsigned base, unsigned limit)
{
    memset(gdt_elem, 0, sizeof(gdt_elem_t));

    /* base address */
    gdt_elem->base_l = base & 0xffff;
    gdt_elem->base_m = (base >> 16) & 0xff;
    gdt_elem->base_h = (base >> 24) & 0xff;

    /* segment limit */
    gdt_elem->limit_l = limit & 0xffff;
    gdt_elem->limit_h = (limit >> 16) & 0xf;

    /* common attributes */
    gdt_elem->system      = 1; /* normal code or data segment */
    gdt_elem->ring        = 0; /* kernel mode  */
    gdt_elem->seg_present = 1; /* segment is always present in memory */
    gdt_elem->avl         = 0; /* unused */
    gdt_elem->reserved    = 0; /* must be set to zero */
    gdt_elem->offset_len  = 1; /* 32-bit offset */
    gdt_elem->granularity = 1; /* size expressed in pages */
}

void ia32_gdt_init()
{
    gdt_elem_t* p_curr_elem = gdt;

    /*
            GDT layout

        +------+-----------------------------+
        | 0x00 | NULL descriptor             |
        +------+-----------------------------+
        | 0x08 | Supervisor code segment     |
        +------+-----------------------------+
        | 0x10 | Supervisor data segment     |
        +------+-----------------------------+
    */

    memset(gdt, 0, sizeof(gdt_elem_t) * GDT_LEN);

    /* NULL segment descriptor */
    ++p_curr_elem;

    /* code segment descriptor */
    ia32_gdt_init_elem(p_curr_elem, 0x0, 0xffffffff);
    p_curr_elem->type = CODE_SEGMENT_R_X;
    ++p_curr_elem;

    /* data segment descriptor */
    ia32_gdt_init_elem(p_curr_elem, 0x0, 0xffffffff);
    p_curr_elem->type = DATA_SEGMENT_RW_;
    ++p_curr_elem;

    /* set the pointer to the GDT */
    gdt_ptr.base  = (unsigned)gdt;
    gdt_ptr.limit = (sizeof(gdt_elem_t) * GDT_LEN) - 1;

    ia32_gdt_load_gdtreg();
}

void ia32_memmapinit(const multiboot_info_t* multiboot_header)
{
    if (!(multiboot_header->flags >> 6 & 0x1))
    {
        sv_panic("ERROR: Invalid multiboot memory map.");
    }

    /* Look for a memory region marked as available that starts at (or above) SV_START_ADDR
     * to set up the supervisor heap there.
     */
    for (int i = 0; i < multiboot_header->mmap_length; i += sizeof(multiboot_memory_map_t))
    {
        multiboot_memory_map_t* memory_region =
            (multiboot_memory_map_t*)(multiboot_header->mmap_addr + i);

        if (memory_region->addr_low == SV_START_ADDR)
        {
            if (memory_region->type != MULTIBOOT_MEMORY_AVAILABLE)
            {
                sv_panic("ERROR: Supervisor memory region is not available.");
            }

            if (memory_region->len_low < SV_REQUIRED_MEMORY)
            {
                sv_panic("ERROR: Not enough memory detected to start supervisor.");
            }

            ha.avail_mem_phy_addr = memory_region->addr_low + SV_TEXT_LEN;
            ha.avail_mem_len      = memory_region->len_low - SV_TEXT_LEN;

            return;
        }
    }

    sv_panic("ERROR: Unable to locate supervisor memory region.");
}
