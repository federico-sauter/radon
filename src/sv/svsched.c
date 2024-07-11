/* Copyright (C) 2024 by Federico Sauter <federicosauter@pm.me>. All rights reserved. */

#include <string.h>
#include <svsched.h>
#include <sys/ccio.h>

#if defined(SV_UNIT_TEST)
#include <assert.h>
#include <stdio.h>
#include <string.h>

extern int rand(void);

#define cc_cpos_set
#define cc_printf printf

#endif

wait_obj_t NOT_WAITING = {.event = NONE};

#define TASK_COUNT 32
static task_t   tasks[TASK_COUNT];
static task_t*  runnable_list[TASK_COUNT];
static uint32_t runnable_list_count;
static task_t*  IDLE_TASK;
static task_t*  pcurrent;

/* CPU context for the current task. The interrupt handler fills these out. */
// TODO: this must be moved to arch
volatile uint32_t _eax, _ecx, _edx, _ebx;
volatile uint32_t _ebp, _esi, _edi, _esp;
volatile uint32_t _eip, _eflags;

/* Prototypes */
task_t*     sv_task_init(void (*entry)(), const char* name);
static void sv_notify(wait_obj_t obj);
void        sv_update_task_ticks();
void        print_task(task_t* task);

int sv_insert_runnable(task_t* task)
{
    if (runnable_list_count < TASK_COUNT)
    {
        for (uint32_t i = runnable_list_count; i >= 1; --i)
        {
            runnable_list[i] = runnable_list[i - 1];
        }
        runnable_list[0] = task;
        ++runnable_list_count;
        return 0;
    }
    return -1;
}

int sv_append_runnable(task_t* task)
{
    if (runnable_list_count < TASK_COUNT)
    {
        runnable_list[runnable_list_count] = task;
        ++runnable_list_count;
        return 0;
    }
    return -1;
}

task_t* sv_pop_runnable()
{
    if (runnable_list_count)
    {
        task_t* task = runnable_list[0];
        --runnable_list_count;
        if (runnable_list_count)
        {
            memmove(runnable_list, runnable_list + 1, runnable_list_count * sizeof(task_t*));
        }
        return task;
    }
    return NULL;
}

void sv_sched_init()
{
    memset(tasks, 0, sizeof(tasks));
    memset(runnable_list, 0, sizeof(runnable_list));
    runnable_list_count = 0;

    _eax = _ecx = _edx = _ebx = 0;
    _ebp = _esi = _edi = _esp = 0;
    _eip = _eflags = 0;

    /* Note that the idle task is started at the end of the sv_startup routine,
     * when the system is fully initialized, so that it can then be pre-empted
     * by the scheduler on the first clock cycle. Therefore, it is not necessary
     * to provide an initial entry point for the task, since it will be filled
     * out by the timer IRQ handler.
     */
    sv_task_init(0, "[idle]");
    pcurrent        = &tasks[0];
    IDLE_TASK       = &tasks[0];
    pcurrent->state = RUNNING;
}

task_t* sv_task_init(void (*entry)(), const char* name)
{
    for (size_t i = 0; i < TASK_COUNT; ++i)
    {
        if (tasks[i].state == INACTIVE)
        {
            tasks[i].esp = (uint32_t)(tasks[i].ustack + sizeof(tasks[i].ustack));
            memset(tasks[i].ustack, 0x00, sizeof(tasks[i].ustack));
            tasks[i].eip                = (uint32_t)entry;
            tasks[i].eflags             = 1 << 9; /* Interrupt enabled flag */
            tasks[i].ticks_running      = 0;
            tasks[i].ticks_until_wakeup = 0;
            tasks[i].waiting_for        = NOT_WAITING;
            strcpy(tasks[i].name, name);
            return &tasks[i];
        }
    }
    return NULL;
}

uint32_t sv_task_create(void (*entry)(), const char* name)
{
    task_t* ptask = sv_task_init(entry, name);
    if (ptask != NULL)
    {
        if (sv_append_runnable(ptask) == 0)
        {
            ptask->state = SLEEPING;
            return ptask - tasks;
        }
    }
    return 0;
}

void sv_task_exit()
{
    pcurrent->state              = INACTIVE;
    pcurrent->ticks_running      = 0;
    pcurrent->ticks_until_wakeup = 0;
    *pcurrent->name              = '\0';
    wait_obj_t w                 = {.event = TASK_COMPLETION, .task_id = pcurrent - tasks};
    sv_notify(w);
    sv_update_task_ticks();
    sv_sched();
}

