#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_test_cfg.h"
#include "esp_test_wifi.h"
#include "esp_test_http.h"
#include "esp_test_gpio.h"


static char            *g_buff;         // 缓存
static uint             g_size;         // 缓存大小
static config_wifi_t    g_config_wifi;  // wifi配置信息
static config_http_t    g_config_http;  // http配置信息
static config_light_t   g_config_light; // light配置信息


void app_main()
{
    ESP_LOGI(TAG, "compile time:%s %s", __DATE__, __TIME__);

    g_size = 10240;
    g_buff = (char*)malloc(g_size);

    config_init(g_buff, g_size, &g_config_http, &g_config_wifi, &g_config_light);

    wifi_init(&g_config_wifi);

    http_init(g_buff, g_size, &g_config_http, &g_config_wifi, &g_config_light);

    gpio_led_init(0);
    gpio_74ls165_init();
    //gpio_74ls595_init();

    ESP_LOGI(TAG, "--------------------------------------");
}
