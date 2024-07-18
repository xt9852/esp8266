#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mqtt_client.h"
#include "esp_log.h"
#include "esp_app_mqtt.h"

static esp_mqtt_client_handle_t     g_mqtt;
static p_config_mqtt                g_conf;

/**
 *\brief        MQTT发布信息
 *\param[in]    topic       主题
 *\param[in]    msg         信息
 *\return                   无
 */
int mqtt_publish(const char *topic, const char *msg)
{
    int msg_id = esp_mqtt_client_publish(g_mqtt, topic, msg, 0, 1, 0);
    ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
    return 0;
}

/**
 *\brief        MQTT事件回调
 *\param[in]    param       自定义参数
 *\param[in]    base        类型
 *\param[in]    event_id    事件ID
 *\param[in]    event_data  事件数据
 *\return                   无
 */
static void mqtt_event_handler(void *param, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);

    int msg_id;
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
    esp_mqtt_client_handle_t client = event->client;

    switch (event->event_id)
    {
        case MQTT_EVENT_BEFORE_CONNECT:
            ESP_LOGI(TAG, "MQTT_EVENT_BEFORE_CONNECT");
            break;
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            for (int i = 0; i < g_conf->topic_count; i++)
            {
                msg_id = esp_mqtt_client_subscribe(client, g_conf->topic[i], 0);
                ESP_LOGI(TAG, "SUBSCRIBE:%s, msg_id=%d", g_conf->topic[i], msg_id);
            }
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA TOPIC=%.*s", event->topic_len, event->topic);
            ESP_LOGI(TAG, "MQTT_EVENT_DATA DATA=%.*s", event->data_len, event->data);

            BaseType_t ret = pdFALSE;

            for (int i = 0; i < g_conf->topic_count; i++)
            {
                if (0 == strncmp(event->topic, g_conf->topic[i], event->topic_len))
                {
                    t_msg_head msg;
                    msg.len = event->data_len;
                    msg.data = (char*)malloc(event->data_len + 1);
                    msg.data[msg.len] = '\0';
                    memcpy(msg.data, event->data, event->data_len);
                    ret = xQueueSend(g_conf->queue[i], &msg, portMAX_DELAY);
                    break;
                }
            }

            ESP_LOGI(TAG, "Send queue %s", (pdTRUE == ret) ? "ok" : "fail");
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
}

/**
 *\brief        初始化MQTT
 *\param[in]    mqtt        配置数据
 *\return       0           成功
 */
int mqtt_init(p_config_mqtt mqtt)
{
    if (NULL == mqtt)
    {
        ESP_LOGE(TAG, "mqtt is null");
        return -1;
    }

    g_conf = mqtt;

    for (int i = 0; i < mqtt->topic_count; i++)
    {
        mqtt->queue[i] = xQueueCreate(10, sizeof(t_msg_head));
    }

    esp_mqtt_client_config_t mqtt_cfg = {
        .uri       = mqtt->broker,
        .username  = mqtt->username,
        .password  = mqtt->password,
        .client_id = mqtt->clientid,
    };

    g_mqtt = esp_mqtt_client_init(&mqtt_cfg);

    if (NULL == g_mqtt)
    {
        ESP_LOGE(TAG, "esp_mqtt_client_init fail");
        return -2;
    }

    esp_err_t err = esp_mqtt_client_register_event(g_mqtt, ESP_EVENT_ANY_ID, mqtt_event_handler, g_mqtt);

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "esp_mqtt_client_register_event error %s", esp_err_to_name(err));
        return -3;
    }

    err = esp_mqtt_client_start(g_mqtt);

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "esp_mqtt_client_start error %s", esp_err_to_name(err));
        return -4;
    }

    return 0;
}