void sv_save_current_task_context()
{
    pcurrent->eax = _eax;
    pcurrent->ecx = _ecx;
    pcurrent->edx = _edx;
    pcurrent->ebx = _ebx;

    pcurrent->ebp = _ebp;
    pcurrent->esi = _esi;
    pcurrent->edi = _edi;
    pcurrent->esp = _esp;

    pcurrent->eip    = _eip;
    pcurrent->eflags = _eflags;
}

void sv_load_task_context(task_t* task)
{
    _eax = task->eax;
    _ecx = task->ecx;
    _edx = task->edx;
    _ebx = task->ebx;

    _ebp = task->ebp;
    _esi = task->esi;
    _edi = task->edi;
    _esp = task->esp;

    _eip    = task->eip;
    _eflags = task->eflags;
}

void sv_sleep(uint32_t ticks)
{
    sv_update_task_ticks();
    pcurrent->state              = SLEEPING;
    pcurrent->ticks_until_wakeup = ticks;
    sv_sched();
}

static void sv_wait(wait_obj_t obj)
{
    /* a waiting process does not run, which precludes this field from being set
     * already when this call is performed
     */
    pcurrent->waiting_for = obj;
    pcurrent->state       = WAITING;
    sv_sched();
}

void sv_wait_for_task(uint32_t task_id)
{
    if (task_id == 0)
        return;
    wait_obj_t w = {.event = TASK_COMPLETION, .task_id = task_id};
    sv_wait(w);
}

void sv_wait_for_keyboard_input()
{
    wait_obj_t w = {.event = KEYBOARD_IRQ, .task_id = 0};
    sv_wait(w);
}

static void sv_notify(wait_obj_t obj)
{
    for (uint32_t i = 1; i < TASK_COUNT; ++i)
    {
        if (tasks[i].state == WAITING && tasks[i].waiting_for.event == obj.event)
        {
            if (obj.event == TASK_COMPLETION && obj.task_id == tasks[i].waiting_for.task_id)
            {
                tasks[i].waiting_for        = NOT_WAITING;
                tasks[i].state              = SLEEPING;
                tasks[i].ticks_until_wakeup = 0;
                sv_append_runnable(&tasks[i]);
            }
            else if (obj.event == KEYBOARD_IRQ)
            {
                tasks[i].waiting_for        = NOT_WAITING;
                tasks[i].state              = SLEEPING;
                tasks[i].ticks_until_wakeup = 0;
                sv_insert_runnable(&tasks[i]);
            }
        }
    }
}

void sv_notify_keyboard_event()
{
    wait_obj_t obj = {.event = KEYBOARD_IRQ, .task_id = 0};
    sv_notify(obj);
}

void sv_update_task_ticks()
{
    if (IDLE_TASK->state == RUNNING)
    {
        ++IDLE_TASK->ticks_running;
    }
    for (uint32_t i = 1; i < TASK_COUNT; ++i)
    {
        if (tasks[i].state == RUNNING)
        {
            ++tasks[i].ticks_running;
        }
        else if (tasks[i].state == SLEEPING)
        {
            /* Invariant: a task which is sleeping with a ticks_until_wakeup count of
             * zero, which is *not* in the runnable list, will never be woken up.
             */
            if (tasks[i].ticks_until_wakeup > 0)
            {
                --tasks[i].ticks_until_wakeup;
                if (tasks[i].ticks_until_wakeup == 0)
                {
                    sv_append_runnable(&tasks[i]);
                }
            }
        }
    }
}

void sv_timer_handler()
{
    sv_update_task_ticks();
    sv_sched();
}

void sv_sched()
{
    task_t* pnext = sv_pop_runnable();
    if (pnext == NULL)
    {
        if (pcurrent->state == RUNNING)
        {
            /* no other task to run; the current ask can continue running */
            pnext = pcurrent;
        }
        else
        {
            /* no task to run, switch to the idle task */
            pnext = IDLE_TASK;
        }
    }

    if (pcurrent != pnext)
    {
        /* switch tasks */
        if (pcurrent->state == RUNNING)
        {
            if (pcurrent != IDLE_TASK)
            {
                /* task preempted, re-add to the tail of the runnable list */
                sv_append_runnable(pcurrent);
            }
            pcurrent->state = SLEEPING;
        }
        sv_load_task_context(pnext);
        pcurrent = pnext;
    }
    pcurrent->state = RUNNING;
}

void print_task(task_t* task)
{
    size_t len = strlen(task->name);
    cc_printf("  %s ", task->name);
    for (size_t i = len; i < sizeof(task->name); ++i)
        cc_printf(" ");
    switch (task->state)
    {
    case INACTIVE:
        cc_printf("i");
        break;
    case SLEEPING:
        cc_printf("S");
        break;
    case RUNNING:
        cc_printf("*");
        break;
    case WAITING:
        cc_printf("W");
        break;
    default:
        cc_printf("%x", task->state);
        break;
    }
    if (task->state != INACTIVE)
    {
        cc_printf("   wakeup_in=%x  running=%x", task->ticks_until_wakeup, task->ticks_running);
    }
    cc_printf("\n");
}

