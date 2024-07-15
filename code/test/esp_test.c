/**
 *\file     esp_test.c
 *\note     UTF-8
 *\author   xt
 *\version  0.0.1
 *\brief    主模块实现
 *          时间|事件
 *          -|-
 *          2022.02.06|创建文件
 *          2024.06.23|添加Doxygen注释
 */
#include <string.h>

extern int g_text_beg;
extern int g_text_end;

extern int g_bss_beg;
extern int g_bss_end;

extern int g_data_beg;
extern int g_data_end;

extern int g_rodata_beg;
extern int g_rodata_end;

extern int g_heap_beg;
extern int g_heap_end;

void uart_div_modify(unsigned int uart_no, unsigned int baud_div);

int ets_printf(const char *fmt, ...);

static int data1 = 0;   // 当初始值为0,且值有变化,将放在bss段内
static int data2 = 1;   // 当初始值不为0,且值有变化,将放在data段内
static int data3 = 2;   // 值无变化,将放在rodata段内

unsigned int get_sp()
{
    unsigned int sp;
    asm volatile ("mov %0, sp;" : "=r" (sp));
    return sp;
}

void print_info()
{
    ets_printf("\033[0;31m");
    ets_printf("--------------------------------------\n");
    ets_printf("Hello world! %s %s\n", __DATE__, __TIME__);

    ets_printf("&g_bss_beg:     0x%x\n", &g_bss_beg);
    ets_printf("&g_bss_end:     0x%x\n", &g_bss_end);

    ets_printf("&g_data_beg:    0x%x\n", &g_data_beg);
    ets_printf("&g_data_end:    0x%x\n", &g_data_end);

    ets_printf("&g_rodata_beg:  0x%x\n", &g_rodata_beg);
    ets_printf("&g_rodata_end:  0x%x\n", &g_rodata_end);

    ets_printf("&g_heap_beg:    0x%x\n", &g_heap_beg);
    ets_printf("&g_heap_end:    0x%x\n", &g_heap_end);

    ets_printf("&g_text_beg:    0x%x\n", &g_text_beg);
    ets_printf("&g_text_end:    0x%x\n", &g_text_end);

    ets_printf("sp:             0x%x\n", get_sp());

    ets_printf("\033[0m");

    ets_printf("data1++: %d bss段\n",    data1++);
    ets_printf("data2++: %d data段\n",   data2++);
    ets_printf("data3:   %d rodata段\n", data3);

    // --------------------------------------
    // Hello world! Jun 24 2024 22:34:09
    // &g_bss_beg:    0x3ffe8000
    // &g_bss_end:    0x3ffe8004
    // &g_data_beg:   0x3ffe8004
    // &g_data_end:   0x3ffe8008
    // &g_rodata_beg: 0x3ffe8008
    // &g_rodata_end: 0x3ffe8160
    // &g_heap_beg:   0x3ffe8160
    // &g_heap_end:   0x3fffffff
    // &g_text_beg:   0x40100000
    // &g_text_end:   0x40100180
}

void app_main()
{
    print_info();

    memset(&g_bss_beg, 0, (&g_bss_end - &g_bss_beg) * sizeof(g_bss_beg)); // 清全局未初始化段
}


