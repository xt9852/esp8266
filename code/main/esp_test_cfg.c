#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_test_cfg.h"


static char *s_buff;
static uint  s_buff_size;


/**
 * \brief      初始化配置
 * \param[in]  char *buff     缓存
 * \param[in]  uint buff_size 缓存大小
 * \return     int 0-成功，其它失败
 */
int config_init(char *buff, uint buff_size)
{
    ESP_LOGI(TAG, "--beg---------------%s", __FUNCTION__);

    s_buff = buff;
    s_buff_size = buff_size;

    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = true
    };

    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    switch(ret)
    {
        case ESP_OK:
        {
            ESP_LOGI(TAG, "esp_vfs_spiffs_register ok");
            break;
        }
        case ESP_FAIL:
        {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
            return -1;
        }
        case ESP_ERR_NOT_FOUND:
        {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
            return -2;
        }
        default:
        {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
            return -3;
        }
    }

    size_t used = 0;
    size_t total = 0;

    ret = esp_spiffs_info(NULL, &total, &used);

    if (ret != ESP_OK)
    {
        esp_vfs_spiffs_unregister(NULL);
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
        return -4;
    }

    ESP_LOGI(TAG, "Partition total: %d, used: %d", total, used);
/*
#define LIGHT_MAX_COUNT 16
    int            i;
    config_http_t  http = { 80 };
    config_wifi_t  wifi = { WIFI_TYPE_STA, "ChinaNet-EUG9", "xt9852@dx.com" };
    config_light_t light[LIGHT_MAX_COUNT];

    for (i = 0; i < LIGHT_MAX_COUNT; i++)
    {
        light[i].id = i;
        light[i].on = i % 2;
        light[i].color = i % 4;
        sprintf(light[i].name, "灯%d", i);
    }

    config_put_http(&http);
    config_put_wifi(&wifi);
    config_put_light(light, LIGHT_MAX_COUNT);
*/
    ESP_LOGI(TAG, "--end---------------%s", __FUNCTION__);
    return 0;
}

/**
 * \brief      解析配置JSON字符串
 * \param[in]  const char *json     JSON字符串
 * \param[out] p_config_http conf   配置数据
 * \return     int 0-成功，其它失败
 */
int config_http_get_data(const char *json, p_config_http http)
{
    ESP_LOGI(TAG, "parse http %s", json);

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
        return -2;
    }

    http->port = port->valueint;

    ESP_LOGI(TAG, "http port:%d", http->port);

    cJSON_Delete(root);
    return 0;
}

/**
 * \brief      生成JSON字符串
 * \param[in]  const p_config_http http     配置数据
 * \param[out] char *json                   JSON字符串
 * \return     int 0-成功，其它失败
 */
int config_http_get_json(const p_config_http http, char *json)
{
    sprintf(json, "{\"%s\":%d}", CONFIG_HTTP_PORT, http->port);
    ESP_LOGI(TAG, json);
    return 0;
}

/**
 * \brief      解析配置JSON字符串
 * \param[in]  char *json           JSON字符串
 * \param[out] p_config_wifi wifi   配置数据
 * \return     int 0-成功，其它失败
 */
int config_wifi_get_data(const char *json, p_config_wifi wifi)
{
    ESP_LOGI(TAG, "parse wifi %s", json);

    cJSON *root = cJSON_Parse(json);

    if (NULL == root)
    {
        ESP_LOGE(TAG, "Failed parse json");
        return -1;
    }

    // process type

    cJSON *type = cJSON_GetObjectItem(root, CONFIG_WIFI_TYPE);

    if (NULL == type)
    {
        ESP_LOGE(TAG, "Failed json no %s", CONFIG_WIFI_TYPE);
        return -2;
    }

    wifi->type = type->valueint;

    // process ssid

    cJSON *ssid = cJSON_GetObjectItem(root, CONFIG_WIFI_SSID);

    if (NULL == ssid)
    {
        ESP_LOGE(TAG, "Failed json no %s", CONFIG_WIFI_SSID);
        return -3;
    }

    if (NULL == ssid->valuestring)
    {
        ESP_LOGE(TAG, "Failed json %s type error", CONFIG_WIFI_SSID);
        return -4;
    }

    strncpy(wifi->ssid, ssid->valuestring, sizeof(wifi->ssid) - 1);

    // process password

    cJSON *password = cJSON_GetObjectItem(root, CONFIG_WIFI_PASSWORD);

    if (NULL == password)
    {
        ESP_LOGE(TAG, "Failed json no %s", CONFIG_WIFI_PASSWORD);
        return -5;
    }

    if (NULL == password->valuestring)
    {
        ESP_LOGE(TAG, "Failed json %s type error", CONFIG_WIFI_PASSWORD);
        return -6;
    }

    strncpy(wifi->password, password->valuestring, sizeof(wifi->password) - 1);

    ESP_LOGI(TAG, "wifi type:%d ssid:%s password:%s",
                   wifi->type,
                   wifi->ssid,
                   wifi->password);

    cJSON_Delete(root);
    return 0;
}