void sv_print_tasks()
{
    int i;
    cc_printf("tasks = {\n");
    for (i = 0; i < TASK_COUNT; ++i)
    {
        if (tasks[i].state != INACTIVE)
        {
            print_task(&tasks[i]);
        }
    }
    cc_printf("}\n");
}

/* Unit tests */
#if defined(SV_UNIT_TEST)

void print_task_list(task_t* task_list[], const uint32_t len, const char* s)
{
    int i;
    cc_printf("%s(%x) = { \n", s, len);
    for (i = 0; i < len; ++i)
    {
        print_task(task_list[i]);
    }
    cc_printf("}\n");
}

void test_runnable_list_impl()
{
    int     task_count = 3;
    task_t *p, *idle_task, *task1, *task2, *task3;
    sv_sched_init();
    idle_task = &tasks[0];
    sv_task_init(NULL, "task1");
    task1 = &tasks[1];
    sv_task_init(NULL, "task2");
    task2 = &tasks[2];
    sv_task_init(NULL, "task3");
    task3 = &tasks[3];

    printf("TEST: Runnables list implementation\n");
    sv_append_runnable(task1);
    sv_append_runnable(task2);
    sv_insert_runnable(task3);

    p = sv_pop_runnable();
    assert(p == task3);
    p = sv_pop_runnable();
    assert(p == task1);
    p = sv_pop_runnable();
    assert(p == task2);
    p = sv_pop_runnable();
    assert(p == NULL);
    printf("RESULT: PASSED\n");

    int iter = 8192;
    printf("TEST: Runnables list implementation randomized test with %i iterations\n", iter);
    for (int i = 0; i < iter; ++i)
    {
        int action = rand() % 4;
        if (action == 0)
        {
            int k = (rand() % task_count) + 1;
            sv_append_runnable(&tasks[k]);
        }
        else if (action == 1)
        {
            int k = (rand() % task_count) + 1;
            sv_insert_runnable(&tasks[k]);
        }
        else
        {
            p = sv_pop_runnable();
            assert(p->name);
        }
    }
    while (sv_pop_runnable() != NULL)
    {
        assert(p->name);
    }
    printf("RESULT: PASSED\n");
}

#define assert_invariants() _assert_invariants(__LINE__)
void _assert_invariants(int line)
{
    /* assert that there is only one task RUNNING at any given time */
    int running_count = 0;
    for (int i = 0; i < TASK_COUNT; ++i)
    {
        running_count += tasks[i].state == RUNNING ? 1 : 0;
    }
    if (running_count != 1)
    {
        fprintf(
            stderr, "Assertion failed at line %d: There is more than one process running!\n", line);
        sv_print_tasks();
        print_task_list(runnable_list, runnable_list_count, "runnables");
        assert(0);
    }

    /* assert consistency of the ticks_until_wakeup state */
    for (int i = 0; i < TASK_COUNT; ++i)
    {
        if (tasks[i].ticks_until_wakeup > 0 && tasks[i].state != SLEEPING)
        {
            fprintf(stderr,
                    "Assertion failed at line %d: A non-sleeping task "
                    "has a ticks_until_wakeup counter set!\n",
                    line);
            sv_print_tasks();
            print_task_list(runnable_list, runnable_list_count, "runnables");
            assert(0);
        }
    }

    /* assert that the runnable list does not contain duplicates */
    if (runnable_list_count > 1)
    {
        for (int i = 0; i < runnable_list_count - 1; ++i)
        {
            for (int k = i + 1; k < runnable_list_count; ++k)
            {
                if (runnable_list[i] == runnable_list[k])
                {
                    fprintf(stderr,
                            "Assertion failed at line %d: The runnable list contains "
                            "duplicates!\n",
                            line);
                    sv_print_tasks();
                    print_task_list(runnable_list, runnable_list_count, "runnables");
                    assert(0);
                }
            }
        }
    }

    /* assert that the idle task is not in the runnable list */
    for (int i = 0; i < runnable_list_count; ++i)
    {
        if (runnable_list[i] == IDLE_TASK)
        {
            fprintf(stderr,
                    "Assertion failed at line %d: The runnable list contains "
                    "an instance of the idle task!\n",
                    line);
            sv_print_tasks();
            print_task_list(runnable_list, runnable_list_count, "runnables");
            assert(0);
        }
    }
}

