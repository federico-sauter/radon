/* Copyright (C) 2024 by Federico Sauter <federicosauter@pm.me>. All rights reserved. */

#include <arch/svha.h>
#include <radon.h>
#include <string.h>
#include <sys/ccio.h>
#include <sys/svsyscalls.h>
#include <tinysh.h>

/* TODO: This stuff is here temporarily until a sane console implementation */
/*    is in place */
extern volatile unsigned short keybd_last_key;
extern volatile unsigned char  keybd_last_scan;
#define is_key_pressed(x) (!(x & 0x80))
extern void cc_keybd_dobs();

/* This function is required for the tinysh integration */
void tinysh_char_out(unsigned char c)
{
    cc_printch((char)c);
}

static void version_command(int argc, char** argv)
{
    cc_printf("Radon project version 0.3\n");
}

static void clear_command(int argc, char** argv)
{
    cc_clear();
    cc_cpos_set(0, 0);
}

static void ps_command(int argc, char** argv)
{
    ps();
}

static void mem_command(int, char**)
{
    cc_printf("System memory:\n"
              "  Start address: %x\n"
              "  Total memory : %x\n",
              sv_get_ha()->avail_mem_phy_addr,
              sv_get_ha()->avail_mem_len);
}

static void memtest_command(int, char**)
{
    cc_printf("Testing memory...");

    volatile unsigned char* p = (volatile unsigned char*)sv_get_ha()->avail_mem_phy_addr;
    for (int i = 0; i < sv_get_ha()->avail_mem_len; ++i)
    {
        p[i] = 0;
    }

    cc_printf("  done\n");
}

void test_task()
{
    cc_printf("\n");
    for (int i = 0; i < 5; ++i)
    {
        cc_printf(".");
        sleep(1);
    }
    cc_printf("\nexiting task...\n");
    exit_task();
}

static void testexit_command(int argc, char** argv)
{
    uint32_t task_id = create_task(test_task, "test task");
    cc_printf("task_id = %x\n", task_id);
    if (argc == 2 && strcmp(argv[1], "wait") == 0)
    {
        wait(task_id);
    }
}

extern void demo_command(int argc, char** argv);

static tinysh_cmd_t version_cmd = {
    0, "ver", "show the operating system version", "[args]", version_command, 0, 0, 0};

static tinysh_cmd_t clear_cmd = {0, "clear", "clear the console", "[args]", clear_command, 0, 0, 0};

static tinysh_cmd_t ps_cmd = {
    0, "ps", "print the task list and state", "[args]", ps_command, 0, 0, 0};

static tinysh_cmd_t demo_cmd = {0, "demo", "multitasking demo", "[args]", demo_command, 0, 0, 0};

static tinysh_cmd_t mem_cmd = {0, "mem", "memory map", "[args]", mem_command, 0, 0, 0};

static tinysh_cmd_t memtest_cmd = {0, "memtest", "memory test", "[args]", memtest_command, 0, 0, 0};

static tinysh_cmd_t exit_test_cmd = {
    0, "exit-test", "exit-test [wait]", "[args]", testexit_command, 0, 0, 0};

void command_shell_task()
{
    cc_printf("\nRadon project - interactive shell\n");
    cc_printf("Copyright(C) 1995-2024 by Federico Sauter. All rights reserved.\n\n");

    tinysh_set_prompt("> ");
    tinysh_add_command(&version_cmd);
    tinysh_add_command(&clear_cmd);
    tinysh_add_command(&ps_cmd);
    tinysh_add_command(&demo_cmd);
    tinysh_add_command(&mem_cmd);
    tinysh_add_command(&memtest_cmd);
    tinysh_add_command(&exit_test_cmd);

    /* TODO: the console input logic should be abstracted and put somewhere else
     */
    for (;;)
    {
        block(); /* wait for the keyboard IRQ to be triggered */
        if (is_key_pressed(keybd_last_scan))
        {
            tinysh_char_in((unsigned char)keybd_last_key);
        }
    }

    cc_printf("\nNote: Interactive shell exited by the user. System halted.");
    sv_get_ha()->halt();
}
