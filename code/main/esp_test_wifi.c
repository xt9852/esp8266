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
#include "esp_test_wifi.h"

#define EVENT_BIT_CONNECTED     BIT0
#define EVENT_BIT_FAIL          BIT1

static EventGroupHandle_t       s_wifi_event_group;


/**
 * \brief      事件回调函数
 * \param[in]  void* arg                    JSON字符串
 * \param[in]  esp_event_base_t event_base  事件类型
 * \param[in]  int32_t event_id             事件ID
 * \param[in]  void* event_data             事件数据
 * \return     无
 */
static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:%s", ip4addr_ntoa(&event->ip_info.ip));
        xEventGroupSetBits(s_wifi_event_group, EVENT_BIT_CONNECTED);
    }
    else
    {
        switch(event_id)
        {
            case WIFI_EVENT_AP_STACONNECTED:
            {
                wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*)event_data;
                ESP_LOGI(TAG, "station "MACSTR" join, AID=%d", MAC2STR(event->mac), event->aid);
                break;
            }
            case WIFI_EVENT_AP_STADISCONNECTED:
            {
                wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*)event_data;
                ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d", MAC2STR(event->mac), event->aid);
                break;
            }
            case WIFI_EVENT_STA_START:
            {
                esp_wifi_connect();
                ESP_LOGI(TAG, "connectiong...");
                break;
            }
            case WIFI_EVENT_STA_DISCONNECTED:
            {
                esp_wifi_connect();
                ESP_LOGI(TAG,"connect to the AP fail");
                break;
            }
            default:
            {

                break;
            }
        }
    }
}

/**
 * \brief      初始化WIFI
 * \param[in]  p_config_data conf   配置数据
 * \return     无
 */
void wifi_init(p_config_wifi wifi)
{
    ESP_LOGI(TAG, "------------------------------%s--beg----", __FUNCTION__);

    wifi_config_t wifi_config = { 0 };
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    tcpip_adapter_init();
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    if (wifi->type == WIFI_TYPE_AP)
    {
        strcpy((char*)wifi_config.ap.ssid, wifi->ssid);
        strcpy((char*)wifi_config.ap.password, wifi->password);
        wifi_config.ap.ssid_len = strlen(wifi->ssid);
        wifi_config.ap.max_connection = 16;
        wifi_config.ap.authmode = (strlen(wifi->password) == 0) ? WIFI_AUTH_OPEN : WIFI_AUTH_WPA_WPA2_PSK;

        ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
        ESP_ERROR_CHECK(esp_wifi_start());
        ESP_LOGI(TAG, "Finish SSID:%s, password:%s", wifi_config.sta.ssid, wifi_config.sta.password);
    }
    else
    {
        strcpy((char*)wifi_config.sta.ssid, wifi->ssid);
        strcpy((char*)wifi_config.sta.password, wifi->password);
        wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

        ESP_LOGI(TAG, "SSID:%s password:%s", wifi_config.sta.ssid, wifi_config.sta.password);
        s_wifi_event_group = xEventGroupCreate();
        ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
        ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
        ESP_ERROR_CHECK(esp_wifi_start() );

        /* Waiting until either the connection is established (EVENT_BIT_CONNECTED)
         * or connection failed for the maximum
         * number of re-tries (EVENT_BIT_FAIL).
         * The bits are set by event_handler() (see above) */
        EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                EVENT_BIT_CONNECTED | EVENT_BIT_FAIL,
                pdFALSE,
                pdFALSE,
                portMAX_DELAY);

        /* xEventGroupWaitBits() returns the bits before the call returned,
         * hence we can test which event actually happened. */
        if (bits & EVENT_BIT_CONNECTED)
        {
            ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                     wifi_config.sta.ssid, wifi_config.sta.password);
        }
        else if (bits & EVENT_BIT_FAIL)
        {
            ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                     wifi_config.sta.ssid, wifi_config.sta.password);
        }
        else
        {
            ESP_LOGE(TAG, "UNEXPECTED EVENT");
        }

        ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler));
        ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler));
        vEventGroupDelete(s_wifi_event_group);
    }

    ESP_LOGI(TAG, "------------------------------%s--end----", __FUNCTION__);
}
