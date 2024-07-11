/* Copyright (C) 2024 by Federico Sauter <federicosauter@pm.me>. All rights reserved. */

#if !defined(_RADON__H_)
#define _RADON__H_

#include <sys/types.h>

/* RADON system calls */

/* puts the current process to sleep for the specified time */
void sleep(unsigned seconds);

/* waits for the specified task to complete */
int wait(uint32_t task_id);

/* waits for keyboard input */
void block();

/* puts the current process to sleep for 300ms */
void nap();

/* prints out the task list and its state */
void ps();

/* creates a new task from the specified entry point */
uint32_t create_task(void (*entry)(), const char* name);

/* exits the current task */
void exit_task();

#endif /* _RADON__H_ */
