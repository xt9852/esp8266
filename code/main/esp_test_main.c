#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_test_cfg.h"
#include "esp_test_wifi.h"
#include "esp_test_http.h"
#include "esp_test_gpio.h"


static char            *s_buff;         // 缓存
static uint             s_size;         // 缓存大小
static config_wifi_t    s_config_wifi;  // wifi配置信息
static config_http_t    s_config_http;  // http配置信息
static config_light_t   s_config_light; // light配置信息


void app_main()
{
    ESP_LOGI(TAG, "----------------------beg------------------------------");
    ESP_LOGI(TAG, "compile time:%s %s", __DATE__, __TIME__);
    ESP_LOGI(TAG, "-------------------------------------------------------");

    s_size = 1024 * 10 - 1;

    s_buff = (char*)malloc(s_size + 1);

    config_init(s_buff, s_size, &s_config_http, &s_config_wifi, &s_config_light);

    wifi_init(&s_config_wifi);

    http_init(s_buff, s_size, &s_config_http, &s_config_wifi, &s_config_light);

    gpio_led_init();

    gpio_74ls165_init();

    gpio_74ls595_init();

    ESP_LOGI(TAG, "----------------------end------------------------------");
}
