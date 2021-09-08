#pragma once // 只编译一次

#ifndef bool
#define bool  uint
#define true  1
#define false 0
#endif

#define _STR(s)         #s
#define STR(s)          _STR(s)
#define TAG             __FILE__ ":" STR(__LINE__)

#define CONFIG_HTTP_FILENAME    "/spiffs/http.cfg"
#define CONFIG_HTTP_PORT        "port"

#define CONFIG_WIFI_FILENAME    "/spiffs/wifi.cfg"
#define CONFIG_WIFI_TYPE        "type"
#define CONFIG_WIFI_SSID        "ssid"
#define CONFIG_WIFI_PASSWORD    "password"

#define CONFIG_LIGHT_FILENAME   "/spiffs/light.cfg"
#define CONFIG_LIGHT_ID         "id"
#define CONFIG_LIGHT_NAME       "name"
#define CONFIG_LIGHT_COLOR      "color"
#define CONFIG_LIGHT_ON         "on"

#define WIFI_TYPE_AP            0
#define WIFI_TYPE_STA           1
#define WIFI_TYPE_AP_NAME       "ap"
#define WIFI_TYPE_STA_NAME      "sta"

#define LIGHT_CMD_NAME          "cmd"
#define LIGHT_CMD_ADD           0
#define LIGHT_CMD_DEL           1
#define LIGHT_CMD_MOD           2
#define LIGHT_CMD_ADD_NAME      "add"
#define LIGHT_CMD_DEL_NAME      "del"
#define LIGHT_CMD_MOD_NAME      "mod"

#define LIGHT_COLOR_RED         0
#define LIGHT_COLOR_BLUE        2
#define LIGHT_COLOR_GREEN       1
#define LIGHT_COLOR_YELLOW      3
#define LIGHT_COLOR_RED_NAME    "red"
#define LIGHT_COLOR_BLUE_NAME   "blue"
#define LIGHT_COLOR_GREEN_NAME  "green"
#define LIGHT_COLOR_YELLOW_NAME "yellow"

#define LIGHT_ON_ON             1
#define LIGHT_ON_OFF            0
#define LIGHT_ON_ON_NAME        "on"
#define LIGHT_ON_OFF_NAME       "off"

#define LIGHT_MAX_COUNT         16

#define CPU_CLK_NAME            "clk"
#define CPU_INT0_NAME           "int0"
#define CPU_INT1_NAME           "int1"

typedef struct _config_http
{
    uint port;

}config_http_t, *p_config_http;

typedef struct _config_wifi
{
    uint type;          // WIFI类型:0-AP,1-STATION
    char ssid[32];
    char password[32];

}config_wifi_t, *p_config_wifi;

typedef struct _config_light_item
{
    bool on;            // 是否开灯
    uint id;            // ID应唯一
    uint color;         // 颜色:0-red,1-green,2-blue,3-yellow
    char name[32];      // 名称

}config_light_item_t, *p_config_light_item;

typedef struct _config_light
{
    uint max;           // 最大数量
    uint count;         // 节点数量
    config_light_item_t item[LIGHT_MAX_COUNT];

}config_light_t, *p_config_light;


/**
 * \brief      初始化配置
 * \param[in]  char *buff       缓存
 * \param[in]  uint size        缓存大小
 * \param[out] p_config_http    http
 * \param[out] p_config_wifi    wifi
 * \param[out] p_config_light   light
 * \return     int 0-成功，其它失败
 */
int config_init(char *buff, uint size, p_config_http http, p_config_wifi wifi, p_config_light light);

int config_http_get_data(const char *json, p_config_http http);

int config_http_get_json(const p_config_http http, char *json);

int config_wifi_get_data(const char *json, p_config_wifi wifi);

int config_wifi_get_json(const p_config_wifi wifi, char *json);

int config_light_get_data(const char *json, p_config_light light);

int config_light_get_json(const p_config_light light, char *json);

int config_get_data(p_config_http http, p_config_wifi wifi, p_config_light light);

int config_put_http(p_config_http http);

int config_put_wifi(p_config_wifi wifi);

int config_put_light(p_config_light light);
