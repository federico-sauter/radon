/* Copyright (C) 2024 by Federico Sauter <federicosauter@pm.me>. All rights reserved. */

#include <svsched.h>
#include <sys/ccio.h>
#include <sys/svsyscalls.h>

extern void command_shell_task();

/* This function is the entry point for system calls. It is called by the IRQ
 * handler when a system call is performed.
 */
uint32_t sv_syscall_dispatch(unsigned id, unsigned arg0, unsigned arg1)
{
    switch (id)
    {
    case SV_SYSCALL_SLEEP:
        sv_sleep(arg0);
        break;
    case SV_SYSCALL_WAIT:
        sv_wait_for_task(arg0);
        break;
    case SV_SYSCALL_BLOCK:
        sv_wait_for_keyboard_input();
        break;
    case SV_SYSCALL_PS:
        sv_print_tasks();
        break;
    case SV_SYSCALL_TASK_CREATE:
        return sv_task_create((void (*)(void))arg0, (const char*)arg1);
        break;
    case SV_SYSCALL_TASK_EXIT:
        sv_task_exit();
        break;
    default:
        cc_printf("\nKernel error: Invalid system call (%x)!\n\n", id);
        break;
    }
    return 0x00;
}

/* Higher-level supervisor initialization. This function is called after the
 * hardware initialization has completed. When the function completes, the
 * control reaches the scheduler.
 */
void sv_init()
{
    sv_sched_init();
    // TODO: the supervisor should be decoupled from the shell!
    if (sv_task_create(command_shell_task, "[shell]") == 0)
    {
        cc_printf("ERROR!\n");
    }
}
