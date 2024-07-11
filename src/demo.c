/* Copyright (C) 2024 by Federico Sauter <federicosauter@pm.me>. All rights reserved. */

#include <radon.h>
#include <sys/ccio.h>

void task1()
{
    cc_cpos_set(5, 2);
    cc_printf("task1:");
    for (unsigned x = 0;; ++x)
    {
        cc_cpos_set(13, 2);
        cc_printf("%x", x);
        sleep(1);
    }
}

void task2()
{
    cc_cpos_set(5, 4);
    cc_printf("task2:");
    for (unsigned x = (unsigned)-1;; --x)
    {
        cc_cpos_set(13, 4);
        cc_printf("%x", x);
        sleep(2);
    }
}

void task3()
{
    cc_cpos_set(5, 6);
    cc_printf("task2:");
    for (unsigned x = 8;; x += 4)
    {
        cc_cpos_set(13, 6);
        cc_printf("%x", x);
        sleep(4);
    }
}

void pstask()
{
    for (;;)
    {
        cc_cpos_set(0, 8);
        ps();
        sleep(1);
    }
}

void ticks()
{
    char t[] = {'-', '/', '*', '\\'};
    for (unsigned x = 0;;)
    {
        for (unsigned i = 2; i < 8; i += 2)
        {
            cc_cpos_set(24, i);
            cc_printch(t[x++ % 4]);
        }
        nap();
    }
}

void demo_command(int argc, char** argv)
{
    cc_clear();
    create_task(task1, "task1");
    create_task(task2, "task2");
    create_task(task3, "task3");
    create_task(ticks, "ticks");
    create_task(pstask, "ps");
    for (;;)
    {
        sleep(3600);
    }
}
