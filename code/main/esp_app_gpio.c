#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "cJSON.h"
#include "esp_log.h"
#include "esp_app_cfg.h"
#include "esp_app_gpio.h"

// ESP8266-NodeMCU模块
// ------------------------------
// |A0  - ADC0      D0  - GPIO16|
// |G   - GND       D1  - GPIO5 |
// |VV  - 5V        D2  - GPIO4 |
// |S3  - GPIO10    D3  - GPIO0 |
// |S2  - GPIO9     D4  - GPIO2 |
// |S1  -           3V  - 3.3V  |
// |SC  -           G   - GND   |
// |S0  -           D5  - GPIO14|
// |SK  -           D6  - GPIO12|
// |G   - GND       D7  - GPIO13|
// |3V  - 3.3V      D8  - GPIO15|
// |EN  -           D9  - GPIO3 |
// |RET -           D10 - GPIO1 |
// |G   - GND       G   - GND   |
// |VIN - 5V        3V  - 3.3V  |
// ------------------------------

// 标签	GPIO	可作为输入	    可作为输出	      中断 PWM 备注
// D0	GPIO16	不可用于中断	不可用于PWM或I2C  否   否  启动时为高电平用于从深度睡眠中唤醒
// D1	GPIO5	是	            是	              是   是  通常用作SCL(I2C)
// D2	GPIO4	是	            是	              是   是  通常用作SDA(I2C)
// D3	GPIO0	已被上拉	    是	              是   是  与FLASH按键连接,如果拉低则会启动失败
// D4	GPIO2	已被上拉	    是	              是   是  启动时为高电平连接板载LED,如果拉低则会启动失败
// D5	GPIO14	是	            是	              是   是  SPI(SCLK)
// D6	GPIO12	是	            是	              是   是  SPI(MISO)
// D7	GPIO13	是	            是	              是   是  SPI(MOSI)
// D8	GPIO15	已被下拉至GND	是	              是   是  SPI(CS)如果拉高则会启动失败
// RX	GPIO3	是	            RX引脚	          是   是  启动时为高电平
// TX	GPIO1	TX引脚	        是	              是   是  启动时为高电平启动时的调试输出引脚，如果拉低会启动失败
// A0	ADC0	模拟输入	    禁用              是   是  ESP8266裸芯片为0-1V. NodeMCU为0-3.3V

// 启动过程中用到的引脚
// GPIO0： 如果被拉低，则启动失败
// GPIO15：如果被拉高，则启动失败
// GPIO2： 启动时输出高电平，如果被拉低，则启动失败
// GPIO1： 启动时输出高电平，如果被拉低，则启动失败
// GPIO16：启动时输出高电平
// GPIO3： 启动时输出高电平
// GPIO10：启动时输出高电平
// GPIO9： 启动时输出高电平

// GPIO 0 2 15 EN RST
// 运行 1 1 0  1  1
// 下载 0 1 0  1  1

// 最小系统
// VCC:    3.3V
// GND:    3.3V
// EN:     1K欧上拉
// GPIO0:  1K欧上拉
// GPIO15: 1K欧下拉

int led = 1;

#define GPIO_ESP8266_LED    2   // esp8266上的led灯

// 74LS165并入串出
// 使用GPIO16,5,4针脚对应右侧的上数1,2,3针脚
#define GPIO_74LS165_LOAD   16  // 输出,加载数据,低电平有效
#define GPIO_74LS165_NEXT   5   // 输出,移位数据,上升沿触发
#define GPIO_74LS165_DATA   4   // 输入,读取数据.LOAD->DATA->CLK->DATA

// 74LS595串入并出
// 使用GPIO14,12,13针脚对应右侧的上数8,9,10针脚
#define GPIO_74LS595_DATA   13  // 输出,输出数据.DATA->SAVE->OUT
#define GPIO_74LS595_SAVE   14  // 输出,移位数据,上升沿触发
#define GPIO_74LS595_OUT    12  // 输出,保存数据,上升沿触发

uint times_intr = 0;
uint times_last = 0;

uint data = 0;

uint g_led_type = 0;
uint g_led_stat = 0;

/**
 * \brief      任务回调函数
 * \param[in]  void *param  参数
 * \return     无
*/
static void task_gpio_led(void *param)
{
    while (1)
    {
        if (g_led_type == 2)
        {
            gpio_set_level(GPIO_ESP8266_LED, g_led_stat = !g_led_stat);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    vTaskDelete(NULL);
}


/**
 * \brief      任务回调函数
 * \param[in]  void *param  参数
 * \return     无

static void gpio_task(void *param)
{
    while (1)
    {
        //led = !led;
        //gpio_set_level(GPIO_ESP8266_LED, led);  // 0-点亮,1-熄灭
        //vTaskDelay(pdMS_TO_TICKS(1000));        // 延时1秒

        if (times_last != times_intr)
        {
            ESP_LOGI(TAG, "times_intr:%d times_last:%d data:0x%x", times_intr, times_last, data);

            times_last = times_intr;

            led = !led;

            gpio_set_level(GPIO_ESP8266_LED, led);
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }

    vTaskDelete(NULL);
}*/

/**
 * \brief      中断回调函数,不能使用ESP_LOGI
 * \param[in]  void *param  参数
 * \return     无
*/
static void gpio_isr_hander(void *param)
{
    times_intr++;

    //gpio_74ls165_load_data();
    //gpio_74ls165_get_data(8);
}

/**
 * \brief      初始化NodeMCU板载中断
 * \param[in]  int id           gpio ID
 * \param[in]  int intr_type    中断触发方式
 * \param[in]  int up           上拉
 * \param[in]  int down         下拉
 * \return     无
 */
void gpio_intr_init(int id, int intr_type, int up, int down)
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
    cfg.pull_down_en = down;            // 下拉
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
 * \param[in]  uint led 是否点亮led,0-关,1-亮,2-闪
 * \return     无
 */
void gpio_led_init(uint led)
{
    gpio_init(GPIO_ESP8266_LED, GPIO_MODE_OUTPUT);

    xTaskCreate(task_gpio_led, "led", 4096, NULL, 5, NULL);

    gpio_led(led);
}

/**
 * \brief      led灯
 * \param[in]  uint led 是否点亮led,0-关,1-亮,2-闪
 * \return     无
 */
void gpio_led(uint led)
{
    g_led_type = led;

    if (led == 2) return;

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
    vTaskDelay(20 / portTICK_PERIOD_MS);
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

data = 0;

    for (i = 0; i < count; i++)
    {
        bit = gpio_get_level(GPIO_74LS165_DATA);    // 读取数据
        data = (bit << i) | data;

        gpio_set_level(GPIO_74LS165_NEXT, 1);       // 上升沿有效,移位已存入寄存器的数据

        vTaskDelay(5 / portTICK_PERIOD_MS);
        gpio_set_level(GPIO_74LS165_NEXT, 0);

        //vTaskDelay(pdMS_TO_TICKS(20));
        vTaskDelay(20 / portTICK_PERIOD_MS);

        ESP_LOGI(TAG, "--%s bit:%d", __FUNCTION__, bit);
    }

    ESP_LOGI(TAG, "--%s data:0x%x", __FUNCTION__, data);

    return data;
}
