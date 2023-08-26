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

// ESP8266-NodeMCU模块
// 左侧针脚                     右侧针脚
// A0  - ADC0   - TOU           D0  - GPIO16 - user - WAKE
// G   - GND                    D1  - GPIO5
// VV  - 5V                     D2  - GPIO4
// S3  - GPIO10 - SDD3          D3  - GPIO0  - FLASH
// S2  - GPIO9  - SDD2          D4  - GPIO2  - TXD1
// S1  - MOSI   - SDD1          3V  - 3.3V
// SC  - CS     - SDCMD         G   - GND
// S0  - MISO   - SDD0          D5  - GPIO14        - HSCLK
// SK  - SCLK   - SDCLK         D6  - GPIO12        - HMISO
// G   - GND                    D7  - GPIO13 - RXD2 - HMOSI
// 3V  - 3.3V                   D8  - GPIO15 - TXD2 - HCS
// EN  -                        D9  - GPIO3  - RXD0
// RET -                        D10 - GPIO1  - TXD0
// G   - GND                    G   - GND
// VIN - 5V                     3V  - 3.3V

// 当ESP8266启动时GPIO1,3,9,10,16为3.3V高电平
// GPIO6-11通常连接到ESP8266板中的flash芯片.因此,不建议使用这些引脚
// GPIO2连接板载LED,以反转逻辑工作
// GPIO只可下拉,其它只可上拉
// ADC0模拟量输入,最大输入电压为0到1V
// RST被拉低时,ESP8266复位

int led = 1;

#define GPIO_ESP8266_LED    2   // esp8266上的led灯

// 74LS165并入串出
// 使用GPIO16,5,4针脚对应右侧的上数1,2,3针脚
#define GPIO_74LS165_LOAD   16  // 输出,加载数据,低电平有效
#define GPIO_74LS165_NEXT   5   // 输出,移位数据,上升沿触发
#define GPIO_74LS165_DATA   4   // 输入,读取数据.LOAD->CLK->DATA

// 74LS595串入并出
// 使用GPIO14,12,13针脚对应右侧的上数8,9,10针脚
#define GPIO_74LS595_DATA   13  // 输出,输出数据.DATA->SAVE->OUT
#define GPIO_74LS595_SAVE   14  // 输出,移位数据,上升沿触发
#define GPIO_74LS595_OUT    12  // 输出,保存数据,上升沿触发


/**
 * \brief      任务回调函数
 * \param[in]  void *param  参数
 * \return     无
*/
static void gpio_task(void *param)
{
    while (1)
    {
        led = !led;
        gpio_set_level(GPIO_ESP8266_LED, led);  // 0-点亮,1-熄灭
        vTaskDelay(pdMS_TO_TICKS(1000));        // 延时1秒
    }

    vTaskDelete(NULL);
}

/**
 * \brief      中断回调函数,不能使用ESP_LOGI
 * \param[in]  void *param  参数
 * \return     无
*/
static void gpio_isr_hander(void *param)
{
    gpio_set_level(GPIO_ESP8266_LED, led);
    led = !led;
}

/**
 * \brief      初始化NodeMCU板载中断
 * \param[in]  int id           gpio ID
 * \param[in]  int intr_type    中断触发方式
 * \param[in]  int up           上拉
 * \return     无
 */
void gpio_intr_init(int id, int intr_type, int up)
{
    // GPIO_INTR_DISABLE    不中断
    // GPIO_INTR_POSEDGE    上升沿触发
    // GPIO_INTR_NEGEDGE    下降沿触发
    // GPIO_INTR_ANYEDGE    双边沿触发
    // GPIO_INTR_LOW_LEVEL  低电平触发
    // GPIO_INTR_HIGH_LEVEL 高电平触发
    // GPIO_INTR_MAX

    gpio_config_t cfg = { 0 };
    cfg.mode = GPIO_MODE_INPUT;         // 模式:GPIO_MODE_INPUT,GPIO_MODE_OUTPUT
    cfg.intr_type = intr_type;          // 中断:
    cfg.pull_up_en = up;                // 上拉
    cfg.pull_down_en = !up;             // 下拉
    cfg.pin_bit_mask = (1ULL << id);    // gpio ID
    gpio_config(&cfg);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(id, gpio_isr_hander, (void*)id);
}

/**
 * \brief      初始化GPIO
 * \param[in]  int id       gpio ID
 * \param[in]  int mode     gpio模式
 * \return     无
 */
void gpio_init(int id, int mode)
{
    gpio_config_t cfg = { 0 };
    cfg.mode = mode;                      // 模式:GPIO_MODE_INPUT,GPIO_MODE_OUTPUT
    cfg.intr_type = GPIO_INTR_DISABLE;    // 禁止中断
    cfg.pull_up_en = 0;                   // 不上拉
    cfg.pull_down_en = 0;                 // 不下拉
    cfg.pin_bit_mask = (1ULL << id);      // gpio ID
    gpio_config(&cfg);
}