void test_task_sleep_and_scheduling_over_idle_task()
{
    printf("TEST: Task sleep and scheduling over idle task\n");

    sv_sched_init();
    sv_task_create(NULL, "task1");
    task_t* task1 = &tasks[1];
    assert(runnable_list_count == 1);
    assert_invariants();
    assert(task1->state == SLEEPING);
    assert(IDLE_TASK->state == RUNNING);

    /* choose task1 */
    sv_timer_handler();
    assert_invariants();
    assert(task1->state == RUNNING);
    assert(IDLE_TASK->state == SLEEPING);
    assert(IDLE_TASK->ticks_running == 1);

    /* task1 calls sleep() */
    sv_sleep(2);
    assert_invariants();
    assert(task1->state == SLEEPING);
    assert(task1->ticks_until_wakeup == 2);
    assert(task1->ticks_running == 1);
    assert(IDLE_TASK->state == RUNNING);
    assert(IDLE_TASK->ticks_running == 1);

    sv_timer_handler();
    assert_invariants();
    assert(task1->state == SLEEPING);
    assert(task1->ticks_until_wakeup == 1);
    assert(task1->ticks_running == 1);
    assert(IDLE_TASK->state == RUNNING);
    assert(IDLE_TASK->ticks_running == 2);

    sv_timer_handler();
    assert_invariants();
    assert(task1->state == RUNNING);
    assert(task1->ticks_until_wakeup == 0);
    assert(task1->ticks_running == 1);
    assert(IDLE_TASK->state == SLEEPING);
    assert(IDLE_TASK->ticks_running == 3);

    for (int i = 0; i < 1024; ++i)
    {
        sv_timer_handler();
        assert_invariants();
        assert(task1->state == RUNNING);
        assert(task1->ticks_until_wakeup == 0);
        assert(task1->ticks_running == 2 + i);
        assert(IDLE_TASK->state == SLEEPING);
        assert(IDLE_TASK->ticks_running == 3);
    }

    printf("RESULT: PASSED\n");
}

void test_scheduling_fairness_on_sleep()
{
    printf("TEST: Scheduling algorithm fainess with tasks that sleep\n");

    sv_sched_init();
    sv_task_create(NULL, "task1");
    sv_task_create(NULL, "task2");
    task_t* task1 = &tasks[1];
    task_t* task2 = &tasks[2];
    assert(runnable_list_count == 2);
    assert_invariants();
    assert(task1->state == SLEEPING);
    assert(task2->state == SLEEPING);
    assert(IDLE_TASK->state == RUNNING);

    /* choose next task */
    sv_timer_handler();
    assert(task1->state == RUNNING);
    assert_invariants();

    task_t* t1 = task1;
    task_t* t2 = task2;
    for (int i = 0; i < 1024; ++i)
    {
        /* t1 calls sleep() */
        sv_sleep(2);
        assert(t1->state == SLEEPING);
        assert(t1->ticks_until_wakeup == 2);
        assert(t2->state == RUNNING);
        assert_invariants();

        /* t2 calls sleep() */
        sv_sleep(2);
        assert(t2->state == SLEEPING);
        assert(t2->ticks_until_wakeup == 2);
        assert(t1->state == SLEEPING);
        assert(t1->ticks_until_wakeup == 1);
        assert(IDLE_TASK->state == RUNNING);
        assert_invariants();

        /* idle task gets preempted */
        sv_timer_handler();
        assert(t1->state == RUNNING);
        assert(t2->state == SLEEPING);
        assert(t2->ticks_until_wakeup == 1);
        assert(IDLE_TASK->state == SLEEPING);
        assert_invariants();

        /* t1 gets preempted */
        sv_timer_handler();
        assert(t2->state == RUNNING);
        assert(t2->ticks_until_wakeup == 0);
        assert(t1->state == SLEEPING);
        assert(IDLE_TASK->state == SLEEPING);
        assert_invariants();

        t1 = t1 == task1 ? task2 : task1;
        t2 = t2 == task1 ? task2 : task1;
    }
    /* Note that there are four ticks per loop and one extra at the beginning
     *  1024*4+1 = 4097
     * Adding the ticks from the tasks we get:
     *  1536+1536+1025 = 4097 q.e.d.
     */
    assert(t1->ticks_running == t2->ticks_running);
    assert(t1->ticks_running == 1536);
    assert(IDLE_TASK->ticks_running == 1025);
    printf("RESULT: PASSED\n");
}

int main(int argc, char* argv[])
{
    test_runnable_list_impl();
    test_task_sleep_and_scheduling_over_idle_task();
    test_scheduling_fairness_on_sleep();
    return 0;
}
#endif
