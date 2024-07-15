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
    uint type;                      ///< WIFI类型:0-AP,1-STATION
    char ssid[32];
    char password[32];

} t_config_wifi, *p_config_wifi;

typedef struct _config_mqtt
{
    char broker[32];                ///< broker地址,mqtt://broker.emqx.io
    char username[32];              ///< 用户名
    char password[32];              ///< 密码
    char clientid[64];              ///< 同一个用户名下可以有多个不同的client
    uint topic_count;               ///< 订阅主题数量
    char topic[MQTT_TOPIC_MAX][64]; ///< 主题
    void *queue[MQTT_TOPIC_MAX];    ///< 每个主题对应一个消息队列

} t_config_mqtt, *p_config_mqtt;

typedef struct _config_http
{
    uint port;                      ///< 端口

} t_config_http, *p_config_http;

typedef struct _config_light
{
    uint on;                        ///< 0-关,1-亮,2-闪

} t_config_light, *p_config_light;

typedef struct _config
{
    t_config_wifi   wifi;
    t_config_http   http;
    t_config_mqtt   mqtt;
    t_config_light  light;

} t_config, *p_config;

typedef struct _msg_head
{
    uint            len;
    char           *data;

} t_msg_head, *p_msg_head;

/**
 *\brief        初始化配置
 *\param[out]   config      配置数据
 *\return       0           成功
 */
int config_init(p_config config);

/**
 *\brief        生成wifi数据的JSON字符串,写入配置文件
 *\param[in]    wifi
 *\return       0           成功
 */
int config_put_wifi(p_config_wifi wifi);

/**
 *\brief        生成http数据的JSON字符串,写入配置文件
 *\param[in]    http
 *\return       0           成功
 */
int config_put_http(p_config_http http);

/**
 *\brief        生成mqtt数据的JSON字符串,写入配置文件
 *\param[in]    mqtt
 *\return       0           成功
 */
int config_put_mqtt(p_config_mqtt mqtt);

/**
 *\brief        生成light数据的JSON字符串,写入配置文件
 *\param[in]    light
 *\return       0           成功
 */
int config_put_light(p_config_light light);