/**
 * \brief      生成JSON字符串
 * \param[in]  p_config_data wifi   配置数据
 * \param[out] char *json           JSON字符串
 * \return     int 0-成功，其它失败
 */
int config_wifi_get_json(const p_config_wifi wifi, char *json)
{
    sprintf(json, "{\"%s\":%d,\"%s\":\"%s\",\"%s\":\"%s\"}",
                  CONFIG_WIFI_TYPE, wifi->type,
                  CONFIG_WIFI_SSID, wifi->ssid,
                  CONFIG_WIFI_PASSWORD, wifi->password);

    ESP_LOGI(TAG, json);
    return 0;
}

/**
 * \brief      解析配置JSON字符串
 * \param[in]  char *json           JSON字符串
 * \param[in]  uint   max           最大节点数量
 * \param[out] p_config_light light 配置数据
 * \param[out] uint *count          配置数据数量
 * \return     int 0-成功，其它失败
 */
int config_light_get_data(const char *json, uint max, p_config_light light, uint *count)
{
    ESP_LOGI(TAG, "parse light %s", json);

    cJSON *root = cJSON_Parse(json);

    if (NULL == root)
    {
        ESP_LOGE(TAG, "Failed parse json");
        return -1;
    }

    int i;
    cJSON *item;
    cJSON *id;
    cJSON *on;
    cJSON *name;
    cJSON *color;

    *count = cJSON_GetArraySize(root); /*获取数组长度*/

    if (*count > max)
    {
        *count = max;
    }

    for (i = 0; i < *count; i++)
    {
        item = cJSON_GetArrayItem(root, i);

        // process id
        id = cJSON_GetObjectItem(item, CONFIG_LIGHT_ID);

        if (NULL == id)
        {
            ESP_LOGE(TAG, "Failed json no %s", CONFIG_LIGHT_ID);
            return -2;
        }

        light->id = id->valueint;

        // process name
        name = cJSON_GetObjectItem(item, CONFIG_LIGHT_NAME);

        if (NULL == name)
        {
            ESP_LOGE(TAG, "Failed json no %s", CONFIG_LIGHT_NAME);
            return -3;
        }

        if (NULL == name->valuestring)
        {
            ESP_LOGE(TAG, "Failed json %s type error", CONFIG_LIGHT_NAME);
            return -4;
        }

        strncpy(light->name, name->valuestring, sizeof(light->name) - 1);

        // process color
        color = cJSON_GetObjectItem(item, CONFIG_LIGHT_COLOR);

        if (NULL == color)
        {
            ESP_LOGE(TAG, "Failed json no %s", CONFIG_LIGHT_COLOR);
            return -5;
        }

        light->color = color->valueint;

        // process on
        on = cJSON_GetObjectItem(item, CONFIG_LIGHT_ON);

        if (NULL == on)
        {
            ESP_LOGE(TAG, "Failed json no %s", CONFIG_LIGHT_ON);
            return -6;
        }

        light->on = on->valueint;

        ESP_LOGI(TAG, "light id:%d name:%s color:%d on:%d",
                  light->id,
                  light->name,
                  light->color,
                  light->on);

        light++;
    }

    cJSON_Delete(root);
    return 0;
}

/**
 * \brief      生成JSON字符串
 * \param[in]  const p_config_light light 配置数据
 * \param[in]  uint  count          配置数据数量
 * \param[out] char *json           JSON字符串
 * \return     int 0-成功，其它失败
 */
int config_light_get_json(const p_config_light light, uint count, char *json)
{
    int i;
    int len = 1;
    p_config_light tmp = light;

    for (i = 0; i < count; i++)
    {
        len += sprintf(&json[len],
                       "{\"%s\":%d,\"%s\":%d,\"%s\":\"%s\",\"%s\":%d},",
                       CONFIG_LIGHT_ID,    tmp->id,
                       CONFIG_LIGHT_ON,    tmp->on,
                       CONFIG_LIGHT_NAME,  tmp->name,
                       CONFIG_LIGHT_COLOR, tmp->color);

        tmp++;
    }

    json[0] = '[';
    json[len - 1] = ']';

    ESP_LOGI(TAG, json);
    return 0;
}

/**
 * \brief      读取配置文件，内容为JSON字符串，解析JOSN
 * \param[out] p_config_http  *http
 * \param[out] p_config_wifi  *wifi
 * \param[out] p_config_light *light
 * \param[in]  uint            max          light最大节点数量
 * \param[out] uint           *count        light数据数量
 * \return     int 0-成功，其它失败
 */
