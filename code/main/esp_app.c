#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_ota_ops.h"
#include "esp_app_cfg.h"
#include "esp_app_wifi.h"
#include "esp_app_http.h"
#include "esp_app_mqtt.h"
#include "esp_app_gpio.h"
#include "esp_app_ota.h"
#include "esp_app_at.h"
#include "esp_app_http_cpu_page.h"
#include "esp_app_http_cpu_data.h"
#include "esp_app_http_cfg_page.h"
#include "esp_app_http_cfg_data.h"

#define SIZEOF(x)           (sizeof(x) / sizeof(x[0]))      ///< 计算数组元素数量

#define BUF_SIZE            10240                           ///< 缓存大小

const unsigned int          g_buf_size          = BUF_SIZE; ///< 缓存大小

char                        g_buf[BUF_SIZE + 1] = "";       ///< 缓存,esp8266的栈比较小,所以使用堆区内存

char                        *g_msg0             = NULL;     ///< 4G模块的mqtt通过AT指令发送来的消息
char                        *g_msg1             = NULL;
int                         g_msg_num           = 0;

static t_config             g_config;                       ///< 配置信息

static t_path_proc          g_http_data[]       = {
                                                    {"/",          http_cpu_page,  NULL},
                                                    {"/cfg.html",  http_cfg_page,  &g_config},
                                                    {"/cfg-wifi",  http_cfg_wifi,  &(g_config.wifi)},
                                                    {"/cfg-http",  http_cfg_http,  &(g_config.http)},
                                                    {"/cfg-mqtt",  http_cfg_mqtt,  &(g_config.mqtt)},
                                                    {"/cfg-light", http_cfg_light, &(g_config.light)},
                                                    {"/cpu-data",  http_cpu_data,  NULL},
                                                    {"/reboot",    http_reboot,    NULL}
                                                  };

void app_main()
{
    printf("\033[2J\033[0;0H"); // 清屏,光标移动到0,0
    at_uart_init(UART_NUM_0);

    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);

    ESP_LOGI(TAG, "-------- %s-%s CPU:%d REV:%d FLASH:%dMB %s --------",
                   __DATE__, __TIME__, chip_info.cores, chip_info.revision,
                   spi_flash_get_chip_size() / (1024 * 1024),
                   (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    config_init(&g_config);

    gpio_led_init(g_config.light.on);

    wifi_init(&(g_config.wifi));

    http_init(g_config.http.port, g_http_data, SIZEOF(g_http_data));

    mqtt_init(&(g_config.mqtt));

    at_mqtt_init(&(g_config.mqtt));

    ota_init(g_config.mqtt.queue[0]);

    //gpio_74ls165_init();
    //gpio_74ls595_init();

    t_msg_head  msg = {0};

    while (xQueueReceive(g_config.mqtt.queue[1], &msg, portMAX_DELAY) == pdTRUE)
    {
        ESP_LOGI(TAG, "rece len:%d data:%s", msg.len, msg.data);

        if (0 == (g_msg_num++ % 2))
        {
            if (NULL != g_msg0) free(g_msg0);
            g_msg0 = msg.data;
        }
        else
        {
            if (NULL != g_msg1) free(g_msg1);
            g_msg1 = msg.data;
        }
    }
}
