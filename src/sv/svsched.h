/* Copyright (C) 2024 by Federico Sauter <federicosauter@pm.me>. All rights reserved. */

#if !defined(_RADON_SV_SCHED__H_)
#define _RADON_SV_SCHED__H_

#include <sys/types.h>

/* task state machine */
enum task_state
{
    INACTIVE = 0, /* a task that has not yet been created */
    SLEEPING,     /* a task that can be scheduled, if ticks_until_wakeup == 0 */
    RUNNING,      /* the task currently running */
    WAITING       /* a task, which is running but will be awaken by an IRQ */
};

/* an event that a task can wait for */
enum wait_event
{
    NONE = 0,
    KEYBOARD_IRQ,
    TASK_COMPLETION
};

/* a task can wait for a wait object */
typedef struct wait_obj_st
{
    enum wait_event event;
    uint32_t        task_id;
} wait_obj_t;

/* a task - the unit of schedulable work in the kernel
 * tasks are identified by the index to the entry in the task array.
 */
typedef struct task_st
{
    enum task_state state;
    unsigned char   ustack[8192]; /* user stack */

    /* the task state for IA32 */
    uint32_t eax, ecx, edx, ebx;
    uint32_t ebp, esi, edi, esp;
    uint32_t eip, eflags;

    char       name[16 + 1];
    uint32_t   ticks_until_wakeup; /* number of ticks until the task is awaken */
    uint32_t   ticks_running;      /* incremented on each timer interval for a running task */
    wait_obj_t waiting_for;
} task_t;

/* scheduler functions */

/* initialize the scheduler */
void sv_sched_init();

/* the kernel task scheduler */
void sv_sched();

/* create a new task and return its task ID; returns 0 on error */
uint32_t sv_task_create(void (*entry)(), const char* name);

/* put the current task to sleep for the specified amount of ticks (time slices)
 */
void sv_sleep(uint32_t ticks);

/* puts the current task to wait for another task to complete */
void sv_wait_for_task(uint32_t task_id);

/* puts the current task to wait for keyboard input */
void sv_wait_for_keyboard_input();

/* notify the scheduler that the specified event has occured, so that the tasks
 * waiting for it can be awaken
 */
void sv_notify_keyboard_event();

/* print out the currently active tasks to the screen */
void sv_print_tasks();

/* exit the current task */
void sv_task_exit();

/* dumps the currently saved state of the general purpose registers to the
 * console */
void sv_dump_reg_state();

#endif /* _RADON_SV_SCHED__H_ */
