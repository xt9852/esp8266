#pragma once // 只编译一次

#ifndef bool
#define bool  uint
#define true  1
#define false 0
#endif

#define _STR(s)                     #s
#define STR(s)                      _STR(s)
#define TAG                         __FILE__ ":" STR(__LINE__)

#define CONFIG_WIFI_KEY             "wifi"
#define CONFIG_WIFI_TYPE            "type"
#define CONFIG_WIFI_SSID            "ssid"
#define CONFIG_WIFI_PASSWORD        "password"

#define CONFIG_MQTT_KEY             "mqtt"
#define CONFIG_MQTT_BROKER          "broker"
#define CONFIG_MQTT_USERNAME        "username"
#define CONFIG_MQTT_PASSWORD        "password"
#define CONFIG_MQTT_CLIENTID        "clientid"
#define CONFIG_MQTT_TOPIC           "topic"

#define CONFIG_HTTP_KEY             "http"
#define CONFIG_HTTP_PORT            "port"

#define CONFIG_LIGHT_KEY            "light"
#define CONFIG_LIGHT_ON             "on"

#define WIFI_TYPE_AP                0
#define WIFI_TYPE_STA               1
#define WIFI_TYPE_AP_NAME           "ap"
#define WIFI_TYPE_STA_NAME          "sta"

#define MQTT_TOPIC_MAX              16

#define LIGHT_ON                    1
#define LIGHT_OFF                   0
#define LIGHT_RUN                   2
#define LIGHT_ON_NAME               "on"
#define LIGHT_OFF_NAME              "off"
#define LIGHT_RUN_NAME              "run"

#define CPU_CLK_NAME                "clk"
#define CPU_INT0_NAME               "int0"
#define CPU_INT1_NAME               "int1"

typedef struct _config_wifi
{
    uint type;                      // WIFI类型:0-AP,1-STATION
    char ssid[32];
    char password[32];

} config_wifi_t, *p_config_wifi;

typedef struct _config_mqtt
{
    char broker[32];                // broker地址,mqtt://broker.emqx.io
    char username[32];              // 用户名
    char password[32];              // 密码
    char clientid[64];              // 同一个用户名下可以有多个不同的client
    uint topic_count;               // 订阅主题数量
    char topic[MQTT_TOPIC_MAX][64]; // 主题
    void *queue[MQTT_TOPIC_MAX];    // 每个主题对应一个消息队列

} config_mqtt_t, *p_config_mqtt;

typedef struct _config_http
{
    uint port;                      // 端口

} config_http_t, *p_config_http;

typedef struct _config_light
{
    uint on;                        // 0-关,1-亮,2-闪

} config_light_t, *p_config_light;

typedef struct _config
{
    config_wifi_t   wifi;
    config_http_t   http;
    config_mqtt_t   mqtt;
    config_light_t  light;

} config_t, *p_config;

typedef struct _msg_head
{
    uint            len;
    char           *data;

} msg_head_t, *p_msg_head;


/**
 * \brief      初始化配置
 * \param[out] p_config     config      配置数据
 * \param[in]  char        *buff        缓存
 * \param[in]  uint         size        缓存大小
 * \return     int 0-成功，其它失败
 */
int config_init(p_config config, char *buff, uint size);

/**
 * \brief      生成JOSN字符串，写入配置文件
 * \param[in]  p_config_wifi wifi
 * \return     int 0-成功，其它失败
 */
int config_put_wifi(p_config_wifi wifi);

/**
 * \brief      生成JOSN字符串，写入配置文件
 * \param[in]  p_config_http http
 * \return     int 0-成功，其它失败
 */
int config_put_http(p_config_http http);

/**
 * \brief      生成JOSN字符串，写入配置文件
 * \param[in]  p_config_mqtt mqtt
 * \return     int 0-成功，其它失败
 */
int config_put_mqtt(p_config_mqtt mqtt);

/**
 * \brief      生成JOSN字符串，写入配置文件
 * \param[in]  p_config_light light
 * \return     int 0-成功，其它失败
 */
int config_put_light(p_config_light light);
