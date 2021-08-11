#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_test_cfg.h"
#include "esp_test_http_cfg_page.h"

#define HTTP_CONTENT_200    "OK"


/**
 * \brief      配置http
 * \param[in]  char         *param          URL请求参数
 * \param[in]  p_config_http http           配置数据
 * \param[out] char         *content        数据体
 * \param[out] uint         *content_len    数据体长度
 * \return     200-成功，其它失败
 */
int http_cfg_http(char *param, p_config_http http, char *content, uint *content_len)
{
    char format[64];

    sprintf(format, "%s=%%d", CONFIG_HTTP_PORT);

    if (1 == sscanf(param, format, &(http->port))) // 函数将返回成功赋值的字段个数
    {
        config_put_http(http);

        strncpy(content, HTTP_CONTENT_200, *content_len);
        *content_len = sizeof(HTTP_CONTENT_200) - 1;
        return 200;
    }

    *content_len = snprintf(content, *content_len, "arg %s error", CONFIG_HTTP_PORT);
    ESP_LOGE(TAG, content);
    return 400;
}

/**
 * \brief      配置wifi
 * \param[in]  char         *param          URL请求参数
 * \param[in]  p_config_wifi wifi           配置数据
 * \param[out] char         *content        数据体
 * \param[out] uint         *content_len    数据体长度
 * \return     200-成功，其它失败
 */
int http_cfg_wifi(char *param, p_config_wifi wifi, char *content, uint *content_len)
{
    uint type;
    char ssid[32];
    char password[32];
    char format[64];

    // 没有做URL转义

    sprintf(format, "%s=%%d&%s=%%[^&]&%s=%%[^&]",
                     CONFIG_WIFI_TYPE,
                     CONFIG_WIFI_SSID,
                     CONFIG_WIFI_PASSWORD);

    if (3 == sscanf(param, format, &type, ssid, password))
    {
        if ((type == WIFI_TYPE_AP || type == WIFI_TYPE_STA) &&
            (0 != strcmp(ssid, "")) &&
            (0 != strcmp(password, "")))
        {
            wifi->type = type;
            strcpy(wifi->ssid, ssid);
            strcpy(wifi->password, password);

            config_put_wifi(wifi);

            strncpy(content, HTTP_CONTENT_200, *content_len);
            *content_len = sizeof(HTTP_CONTENT_200) -1;
            return 200;
        }
    }

    *content_len = snprintf(content,
                            *content_len,
                            "arg %s %s %s error",
                            CONFIG_WIFI_TYPE,
                            CONFIG_WIFI_SSID,
                            CONFIG_WIFI_PASSWORD);
    ESP_LOGE(TAG, content);
    return 400;
}

/**
 * \brief      添加light
 * \param[in]  p_config_light light          配置数据
 * \param[in]  uint           id             ID
 * \param[in]  char          *name           名称
 * \param[in]  uint           color          颜色
 * \param[in]  bool           on             是否打开
 * \param[out] char          *content        数据体
 * \param[out] uint          *content_len    数据体长度
 * \return     200-成功，其它失败
 */
int http_cfg_light_add(p_config_light light, uint id, char *name, uint color, bool on,
                       char *content, uint *content_len)
{
    int i;
    int count = light->count;
    bool have = false;

    if (count >= light->max)
    {
        *content_len = snprintf(content, *content_len, "light count >= max:%d", light->max);
        ESP_LOGE(TAG, content);
        return 400;
    }

    for (i = 0; i < count; i++)
    {
        if (light->item[i].id == id)
        {
            have = true;
            break;
        }
    }

    if (have)
    {
        *content_len = snprintf(content, *content_len, "already exists id:%d light", id);
        ESP_LOGE(TAG, content);
        return 400;
    }

    p_config_light_item item = &(light->item[count]);
    item->id    = id;
    item->on    = on;
    item->color = color;
    strcpy(item->name, name);
    light->count++;

    ESP_LOGE(TAG, "--------1----------");

    for (i = 0; i < light->count; i++)
    {
        item = &(light->item[i]);
        ESP_LOGE(TAG, "%s id:%d name:%s color:%d on:%d",
                      __FUNCTION__,
                      item->id,
                      item->name,
                      item->color,
                      item->on);
    }

    config_put_light(light);

    ESP_LOGE(TAG, "--------2----------");

    strncpy(content, HTTP_CONTENT_200, *content_len);
    *content_len = sizeof(HTTP_CONTENT_200) -1;
    return 200;
}

/**
 * \brief      删除light
 * \param[in]  char          *param          URL请求参数
 * \param[in]  p_config_light light          配置数据
 * \param[in]  uint           id             ID
 * \param[out] char          *content        数据体
 * \param[out] uint          *content_len    数据体长度
 * \return     200-成功，其它失败
 */
int http_cfg_light_del(p_config_light light, uint id,
                       char *content, uint *content_len)
{
    int i;
    int count = light->count;
    bool have = false;

    for (i = 0; i < count; i++)
    {
        if (!have && light->item[i].id == id)
        {
            have = true;
            light->count--;
            ESP_LOGE(TAG, "%s del light id:%d", __FUNCTION__, id);
        }
        else if (have)
        {
            light->item[i - 1] = light->item[i];
        }
    }

    if (!have)
    {
        *content_len = snprintf(content, *content_len, "don't have id:%d light", id);
        ESP_LOGE(TAG, content);
        return 400;
    }

    ESP_LOGE(TAG, "--------1----------");

    for (i = 0; i < light->count; i++)
    {
        ESP_LOGE(TAG, "%s id:%d", __FUNCTION__, light->item[i].id);
    }

    config_put_light(light);

    ESP_LOGE(TAG, "--------2----------");

    strncpy(content, HTTP_CONTENT_200, *content_len);
    *content_len = sizeof(HTTP_CONTENT_200) -1;
    return 200;
}