/**
 * \brief      初始化NodeMCU板载led灯
 * \param[in]  uint shine 闪
 * \return     无
 */
void gpio_led_init(uint shine)
{
    gpio_init(GPIO_ESP8266_LED, GPIO_MODE_OUTPUT);

    if (shine)
    {
        xTaskCreate(gpio_task, "led", 4096, NULL, 5, NULL);
    }
    else
    {
        gpio_intr_init(5, GPIO_INTR_NEGEDGE, 1);
    }
}

/**
 * \brief      led灯
 * \param[in]  uint led 是否点亮led
 * \return     无
 */
void gpio_led(uint led)
{
    gpio_set_level(GPIO_ESP8266_LED, !led);
}

/**
 * \brief      初始化74LS595使用的GPIO
 * \return     无
 */
void gpio_74ls595_init()
{
    ESP_LOGI(TAG, "--%s", __FUNCTION__);

    gpio_init(GPIO_74LS595_DATA, GPIO_MODE_OUTPUT);
    gpio_init(GPIO_74LS595_SAVE, GPIO_MODE_OUTPUT);
    gpio_init(GPIO_74LS595_OUT,  GPIO_MODE_OUTPUT);

    gpio_set_level(GPIO_74LS595_SAVE, 0);   // 上升沿有效
    gpio_set_level(GPIO_74LS595_OUT,  0);   // 上升沿有效

    gpio_74ls595_save(0);
    gpio_74ls595_save(1);
    gpio_74ls595_save(1);
    gpio_74ls595_save(1);
    gpio_74ls595_save(0);
    gpio_74ls595_save(1);
    gpio_74ls595_save(0);
    gpio_74ls595_save(1);

    gpio_74ls595_output();
}

/**
 * \brief      向74LS595串行输入数据
 * \param[in]  uint data 布尔值
 * \return     无
 */
void gpio_74ls595_save(uint data)
{
    gpio_set_level(GPIO_74LS595_DATA,  data);

    gpio_set_level(GPIO_74LS595_SAVE, 1);   // 上升沿有效,保存数据并移位
    vTaskDelay(20 / portTICK_PERIOD_MS);
    gpio_set_level(GPIO_74LS595_SAVE, 0);

    ESP_LOGI(TAG, "--%s--data:%d", __FUNCTION__, data);
}

/**
 * \brief      从74LS595并行输出数据
 * \param[in]  无
 * \return     无
 */
void gpio_74ls595_output()
{
    gpio_set_level(GPIO_74LS595_OUT, 1);    // 上升沿保存数据
    vTaskDelay(10 / portTICK_PERIOD_MS);
    gpio_set_level(GPIO_74LS595_OUT, 0);

    ESP_LOGI(TAG, "--%s", __FUNCTION__);
}

/**
 * \brief      初始化74LS165使用的GPIO
 * \return     无
 */
void gpio_74ls165_init()
{
    ESP_LOGI(TAG, "--%s", __FUNCTION__);

    gpio_init(GPIO_74LS165_LOAD, GPIO_MODE_OUTPUT);
    gpio_init(GPIO_74LS165_NEXT, GPIO_MODE_OUTPUT);
    gpio_init(GPIO_74LS165_DATA, GPIO_MODE_INPUT);

    gpio_set_level(GPIO_74LS165_LOAD, 1);           // 高电平-设置为读取模式,低电平-将并行数据存入寄存器
    gpio_set_level(GPIO_74LS165_NEXT, 0);           // 上升沿有效
}

/**
 * \brief      向74LS165存入并行数据
 * \return     无
 */
void gpio_74ls165_load_data()
{
    gpio_set_level(GPIO_74LS165_LOAD, 0);
    gpio_set_level(GPIO_74LS165_LOAD, 1);

    ESP_LOGI(TAG, "--%s", __FUNCTION__);
}

/**
 * \brief      从74LS165串行读取数据
 * \param[in]  int count   组装数据的个数
 * \return     数据
 */
uint gpio_74ls165_get_data(uint count)
{
    ESP_LOGI(TAG, "--%s count:%d", __FUNCTION__, count);

    uint i;
    uint bit;
    uint data = 0;

    for (i = 0; i < count; i++)
    {
        bit = gpio_get_level(GPIO_74LS165_DATA);    // 读取数据
        data = (bit << i) | data;

        gpio_set_level(GPIO_74LS165_NEXT, 1);       // 上升沿有效,移位已存入寄存器的数据
        gpio_set_level(GPIO_74LS165_NEXT, 0);

        ESP_LOGI(TAG, "--%s data:%d", __FUNCTION__, bit);
    }

    return data;
}