int config_get_data(p_config_http http, p_config_wifi wifi, p_config_light light, uint light_max, uint *light_count)
{
    ESP_LOGI(TAG, "--beg--%s-----------------", __FUNCTION__);

    // http config
    ESP_LOGI(TAG, "--open:%s", CONFIG_HTTP_FILENAME);

    FILE *fp = fopen(CONFIG_HTTP_FILENAME, "rb+");

    if (fp == NULL)
    {
        esp_vfs_spiffs_unregister(NULL);
        ESP_LOGE(TAG, "Failed to open %s", CONFIG_HTTP_FILENAME);
        return -1;
    }

    fgets(s_buff, s_buff_size, fp);
    fclose(fp);

    // http data
    config_http_get_data(s_buff, http);

    // wifi config
    ESP_LOGI(TAG, "--open:%s", CONFIG_WIFI_FILENAME);

    fp = fopen(CONFIG_WIFI_FILENAME, "rb+");

    if (fp == NULL)
    {
        esp_vfs_spiffs_unregister(NULL);
        ESP_LOGE(TAG, "Failed to open %s", CONFIG_WIFI_FILENAME);
        return -2;
    }

    fgets(s_buff, s_buff_size, fp);
    fclose(fp);

    // wifi data
    config_wifi_get_data(s_buff, wifi);

    // light config
    ESP_LOGI(TAG, "--open:%s", CONFIG_LIGHT_FILENAME);

    fp = fopen(CONFIG_LIGHT_FILENAME, "rb+");

    if (fp == NULL)
    {
        esp_vfs_spiffs_unregister(NULL);
        ESP_LOGE(TAG, "Failed to open %s", CONFIG_LIGHT_FILENAME);
        return -3;
    }

    fgets(s_buff, s_buff_size, fp);
    fclose(fp);

    // light data
    config_light_get_data(s_buff, light_max, light, light_count);

    ESP_LOGI(TAG, "--end--%s-----------------", __FUNCTION__);
    return 0;
}

/**
 * \brief      生成JOSN字符串，写入配置文件
 * \param[in]  p_config_http *http
 * \return     int 0-成功，其它失败
 */
int config_put_http(p_config_http http)
{
    ESP_LOGI(TAG, "--beg--%s-----------------", __FUNCTION__);

    ESP_LOGI(TAG, "--update:%s", CONFIG_HTTP_FILENAME);

    config_http_get_json(http, s_buff);

    FILE *fp = fopen(CONFIG_HTTP_FILENAME, "wb+");

    if (fp == NULL)
    {
        ESP_LOGE(TAG, "fopen %s", CONFIG_HTTP_FILENAME);
        return -1;
    }

    fwrite(s_buff, strlen(s_buff), 1, fp);
    fclose(fp);

    ESP_LOGI(TAG, "--end--%s-----------------", __FUNCTION__);
    return 0;
}

/**
 * \brief      生成JOSN字符串，写入配置文件
 * \param[in]  p_config_wifi *wifi
 * \return     int 0-成功，其它失败
 */
int config_put_wifi(p_config_wifi wifi)
{
    ESP_LOGI(TAG, "--beg--%s-----------------", __FUNCTION__);

    ESP_LOGI(TAG, "--update:%s", CONFIG_WIFI_FILENAME);

    config_wifi_get_json(wifi, s_buff);

    FILE *fp = fopen(CONFIG_WIFI_FILENAME, "wb+");

    if (fp == NULL)
    {
        ESP_LOGE(TAG, "fopen %s", CONFIG_WIFI_FILENAME);
        return -6;
    }

    fwrite(s_buff, strlen(s_buff), 1, fp);
    fclose(fp);

    ESP_LOGI(TAG, "--end--%s-----------------", __FUNCTION__);
    return 0;
}

/**
 * \brief      生成JOSN字符串，写入配置文件
 * \param[in]  p_config_light *light
 * \param[in]  uint light_count light节点数量
 * \return     int 0-成功，其它失败
 */
int config_put_light(p_config_light light, uint light_count)
{
    ESP_LOGI(TAG, "--beg--%s-----------------", __FUNCTION__);

    ESP_LOGI(TAG, "--update:%s", CONFIG_LIGHT_FILENAME);

    config_light_get_json(light, light_count, s_buff);

    FILE *fp = fopen(CONFIG_LIGHT_FILENAME, "wb+");

    if (fp == NULL)
    {
        ESP_LOGE(TAG, "fopen %s", CONFIG_LIGHT_FILENAME);
        return -7;
    }

    fwrite(s_buff, strlen(s_buff), 1, fp);
    fclose(fp);

    ESP_LOGI(TAG, "--end--%s-----------------", __FUNCTION__);
    return 0;
}