/**
 * \brief      修改light
 * \param[in]  char          *param          URL请求参数
 * \param[in]  p_config_light light          配置数据
 * \param[in]  uint           id             ID
 * \param[in]  char          *name           名称
 * \param[in]  uint           color          颜色
 * \param[in]  bool           on             是否打开
 * \param[out] char          *content        数据体
 * \param[out] uint          *content_len    数据体长度
 * \return     200-成功，其它失败
 */
int http_cfg_light_mod(p_config_light light, uint id, char *name, uint color, bool on,
                       char *content, uint *content_len)
{
    int i;
    int count = light->count;
    bool have = false;

    for (i = 0; i < count; i++)
    {
        if (light->item[i].id == id)
        {
            have = true;
            break;
        }
    }

    if (!have)
    {
        *content_len = snprintf(content, *content_len, "don't have id:%d light", id);
        ESP_LOGE(TAG, content);
        return 400;
    }

    light->item[i].on    = on;
    light->item[i].color = color;
    strcpy(light->item[i].name, name);

    ESP_LOGE(TAG, "--------1----------");

    for (i = 0; i < count; i++)
    {
        ESP_LOGE(TAG, "%s id:%d name:%s color:%d on:%d",
                      __FUNCTION__,
                      light->item[i].id,
                      light->item[i].name,
                      light->item[i].color,
                      light->item[i].on);
    }

    config_put_light(light);

    ESP_LOGE(TAG, "--------2----------");

    strncpy(content, HTTP_CONTENT_200, *content_len);
    *content_len = sizeof(HTTP_CONTENT_200) -1;
    return 200;
}

/**
 * \brief      配置light
 * \param[in]  char          *param         URL请求参数
 * \param[in]  p_config_light light         配置数据
 * \param[out] char          *content       数据体
 * \param[out] uint          *content_len   数据体长度
 * \return     200-成功，其它失败
 */
int http_cfg_light(char *param, p_config_light light, char *content, uint *content_len)
{
    uint id;
    bool on;
    uint cmd;
    uint color;
    char name[64];
    char format[64];

    sprintf(format, "%s=%%d&%s=%%d&%s=%%[^&]&%s=%%d&%s=%%d",
                     LIGHT_CMD_NAME,
                     CONFIG_LIGHT_ID,
                     CONFIG_LIGHT_NAME,
                     CONFIG_LIGHT_COLOR,
                     CONFIG_LIGHT_ON);

    if (5 == sscanf(param, format, &cmd, &id, name, &color, &on))
    {
        ESP_LOGI(TAG, "%s:%d %s:%d %s:%s %s:%d %s:%d",
                       LIGHT_CMD_NAME, cmd,
                       CONFIG_LIGHT_ID, id,
                       CONFIG_LIGHT_NAME, name,
                       CONFIG_LIGHT_COLOR, color,
                       CONFIG_LIGHT_ON, on);

        switch (cmd)
        {
            case LIGHT_CMD_ADD:
            {
                return http_cfg_light_add(light, id, name, color, on, content, content_len);
            }
            case LIGHT_CMD_DEL:
            {
                return http_cfg_light_del(light, id, content, content_len);
            }
            case LIGHT_CMD_MOD:
            {
                return http_cfg_light_mod(light, id, name, color, on, content, content_len);
            }
        }
    }

    *content_len = snprintf(content,
                            *content_len,
                            "arg %s %s %s %s %s error",
                            LIGHT_CMD_NAME,
                            CONFIG_LIGHT_ID,
                            CONFIG_LIGHT_NAME,
                            CONFIG_LIGHT_COLOR,
                            CONFIG_LIGHT_ON);

    ESP_LOGE(TAG, content);
    return 400;
}

/**
 * \brief      配置light.on
 * \param[in]  char          *param         请求参数
 * \param[in]  p_config_light light         配置数据
 * \param[out] char          *content       数据体
 * \param[out] uint          *content_len   数据体长度
 * \return     0-成功，其它失败
 */
int http_cfg_light_on(char *param, p_config_light light, char *content, uint *content_len)
{
    return 0;
}

/**
 * \brief      任务回调函数
 * \param[in]  void* pvParameters  参数
 * \return     无
 */
static void restart_task(void *pvParameters)
{
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "------------------2000ms--restart----------");
    esp_restart();
}

/**
 * \brief      重启
 * \param[out] char          *content       数据体
 * \param[out] uint          *content_len   数据体长度
 * \return     200-成功，其它失败
 */
int http_reboot(char *content, uint *content_len)
{
    xTaskCreate(restart_task, "restart_task", 4096, NULL, 5, NULL);
    strncpy(content, HTTP_CONTENT_200, *content_len);
    *content_len = sizeof(HTTP_CONTENT_200) - 1;
    ESP_LOGI(TAG, "------------------send reboot cmd----------");
    return 200;
}
