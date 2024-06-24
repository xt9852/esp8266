#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_app_cfg.h"

static uint         g_size;     // 缓存大小
static char*        g_buff;     // 缓存,esp8266的栈比较小,所以使用堆区内存
static nvs_handle   g_nvs;      // 句柄

/**
 * \brief      解析配置JSON字符串
 * \param[in]  char            *json    JSON字符串
 * \param[out] p_config_wifi    wifi    配置数据
 * \return     int 0-成功，其它失败
 */
int config_get_wifi_data(const char *json, p_config_wifi wifi)
{
    cJSON *root = cJSON_Parse(json);

    if (NULL == root)
    {
        ESP_LOGE(TAG, "Failed parse json");
        return -1;
    }

    cJSON *type = cJSON_GetObjectItem(root, CONFIG_WIFI_TYPE);

    if (NULL == type)
    {
        ESP_LOGE(TAG, "Failed json no %s", CONFIG_WIFI_TYPE);
        cJSON_Delete(root);
        return -2;
    }

    wifi->type = type->valueint;

    cJSON *ssid = cJSON_GetObjectItem(root, CONFIG_WIFI_SSID);

    if (NULL == ssid)
    {
        ESP_LOGE(TAG, "Failed json no %s", CONFIG_WIFI_SSID);
        cJSON_Delete(root);
        return -3;
    }

    if (NULL == ssid->valuestring)
    {
        ESP_LOGE(TAG, "Failed json %s type error", CONFIG_WIFI_SSID);
        cJSON_Delete(root);
        return -4;
    }

    strncpy(wifi->ssid, ssid->valuestring, sizeof(wifi->ssid) - 1);

    cJSON *password = cJSON_GetObjectItem(root, CONFIG_WIFI_PASSWORD);

    if (NULL == password)
    {
        ESP_LOGE(TAG, "Failed json no %s", CONFIG_WIFI_PASSWORD);
        cJSON_Delete(root);
        return -5;
    }

    if (NULL == password->valuestring)
    {
        ESP_LOGE(TAG, "Failed json %s type error", CONFIG_WIFI_PASSWORD);
        cJSON_Delete(root);
        return -6;
    }

    strncpy(wifi->password, password->valuestring, sizeof(wifi->password) - 1);

    ESP_LOGI(TAG, " wifi type:%d", wifi->type);
    ESP_LOGI(TAG, " wifi ssid:%s", wifi->ssid);
    ESP_LOGI(TAG, " wifi password:%s", wifi->password);

    cJSON_Delete(root);
    return 0;
}

/**
 * \brief      生成JSON字符串
 * \param[in]  p_config_data    wifi    配置数据
 * \param[out] char            *json    JSON字符串
 * \return     int 0-成功，其它失败
 */
int config_get_wifi_json_str(const p_config_wifi wifi, char *json)
{
    return sprintf(json, "{\"%s\":%d,\"%s\":\"%s\",\"%s\":\"%s\"}",
                          CONFIG_WIFI_TYPE,     wifi->type,
                          CONFIG_WIFI_SSID,     wifi->ssid,
                          CONFIG_WIFI_PASSWORD, wifi->password);
}

/**
 * \brief      解析配置JSON字符串
 * \param[in]  char            *json    JSON字符串
 * \param[out] p_config_mqtt    mqtt    配置数据
 * \return     int 0-成功，其它失败
 */
