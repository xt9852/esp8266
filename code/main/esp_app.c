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
#include "freertos/queue.h"

#define                 BUFF_SIZE   10240               // 缓存大小

static char             g_buff[BUFF_SIZE + 1];          // 缓存

static config_t         g_config;                       // 配置信息

void app_main()
{
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);

    ESP_LOGI(TAG, "-------- %s-%s CPU:%d REV:%d FLASH:%dMB %s --------", __DATE__, __TIME__, chip_info.cores, chip_info.revision,
    spi_flash_get_chip_size() / (1024 * 1024), (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    config_init(&g_config, g_buff, BUFF_SIZE);

    gpio_led_init(g_config.light.on);

    wifi_init(&(g_config.wifi));

    http_init(&g_config, g_buff, BUFF_SIZE);

    mqtt_init(&(g_config.mqtt));

    ota_init(g_config.mqtt.queue[0], g_buff, BUFF_SIZE);

    //gpio_74ls165_init();
    //gpio_74ls595_init();
}
