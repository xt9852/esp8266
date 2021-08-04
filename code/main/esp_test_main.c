#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_test_cfg.h"
#include "esp_test_wifi.h"
#include "esp_test_http.h"
#include "esp_test_gpio.h"

#define LIGHT_MAX_COUNT 16

static char            *s_buff;
static uint             s_buff_size;
static config_wifi_t    s_config_wifi;
static config_http_t    s_config_http;
static config_light_t   s_config_light[LIGHT_MAX_COUNT];
static uint             s_config_light_count;

void app_main()
{
    ESP_LOGI(TAG, "--beg--------------------------------------------------");
    ESP_LOGI(TAG, "compile time:%s %s", __DATE__, __TIME__);

    s_buff_size = 2047;
    s_buff = (char*)malloc(s_buff_size + 1);

    config_init(s_buff, s_buff_size);
    config_get_data(&s_config_http, &s_config_wifi, s_config_light, LIGHT_MAX_COUNT, &s_config_light_count);

    gpio_led_init();
    gpio_cpu_init();
    gpio_cpu_get_json(s_buff, s_buff_size);

    wifi_init(&s_config_wifi);

    http_init(&s_config_http, &s_config_wifi,
              s_config_light, &s_config_light_count, LIGHT_MAX_COUNT,
              s_buff, s_buff_size);

    ESP_LOGI(TAG, s_buff);
    ESP_LOGI(TAG, "--end--------------------------------------------------");
}
