/* Copyright (C) 2024 by Federico Sauter <federicosauter@pm.me>. All rights reserved. */

#include <arch/svha.h>
#include <sys/svsyscalls.h>

void sleep(unsigned seconds)
{
    /* The Programmable Interval Timer (PIT) frequency is set at 100Hz,
     * see the subroutine ia32_setup_pli in ia32tasks.asm
     */
    sv_get_ha()->do_syscall(SV_SYSCALL_SLEEP, seconds * 100, 0x00);
}

int wait(uint32_t task_id)
{
    if (task_id == 0)
        return -1;
    sv_get_ha()->do_syscall(SV_SYSCALL_WAIT, task_id, 0x00);
    return 0;
}

void block()
{
    sv_get_ha()->do_syscall(SV_SYSCALL_BLOCK, 0x00, 0x00);
}

void nap()
{
    sv_get_ha()->do_syscall(SV_SYSCALL_SLEEP, 0x07, 0x00);
}

void ps()
{
    sv_get_ha()->do_syscall(SV_SYSCALL_PS, 0x00, 0x00);
}

uint32_t create_task(void (*entry)(), const char* name)
{
    return sv_get_ha()->do_syscall(SV_SYSCALL_TASK_CREATE, (unsigned)entry, (unsigned)name);
}

void exit_task()
{
    sv_get_ha()->do_syscall(SV_SYSCALL_TASK_EXIT, 0x00, 0x00);
}
