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


#define GPIO_ESP8266_LED    2   // esp8266上的led灯

#define GPIO_74LS165_LOAD   16  // 输出,加载数据,低电平有效
#define GPIO_74LS165_CLK    5   // 输出,心跳,上升沿触发
#define GPIO_74LS165_DATA   4   // 输入,数据.LOAD->CLK->DATA

#define GPIO_74LS595_DATA   14  // 输出,数据.DATA->CLK->OUT
#define GPIO_74LS595_OUT    12  // 输出,保存数据,上升沿触发
#define GPIO_74LS595_CLK    13  // 输出,移位数据,上升沿触发


/**
 * \brief      任务回调函数
 * \param[in]  void *pvParameters  参数
 * \return     无
 
xTaskCreate(gpio_task, "led", 4096, NULL, 5, NULL);
 
static void gpio_task(void *pvParameters)
{
    while (1)
    {
        gpio_set_level(GPIO_ESP8266_LED, 0);    // 点亮
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // 延时1秒
        gpio_set_level(GPIO_ESP8266_LED, 1);    // 熄灭
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}
*/

/**
 * \brief      初始化GPIO
 * \param[in]  int id       gpio ID
 * \param[in]  int mode     gpio模式
 * \return     无
 */
void gpio_init(int id, int mode)
{
    gpio_config_t gpio_conf;
    gpio_conf.pin_bit_mask = (1ULL << id);      // gpio ID
    gpio_conf.mode = mode;                      // 模式:GPIO_MODE_INPUT,GPIO_MODE_OUTPUT
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
    gpio_init(GPIO_ESP8266_LED, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_ESP8266_LED, 1);
}

/**
 * \brief      led灯
 * \param[in]  int led 是否点亮led
 * \return     无
 */
void gpio_led(int led)
{
    gpio_set_level(GPIO_ESP8266_LED, led);
}

/**
 * \brief      初始化74LS165使用的GPIO
 * \return     无
 */
void gpio_cpu_init()
{
    gpio_init(GPIO_74LS165_CLK,  GPIO_MODE_OUTPUT);
    gpio_init(GPIO_74LS165_LOAD, GPIO_MODE_OUTPUT);
    gpio_init(GPIO_74LS165_DATA, GPIO_MODE_INPUT);
    gpio_set_level(GPIO_74LS165_CLK,  0);
    gpio_set_level(GPIO_74LS165_LOAD, 1);

    gpio_init(GPIO_74LS595_DATA, GPIO_MODE_OUTPUT);
    gpio_init(GPIO_74LS595_CLK,  GPIO_MODE_OUTPUT);
    gpio_init(GPIO_74LS595_OUT,  GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_74LS595_CLK, 0);
    gpio_set_level(GPIO_74LS595_OUT, 0);
}

/**
 * \brief      载入74LS165数据
 * \return     无
 */
void gpio_cpu_load_data()
{
    gpio_set_level(GPIO_74LS165_LOAD, 0);   // 低电平,将并行数据存入寄存器
    vTaskDelay(2 / portTICK_PERIOD_MS);
    gpio_set_level(GPIO_74LS165_LOAD, 1);   // 高电平,设置为读取模式
    vTaskDelay(2 / portTICK_PERIOD_MS);
}

/**
 * \brief      读取74LS165数据,组装成数据
 * \param[in]  int count   组装数据的个数
 * \return     数据
 */
uint gpio_cpu_get_data(uint count)
{
    uint i;

    uint data = 0;

    for (i = 0; i < count; i++)
    {
        data = (data << 1) | gpio_get_level(GPIO_74LS165_DATA);    // 读取数据
        vTaskDelay(2 / portTICK_PERIOD_MS);

        gpio_set_level(GPIO_74LS165_CLK, 1);        // 上升沿输出已存入寄存器的数据
        vTaskDelay(2 / portTICK_PERIOD_MS);
        gpio_set_level(GPIO_74LS165_CLK, 0);
        vTaskDelay(2 / portTICK_PERIOD_MS);
    }

    return data;
}

/**
 * \brief      输出数据
 * \param[in]  uint data    数据
 * \return     无
 */
void gpio_cpu_set_data(uint data)
{
    gpio_set_level(GPIO_74LS595_DATA, data);   // 输出数据
    vTaskDelay(2 / portTICK_PERIOD_MS);

    gpio_set_level(GPIO_74LS595_CLK, 1);    // 上升沿时移位
    vTaskDelay(2 / portTICK_PERIOD_MS);
    gpio_set_level(GPIO_74LS595_CLK, 0);
    vTaskDelay(2 / portTICK_PERIOD_MS);
}

/**
 * \brief      保存并输出74LS595数据
 * \return     无
 */
void gpio_cpu_out_data()
{
    gpio_set_level(GPIO_74LS595_OUT, 1);    // 上升沿时保存数据并输出
    vTaskDelay(2 / portTICK_PERIOD_MS);
    gpio_set_level(GPIO_74LS595_OUT, 0);
    vTaskDelay(2 / portTICK_PERIOD_MS);
}