int config_get_mqtt_data(const char *json, p_config_mqtt mqtt)
{
    cJSON *root = cJSON_Parse(json);

    if (NULL == root)
    {
        ESP_LOGE(TAG, "Failed parse json");
        return -1;
    }

    cJSON *broker = cJSON_GetObjectItem(root, CONFIG_MQTT_BROKER);

    if (NULL == broker)
    {
        ESP_LOGE(TAG, "Failed json no %s", CONFIG_MQTT_BROKER);
        cJSON_Delete(root);
        return -2;
    }

    if (NULL == broker->valuestring)
    {
        ESP_LOGE(TAG, "Failed json %s type error", CONFIG_MQTT_USERNAME);
        cJSON_Delete(root);
        return -3;
    }

    strncpy(mqtt->broker, broker->valuestring, sizeof(mqtt->broker) - 1);

    cJSON *username = cJSON_GetObjectItem(root, CONFIG_MQTT_USERNAME);

    if (NULL == username)
    {
        ESP_LOGE(TAG, "Failed json no %s", CONFIG_MQTT_USERNAME);
        cJSON_Delete(root);
        return -4;
    }

    if (NULL == username->valuestring)
    {
        ESP_LOGE(TAG, "Failed json %s type error", CONFIG_MQTT_USERNAME);
        cJSON_Delete(root);
        return -5;
    }

    strncpy(mqtt->username, username->valuestring, sizeof(mqtt->username) - 1);

    cJSON *password = cJSON_GetObjectItem(root, CONFIG_MQTT_PASSWORD);

    if (NULL == password)
    {
        ESP_LOGE(TAG, "Failed json no %s", CONFIG_MQTT_PASSWORD);
        cJSON_Delete(root);
        return -6;
    }

    if (NULL == password->valuestring)
    {
        ESP_LOGE(TAG, "Failed json %s type error", CONFIG_MQTT_PASSWORD);
        cJSON_Delete(root);
        return -7;
    }

    strncpy(mqtt->password, password->valuestring, sizeof(mqtt->password) - 1);

    cJSON *clientid = cJSON_GetObjectItem(root, CONFIG_MQTT_CLIENTID);

    if (NULL == clientid)
    {
        ESP_LOGE(TAG, "Failed json no %s", CONFIG_MQTT_CLIENTID);
        cJSON_Delete(root);
        return -8;
    }

    if (NULL == clientid->valuestring)
    {
        ESP_LOGE(TAG, "Failed json %s type error", CONFIG_MQTT_CLIENTID);
        cJSON_Delete(root);
        return -9;
    }

    strncpy(mqtt->clientid, clientid->valuestring, sizeof(mqtt->clientid) - 1);

    cJSON *topic = cJSON_GetObjectItem(root, CONFIG_MQTT_TOPIC);

    if (NULL == topic)
    {
        ESP_LOGE(TAG, "Failed json no %s", CONFIG_MQTT_TOPIC);
        cJSON_Delete(root);
        return -10;
    }

    int count = cJSON_GetArraySize(topic);

    mqtt->topic_count = count;

    if (count > MQTT_TOPIC_MAX)
    {
        ESP_LOGE(TAG, "Failed json %s array count:%d > MAX:%d", CONFIG_MQTT_TOPIC, count, MQTT_TOPIC_MAX);
        cJSON_Delete(root);
        return -11;
    }

    for (unsigned int i = 0; i < mqtt->topic_count; i++)
    {
        cJSON *item = cJSON_GetArrayItem(topic, i);

        if (NULL == item || NULL == item->valuestring)
        {
            ESP_LOGE(TAG, "Failed json %s[%d]", CONFIG_MQTT_TOPIC, i);
            cJSON_Delete(root);
            return -12;
        }

        strcpy(mqtt->topic[i], item->valuestring);
    }

    ESP_LOGI(TAG, "mqtt broker:%s", mqtt->broker);
    ESP_LOGI(TAG, "mqtt username:%s", mqtt->username);
    ESP_LOGI(TAG, "mqtt password:%s", mqtt->password);
    ESP_LOGI(TAG, "mqtt clientid:%s", mqtt->clientid);

    for (unsigned int i = 0; i < mqtt->topic_count; i++)
    {
        ESP_LOGI(TAG, "mqtt topic[%d]:%s", i, mqtt->topic[i]);
    }

    cJSON_Delete(root);
    return 0;
}

/**
 * \brief      生成JSON字符串
 * \param[in]  p_config_data    mqtt    配置数据
 * \param[out] char            *json    JSON字符串
 * \return     int 0-成功，其它失败
 */
int config_get_mqtt_json_str(const p_config_mqtt mqtt, char *json)
{
    int len = sprintf(json, "{\"%s\":\"%s\",\"%s\":\"%s\",\"%s\":\"%s\",\"%s\":\"%s\",\"%s\":[",
                             CONFIG_MQTT_BROKER,   mqtt->broker,
                             CONFIG_MQTT_USERNAME, mqtt->username,
                             CONFIG_MQTT_PASSWORD, mqtt->password,
                             CONFIG_MQTT_CLIENTID, mqtt->clientid,
                             CONFIG_MQTT_TOPIC);

    for (unsigned int i = 0; i < mqtt->topic_count; i++)
    {
        len += sprintf(&json[len], "\"%s\",", mqtt->topic[i]);
    }

    json[len - 1] = ']';
    json[len++] = '}';
    json[len] = '\0';
    return len;
}

/**
 * \brief      解析配置JSON字符串
 * \param[in]  const char      *json    JSON字符串
 * \param[out] p_config_http    http    配置数据
 * \return     int 0-成功，其它失败
 */
