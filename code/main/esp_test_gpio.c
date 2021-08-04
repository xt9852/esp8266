#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "cJSON.h"
#include "esp_log.h"
#include "esp_test_cfg.h"
#include "esp_test_gpio.h"

#define GPIO_LED            2
#define GPIO_74LS165_CLK    4
#define GPIO_74LS165_LOAD   5
#define GPIO_74LS165_DATA   16


/**
 * \brief      任务回调函数
 * \param[in]  void* pvParameters  参数
 * \return     无
 */
static void gpio_task(void *pvParameters)
{
    while (1)
    {
        gpio_set_level(GPIO_LED, 0);            // 熄灭
        vTaskDelay(500 / portTICK_PERIOD_MS);   // 延时500ms
        gpio_set_level(GPIO_LED, 1);            // 点亮
        vTaskDelay(500 / portTICK_PERIOD_MS);   // 延时500ms
    }

    vTaskDelete(NULL);
}



/**
 * \brief      初始化GPIO
 * \return     无
 */
void gpio_init(int id, int mode)
{
    gpio_config_t gpio_conf;
    gpio_conf.pin_bit_mask = (1ULL << id);      // 选择gpio2
    gpio_conf.mode = mode;                      // 输入输出模式
    gpio_conf.pull_up_en = 0;                   // 不上拉
    gpio_conf.pull_down_en = 0;                 // 不下拉
    gpio_conf.intr_type = GPIO_INTR_DISABLE;    // 禁止中断
    gpio_config(&gpio_conf);
}

/**
 * \brief      led灯,1秒间隔闪灯
 * \return     无
 */
void gpio_led_init()
{
    gpio_init(GPIO_LED, GPIO_MODE_OUTPUT);

    xTaskCreate(gpio_task, "gpio", 4096, NULL, 5, NULL);
}

/**
 * \brief      初始化GPIO
 * \return     无
 */
void gpio_cpu_init()
{
    gpio_init(GPIO_74LS165_CLK, GPIO_MODE_OUTPUT);
    gpio_init(GPIO_74LS165_LOAD, GPIO_MODE_OUTPUT);
    gpio_init(GPIO_74LS165_DATA, GPIO_MODE_INPUT);
}

/**
 * \brief      载入74ls165数据
 * \return     无
 */
void gpio_cpu_load_data()
{
    gpio_set_level(GPIO_74LS165_LOAD, 0);   // 将并行数据存入寄存器
    vTaskDelay(2 / portTICK_PERIOD_MS);     // 延时2ms
    gpio_set_level(GPIO_74LS165_LOAD, 1);   // 设置为读取模式
    vTaskDelay(2 / portTICK_PERIOD_MS);     // 延时2ms
}

/**
 * \brief      读取74ls165数据,组装成数据
 * \param[in]  int count   组装数据的个数
 * \return     数据
 */
uint gpio_cpu_get_data(uint count)
{
    uint i;
    uint tmp;
    uint data = 0;

    ESP_LOGI(TAG, "get data count %d", count);

    for (i = 0; i < count; i++)
    {
        tmp = gpio_get_level(GPIO_74LS165_DATA);    // 读取数据
        vTaskDelay(2 / portTICK_PERIOD_MS);         // 延时2ms

        gpio_set_level(GPIO_74LS165_CLK, 0);        // 上升沿输出已存入寄存器的数据
        vTaskDelay(2 / portTICK_PERIOD_MS);         // 延时2ms
        gpio_set_level(GPIO_74LS165_CLK, 1);
        vTaskDelay(2 / portTICK_PERIOD_MS);

        data |= (tmp << i);
    }

    return data;
}

/**
 * \brief      读取cpu数据,组成json串
 * \param[out] char *data  配置数据
 * \param[in]  uint json_size   配置数据缓冲区大小
 * \return     int 0-成功，其它失败
 */
int gpio_cpu_get_json(char *json, uint json_size)
{
    ESP_LOGI(TAG, "--beg--%s--------------------", __FUNCTION__);

    gpio_cpu_load_data();   // 载入数据

    int data = gpio_cpu_get_data(16);

    return snprintf(json, json_size, "{\"data\":\"0x%04X\"}", data);

/*
    int mi_addr = gpio_cpu_get_data(12);
    int mi_addr_next = gpio_cpu_get_data(12);
    int mi_addr_next_true = gpio_cpu_get_data(12);
    ESP_LOGI(TAG, "---------1");

    int bus_data = gpio_cpu_get_data(16);
    int bus_addr = gpio_cpu_get_data(16);
    ESP_LOGI(TAG, "---------2");
    int out_alu = gpio_cpu_get_data(16);
    int out_shf = gpio_cpu_get_data(16);
    int sel_int = gpio_cpu_get_data(1);
    int sel_ir = gpio_cpu_get_data(1);
    int sel_jp = gpio_cpu_get_data(1);
    int sel_je = gpio_cpu_get_data(1);
    int sel_jb = gpio_cpu_get_data(1);
    int sel_jl = gpio_cpu_get_data(1);
    ESP_LOGI(TAG, "---------3");
    int sel = gpio_cpu_get_data(2);
    int alu = gpio_cpu_get_data(2);
    int msw = gpio_cpu_get_data(3);
    int ir = gpio_cpu_get_data(2);
    int ip = gpio_cpu_get_data(3);
    int sp = gpio_cpu_get_data(3);
    int ar = gpio_cpu_get_data(3);
    int br = gpio_cpu_get_data(3);
    int tr = gpio_cpu_get_data(3);

    ESP_LOGI(TAG, "--end--%s--------------------", __FUNCTION__);

    return sprintf(json, "{\"mi_addr\":\"0x%03X\",\"mi_addr_next\":\"0x%03X\",\"mi_addr_next_true\":\"0x%03X\","
                         "\"bus_data\":\"0x%04X\",\"bus_addr\":\"0x%04X\","
                         "\"out_alu\":\"0x%04X\",\"out_shf\":\"0x%04X\","
                         "\"sel\":%d,\"sel_int\":%d,\"sel_ir\":%d,\"sel_jp\":%d,\"sel_je\":%d,\"sel_jb\":%d,\"sel_jl\":%d,"
                         "\"alu\":%d,\"msw\":%d,\"ir\":%d,\"ip\":%d,"
                         "\"sp\":%d,\"ar\":%d,\"br\":%d,\"tr\":%d}",
                         mi_addr, mi_addr_next, mi_addr_next_true,
                         bus_data, bus_addr, out_alu, out_shf,
                         sel, sel_int, sel_ir, sel_jp, sel_je, sel_jb, sel_jl,
                         alu, msw, ir, ip, sp, ar, br, tr);
*/
}
