#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h" // EventGroupHandle_t BIT0 BIT1
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_app_wifi.h"

#define EVENT_BIT_OK            BIT0
#define EVENT_BIT_FAIL          BIT1

static EventGroupHandle_t       g_wifi_event_group;


/**
 * \brief      事件回调函数
 * \param[in]  void* arg                    JSON字符串
 * \param[in]  esp_event_base_t event_base  事件类型
 * \param[in]  int32_t event_id             事件ID
 * \param[in]  void* event_data             事件数据
 * \return     无
 */
static void wifi_event_callback(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    switch (event_id)
    {
        case WIFI_EVENT_STA_START:
        {
            esp_wifi_connect();
            break;
        }
        case WIFI_EVENT_STA_CONNECTED:
        {
            break;
        }
        case IP_EVENT_STA_GOT_IP:
        {
            if (event_base != IP_EVENT) break;

            ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
            ESP_LOGI(TAG, "STA connect OK %s", ip4addr_ntoa(&event->ip_info.ip));
            xEventGroupSetBits(g_wifi_event_group, EVENT_BIT_OK);
            break;
        }
        case WIFI_EVENT_STA_DISCONNECTED:
        {
            esp_wifi_connect();
            ESP_LOGI(TAG, "STA connect FAIL");
            break;
        }
        case WIFI_EVENT_AP_STACONNECTED:
        {
            wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*)event_data;
            ESP_LOGI(TAG, "AP "MACSTR" join, AID=%d", MAC2STR(event->mac), event->aid);
            break;
        }
        case WIFI_EVENT_AP_STADISCONNECTED:
        {
            wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*)event_data;
            ESP_LOGI(TAG, "AP "MACSTR" leave, AID=%d", MAC2STR(event->mac), event->aid);
            break;
        }
        default:
        {
            ESP_LOGI(TAG, "wifi event_id:%d", event_id);
            break;
        }
    }
}

/**
 * \brief      初始化WIFI
 * \param[in]  p_config_wifi    wifi    配置数据
 * \return     0-成功，其它失败
 */
int wifi_init(p_config_wifi wifi)
{
    wifi_config_t wifi_config = { 0 };
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    tcpip_adapter_init();
    esp_event_loop_create_default();
    esp_wifi_init(&cfg);

    if (wifi->type == WIFI_TYPE_AP)
    {
        esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_callback, NULL);

        strcpy((char*)wifi_config.ap.ssid, wifi->ssid);
        strcpy((char*)wifi_config.ap.password, wifi->password);
        wifi_config.ap.ssid_len = strlen(wifi->ssid);
        wifi_config.ap.max_connection = 16;
        wifi_config.ap.authmode = (strlen(wifi->password) == 0) ? WIFI_AUTH_OPEN : WIFI_AUTH_WPA_WPA2_PSK;

        esp_wifi_set_mode(WIFI_MODE_AP);
        esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config);
        esp_wifi_start();
    }
    else
    {
        g_wifi_event_group = xEventGroupCreate();
        esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_callback, NULL);
        esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_callback, NULL);

        strcpy((char*)wifi_config.sta.ssid, wifi->ssid);
        strcpy((char*)wifi_config.sta.password, wifi->password);
        wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

        esp_wifi_set_mode(WIFI_MODE_STA);
        esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
        esp_wifi_start();

        // 等待 EVENT_BIT_OK, EVENT_BIT_FAIL, 在wifi_event_callback内设置
        xEventGroupWaitBits(g_wifi_event_group, EVENT_BIT_OK | EVENT_BIT_FAIL, pdFALSE, pdFALSE, portMAX_DELAY);

        esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_callback);
        esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_callback);
        vEventGroupDelete(g_wifi_event_group);
    }

    return 0;
}