int config_get_http_data(const char *json, p_config_http http)
{
    cJSON *root = cJSON_Parse(json);

    if (NULL == root)
    {
        ESP_LOGE(TAG, "Failed parse json");
        return -1;
    }

    cJSON *port = cJSON_GetObjectItem(root, CONFIG_HTTP_PORT);

    if (NULL == port)
    {
        ESP_LOGE(TAG, "Failed json no %s", CONFIG_HTTP_PORT);
        cJSON_Delete(root);
        return -2;
    }

    http->port = port->valueint;

    ESP_LOGI(TAG, "http port:%d", http->port);

    cJSON_Delete(root);
    return 0;
}

/**
 * \brief      生成JSON字符串
 * \param[in]  const p_config_http  http    配置数据
 * \param[out] char                *json    JSON字符串
 * \return     int 0-成功，其它失败
 */
int config_get_http_json_str(const p_config_http http, char *json)
{
    return sprintf(json, "{\"%s\":%d}", CONFIG_HTTP_PORT, http->port);
}

/**
 * \brief      解析配置JSON字符串
 * \param[in]  char            *json    JSON字符串
 * \param[out] p_config_light   light   配置数据
 * \return     int 0-成功，其它失败
 */
int config_get_light_data(const char *json, p_config_light light)
{
    cJSON *root = cJSON_Parse(json);

    if (NULL == root)
    {
        ESP_LOGE(TAG, "Failed parse json");
        return -1;
    }

    cJSON *on = cJSON_GetObjectItem(root, LIGHT_ON_NAME);

    if (NULL == on)
    {
        ESP_LOGE(TAG, "Failed json no %s", LIGHT_ON_NAME);
        cJSON_Delete(root);
        return -2;
    }

    light->on = on->valueint;

    ESP_LOGI(TAG, "light on:%d", light->on);

    cJSON_Delete(root);
    return 0;
}

/**
 * \brief      生成JSON字符串
 * \param[in]  const p_config_light     light   配置数据
 * \param[out] char                    *json    JSON字符串
 * \return     int 0-成功，其它失败
 */
int config_get_light_json_str(const p_config_light light, char *json)
{
    return sprintf(json, "{\"%s\":%d}", CONFIG_LIGHT_ON, light->on);
}

/**
 * \brief      读取数据
 * \param[in]  const char               *key 文件名
 * \return     int 0-成功，其它失败
 */
int config_get_nvs_data(const char *key)
{
    size_t len = 0;

    esp_err_t err = nvs_get_str(g_nvs, key, NULL, &len);

    if (err != ESP_OK)
    {
        if (ESP_ERR_NVS_NOT_FOUND == err && strcmp(key, CONFIG_WIFI_KEY) == 0)
        {
            config_wifi_t wifi = { WIFI_TYPE_STA, "ChinaNet-EUG9", "XT9852dxc0m" };
            config_put_wifi(&wifi);
            err = nvs_get_str(g_nvs, key, NULL, &len);
        }
        else if (ESP_ERR_NVS_NOT_FOUND == err && strcmp(key, CONFIG_MQTT_KEY) == 0)
        {
            config_mqtt_t mqtt = { "mqtt://broker.emqx.io:1883", "", "", "36825c95-9b21-81a5-7930-0239418a4587/esp8266",
                                   2, { "36825c95-9b21-81a5-7930-0239418a4587/ota", "36825c95-9b21-81a5-7930-0239418a4587/msg" }, {} };
            config_put_mqtt(&mqtt);
            err = nvs_get_str(g_nvs, key, NULL, &len);
        }
        else if (ESP_ERR_NVS_NOT_FOUND == err && strcmp(key, CONFIG_HTTP_KEY) == 0)
        {
            config_http_t http = { 80 };
            config_put_http(&http);
            err = nvs_get_str(g_nvs, key, NULL, &len);
        }
        else if (ESP_ERR_NVS_NOT_FOUND == err && strcmp(key, CONFIG_LIGHT_KEY) == 0)
        {
            config_light_t light = { 0 };
            config_put_light(&light);
            err = nvs_get_str(g_nvs, key, NULL, &len);
        }
        else
        {
            ESP_LOGE(TAG, "read %s error set %s", key, esp_err_to_name(err));
            return -1;
        }

        if (err == ESP_OK)
        {
            ESP_LOGW(TAG, "read %s ESP_ERR_NVS_NOT_FOUND use default value ok", key);
            return 0;
        }
        else
        {
            ESP_LOGE(TAG, "read %s ESP_ERR_NVS_NOT_FOUND use default value stll error", key);
            return -1;
        }
    }

    err = nvs_get_str(g_nvs, key, g_buff, &len);

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "read %s data error:%s", key, esp_err_to_name(err));
        return -1;
    }

    return 0;
}

