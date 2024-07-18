#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_app_http_cfg_page.h"

/// 配置HTML
#define HTTP_CONFIG_HTML_BEG "\
<form action='/cfg-wifi'>\
  <input name='" CONFIG_WIFI_SSID "' value='%s' maxlength='32'/><br>\
  <input name='" CONFIG_WIFI_PASSWORD "' value='%s' maxlength='32'/><br>\
  <select name='" CONFIG_WIFI_TYPE "'>\
    <option value='" STR(WIFI_TYPE_AP) "' %s>" WIFI_TYPE_AP_NAME "</option>\
    <option value='" STR(WIFI_TYPE_STA) "' %s>" WIFI_TYPE_STA_NAME "</option>\
  </select><br>\
  <input type='submit' value='wifi'/>\
</form>\
<form action='/cfg-mqtt'>\
  <input name='" CONFIG_MQTT_BROKER "' value='%s' maxlength='32'/><br>\
  <input name='" CONFIG_MQTT_USERNAME "' value='%s' maxlength='32'/><br>\
  <input name='" CONFIG_MQTT_PASSWORD "' value='%s' maxlength='32'/><br>\
  <input name='" CONFIG_MQTT_CLIENTID "' value='%s' maxlength='64' style=\"width:400px\"/><br>"
#define HTTP_CONFIG_HTML_MID "\
   <input name='" CONFIG_MQTT_TOPIC "' value='%s' maxlength='64' style=\"width:400px\"/><br>"
#define HTTP_CONFIG_HTML_END "\
  <input type='submit' value='mqtt'/>\
</form>\
<form action='/cfg-http'>\
  <input name='" CONFIG_HTTP_PORT "' value='%d' maxlength='5' type='number'/><br>\
  <input type='submit' value='http'/>\
</form>\
<form action='/cfg-light'>\
  <select name='" CONFIG_LIGHT_ON "'>\
    <option value='" STR(LIGHT_ON) "' %s>" LIGHT_ON_NAME "</option>\
    <option value='" STR(LIGHT_OFF) "' %s>" LIGHT_OFF_NAME "</option>\
    <option value='" STR(LIGHT_RUN) "' %s>" LIGHT_RUN_NAME "</option>\
  </select><br>\
  <input type='submit' value='light'/>\
</form>\
<form action='/reboot'>\
  <input type='submit' value='reboot'/>\
</form>\
<p>%d</p>\
<p>%s</p>\
<p>%s</p>"

extern char *g_msg0;  ///< 4G模块的mqtt通过AT指令发送来的消息
extern char *g_msg1;
extern int  g_msg_num;

/**
 *\brief        配置页面,模板HTTP_PATH_PROC
 *\param[in]    arg             URL请求参数
 *\param[in]    param           配置数据
 *\param[out]   content         数据体
 *\param[out]   content_len     输入缓冲区大小,输出数据体长度
 *\return       0               成功
 */
int http_cfg_page(const char *arg, void *param, char *content, unsigned int *content_len)
{
    p_config config = (p_config)param;
    p_config_wifi  wifi  = &(config->wifi);
    p_config_mqtt  mqtt  = &(config->mqtt);
    p_config_http  http  = &(config->http);
    p_config_light light = &(config->light);

    int len = snprintf(content,
                       *content_len,
                       HTTP_CONFIG_HTML_BEG,
                       wifi->ssid,
                       wifi->password,
                       (wifi->type == WIFI_TYPE_AP) ? "selected" : "",
                       (wifi->type == WIFI_TYPE_STA) ? "selected" : "",
                       mqtt->broker,
                       mqtt->username,
                       mqtt->password,
                       mqtt->clientid);

    for (int i = 0; i < mqtt->topic_count; i++)
    {
        len += snprintf(&content[len], *content_len - len, HTTP_CONFIG_HTML_MID, mqtt->topic[i]);
    }

    len += snprintf(&content[len],
                    *content_len - len,
                    HTTP_CONFIG_HTML_END,
                    http->port,
                    (light->on == LIGHT_ON) ? "selected" : "",
                    (light->on == LIGHT_OFF) ? "selected" : "",
                    (light->on == LIGHT_RUN) ? "selected" : "",
                    g_msg_num,
                    (NULL == g_msg0) ? "" : g_msg0,
                    (NULL == g_msg1) ? "" : g_msg1);

    *content_len = len;
    return 200;
}