/**
 * \brief      读取配置文件，内容为JSON字符串，解析JOSN
 * \param[out] p_config  config
 * \return     int 0-成功，其它失败
 */
int config_get_data(p_config config)
{
    p_config_wifi  wifi  = &(config->wifi);
    p_config_mqtt  mqtt  = &(config->mqtt);
    p_config_http  http  = &(config->http);
    p_config_light light = &(config->light);

    if (config_get_nvs_data(CONFIG_WIFI_KEY) != 0) return -1;

    if (config_get_wifi_data(g_buff, wifi) != 0) return -2;

    if (config_get_nvs_data(CONFIG_MQTT_KEY) != 0) return -3;

    if (config_get_mqtt_data(g_buff, mqtt) != 0) return -4;

    if (config_get_nvs_data(CONFIG_HTTP_KEY) != 0) return -5;

    if (config_get_http_data(g_buff, http) != 0) return -6;

    if (config_get_nvs_data(CONFIG_LIGHT_KEY) != 0) return -7;

    if (config_get_light_data(g_buff, light) != 0) return -8;

    return 0;
}

/**
 * \brief      写入数据
 * \param[in]  const char               *key      文件名
 * \param[in]  const char               *data     数据
 * \return     int 0-成功，其它失败
 */
int config_set_nvs_data(const char *key, const char *data)
{
    esp_err_t err = nvs_set_str(g_nvs, key, data);

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "write %s data error:%s", key, esp_err_to_name(err));
        return -1;
    }

    err = nvs_commit(g_nvs);

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "nvs_commit %s error:%s", key, esp_err_to_name(err));
    }

    ESP_LOGI(TAG, "set %s %s", key, data);
    return 0;
}

/**
 * \brief      生成JOSN字符串，写入配置文件
 * \param[in]  p_config_wifi wifi
 * \return     int 0-成功，其它失败
 */
int config_put_wifi(p_config_wifi wifi)
{
    if (config_get_wifi_json_str(wifi, g_buff) <= 0) return -1;

    if (config_set_nvs_data(CONFIG_WIFI_KEY, g_buff) != 0) return -2;

    return 0;
}

/**
 * \brief      生成JOSN字符串，写入配置文件
 * \param[in]  p_config_mqtt mqtt
 * \return     int 0-成功，其它失败
 */
int config_put_mqtt(p_config_mqtt mqtt)
{
    if (config_get_mqtt_json_str(mqtt, g_buff) <= 0) return -1;

    if (config_set_nvs_data(CONFIG_MQTT_KEY, g_buff) != 0) return -2;

    return 0;
}

/**
 * \brief      生成JOSN字符串，写入配置文件
 * \param[in]  p_config_http http
 * \return     int 0-成功，其它失败
 */
int config_put_http(p_config_http http)
{
    if (config_get_http_json_str(http, g_buff) <= 0) return -1;

    if (config_set_nvs_data(CONFIG_HTTP_KEY, g_buff) != 0) return -2;

    return 0;
}

/**
 * \brief      生成JOSN字符串，写入配置文件
 * \param[in]  p_config_light light
 * \return     int 0-成功，其它失败
 */
int config_put_light(p_config_light light)
{
    if (config_get_light_json_str(light, g_buff) <= 0) return -1;

    if (config_set_nvs_data(CONFIG_LIGHT_KEY, g_buff) != 0) return -2;

    return 0;
}

/**
 * \brief      初始化配置
 * \param[out] p_config     config      配置数据
 * \param[in]  char        *buff        缓存
 * \param[in]  uint         size        缓存大小
 * \return     int 0-成功，其它失败
 */
int config_init(p_config config, char *buff, uint size)
{
    if (NULL == buff || NULL == buff || 0 == size)
    {
        ESP_LOGE(TAG, "arg error");
        return -1;
    }

    g_buff = buff;
    g_size = size;

    esp_err_t err = nvs_flash_init();

    if (err == ESP_ERR_NVS_NO_FREE_PAGES)
    {
        ESP_LOGE(TAG, "init NVS Error (%s)!\n", esp_err_to_name(err));
        return -2;
    }

    err = nvs_open("nvs_data", NVS_READWRITE, &g_nvs);

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "opening NVS Error (%s)!\n", esp_err_to_name(err));
        return -3;
    }

    return config_get_data(config);
}