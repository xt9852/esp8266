#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_spiffs.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "cJSON.h"

#ifndef bool
#define bool  uint
#define true  1
#define false 0
#endif

#define CONFIG_HTTP_FILENAME    "/spiffs/http.cfg"
#define CONFIG_HTTP_PORT        "port"

#define CONFIG_WIFI_FILENAME    "/spiffs/wifi.cfg"
#define CONFIG_WIFI_TYPE        "type"
#define CONFIG_WIFI_SSID        "ssid"
#define CONFIG_WIFI_PASSWORD    "password"
#define WIFI_TYPE_AP            0
#define WIFI_TYPE_STA           1

#define CONFIG_LIGHT_FILENAME   "/spiffs/light.cfg"
#define CONFIG_LIGHT_ID         "id"
#define CONFIG_LIGHT_ON         "on"
#define CONFIG_LIGHT_NAME       "name"
#define CONFIG_LIGHT_COLOR      "color"
#define LIGHT_MAX_COUNT         16
#define LIGHT_MAX_COLOR_COUNT   4

#define EVENT_BIT_CONNECTED     BIT0
#define EVENT_BIT_FAIL          BIT1

#define HTTP_HEAD_200 "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: "
#define HTTP_FILE_200 "200"
#define HTTP_HEAD_404 "HTTP/1.1 404 Not Found\nContent-Type: text/html\nContent-Length: "
#define HTTP_FILE_404 "404"

#define HTTP_FILE_INDEX "\
<meta http-equiv='Content-Type' content='charset=utf-8'/>\
<style>\
div{margin:5;padding:0 20;background:green;float:left;}\
</style>\
<script>\
function load()\
{\
req = new XMLHttpRequest();\
req.open('GET', '/index');\
req.send(null);\
req.onload=function()\
{\
if(req.readyState!=4 || req.status!=200){console.log(req.status);return;}\
rp=JSON.parse(req.responseText);\
t1=document.getElementById('t1');\
t2=document.getElementById('t2');\
t3=document.getElementById('t3');\
t4=document.getElementById('t4');\
t5=document.getElementById('t5');\
t6=document.getElementById('t6');\
t7=document.getElementById('t7');\
t8=document.getElementById('t8');\
t1.innerText='温度：'+rp['t1']+'\u2103';\
t2.innerText='湿度：'+rp['t2']+'%';\
t3.innerText='水表：'+rp['t3'];\
t4.innerText='电表：'+rp['t4'];\
t5.innerText='气表：'+rp['t5'];\
t6.innerText='电压：'+rp['t6']+'V';\
t7.innerText='电流：'+rp['t7']+'A';\
t8.innerText='功率：'+rp['t6']*rp['t7']+'W';\
}\
}\
</script>\
<body onload='load()'>\
<div>\
<h3 id='t1'/>\
<h3 id='t2'/>\
</div>\
<div>\
<h3 id='t3'/>\
<h3 id='t4'/>\
<h3 id='t5'/>\
</div>\
<div>\
<h3 id='t6'/>\
<h3 id='t7'/>\
<h3 id='t8'/>\
</div>\
</body>"

#define HTTP_FILE_LIGHT "\
<meta http-equiv='Content-Type' content='charset=utf-8'/>\
<style>\
div{width:43;height:43;border-radius:50%;background:green;float:left;cursor:pointer;}\
</style>\
<script>\
color=['red','green','blue','yellow'];\
function load()\
{\
req=new XMLHttpRequest();\
req.open('GET', '/light');\
req.send(null);\
req.onload=function()\
{\
if(req.readyState!=4 || req.status!=200){console.log(req.status);return;} \
rp=JSON.parse(req.responseText);\
console.log(req.responseText);\
s1=document.getElementById('s1');\
n1=document.getElementById('n1');\
bd=s1.parentNode;\
for(id in rp)\
{\
light=rp[id];\
s0=s1.cloneNode(true);\
n0=n1.cloneNode(true);\
s0.id=light['id'];\
s0.style.background=(light['on']==0)?'gray':color[light['color']];\
n0.innerText=light['name'];\
bd.appendChild(s0);\
bd.appendChild(n0);\
}\
s1.remove();\
n1.remove();\
}\
}\
function on(div)\
{\
url='/update_light_on?id='+div.id+'&on='+((div.style.background=='gray')?'on':'off');\
req=new XMLHttpRequest();\
req.open('GET', url);\
req.send(null);\
req.onload = function()\
{\
if(req.readyState==4&&req.status==200)\
{\
div.style.background=(div.style.background=='gray')?color[Number(req.responseText)]:'gray';\
}\
}\
}\
</script>\
<body onload='load()'>\
<div id='s1' onclick='on(this)'/><h1 id='n1'/>\
</body>"


#define HTTP_FILE_CPU "\
<meta http-equiv='Content-Type' content='charset=utf-8'/>\
<style>\
div{margin:5;padding:0 20;background:green;float:left;}\
.k{margin:0 20 19 0;width:25;border-radius:50%;float:left;background:gray;cursor:pointer;}\
.p{margin:0 20 19 0;width:25;border-radius:50%;float:left;}\
</style>\
<script>\
function load()\
{\
req=new XMLHttpRequest();\
req.open('GET', '/cpu');\
req.send(null);\
req.onload=function()\
{\
if(req.readyState!=4 || req.status!=200){console.log(req.status);return;}\
txt=['SEL0选取下一条地址为微指令地址','SEL1选取0x[IR]0组成微指令地址',\
'SEL2选取中断微指令地址0x011','SEL3选取条件跳转微指令地址0x000'];\
opr=['gray','blue','yellow','red','Aqua','yellow','Magenta','yellow'];\
int=['gray','red'];\
rp=JSON.parse(req.responseText);\
mi_addr=document.getElementById('mi_addr');\
mi_next=document.getElementById('mi_next');\
mi_true=document.getElementById('mi_true');\
data=document.getElementById('bus_data');\
addr=document.getElementById('bus_addr');\
out_alu=document.getElementById('out_alu');\
out_shf=document.getElementById('out_shf');\
sel=document.getElementById('sel');\
sel_int=document.getElementById('sel_int');\
sel_ir=document.getElementById('sel_ir');\
sel_jp=document.getElementById('sel_jp');\
sel_je=document.getElementById('sel_je');\
sel_jb=document.getElementById('sel_jb');\
sel_jl=document.getElementById('sel_jl');\
alu=document.getElementById('alu');\
msw=document.getElementById('msw');\
ir=document.getElementById('ir');\
ip=document.getElementById('ip');\
sp=document.getElementById('sp');\
ar=document.getElementById('ar');\
br=document.getElementById('br');\
tr=document.getElementById('tr');\
mi_addr.innerText='指令当前地址：'+rp['mi_addr'];\
mi_next.innerText='指令下条地址：'+rp['mi_addr_next'];\
mi_true.innerText='真实下条地址：'+rp['mi_addr_next_true'];\
out_alu.innerText='逻辑输出：'+rp['out_alu'];\
out_shf.innerText='移位输出：'+rp['out_shf'];\
bus_data.innerText='数据总线：'+rp['bus_data'];\
bus_addr.innerText='地址总线：'+rp['bus_addr'];\
sel.innerText=txt[rp['sel']];\
sel_int.style.background=int[rp['sel_ir']];\
sel_ir.style.background=int[rp['sel_ir']];\
sel_jp.style.background=int[rp['sel_jp']];\
sel_je.style.background=int[rp['sel_je']];\
sel_jb.style.background=int[rp['sel_jb']];\
sel_jl.style.background=int[rp['sel_jl']];\
alu.style.background=opr[rp['alu']];\
msw.style.background=opr[rp['msw']];\
ir.style.background=opr[rp['ir']];\
ip.style.background=opr[rp['ip']];\
sp.style.background=opr[rp['sp']];\
ar.style.background=opr[rp['ar']];\
br.style.background=opr[rp['br']];\
tr.style.background=opr[rp['tr']];\
}\
}\
</script>\
<body onload='load()'>\
<div>\
<h3 id='mi_addr'/>\
<h3 id='mi_next'/>\
<h3 id='mi_true'/>\
<h3 id='clk' class='k'>clk</h3>\
<h3 id='int0' class='k'>int0</h3>\
<h3 id='int1' class='k'>int1</h3>\
</div>\
<div>\
<h3 id='bus_data'/>\
<h3 id='bus_addr'/>\
<h3 id='out_alu'/>\
<h3 id='out_shf'/>\
</div>\
<div>\
<h3 id='sel'/>\
<h3 id='sel_int' class='p'>int</h3>\
<h3 id='sel_ir' class='p'>ir</h3>\
<h3 id='sel_jp' class='p'>jp</h3>\
<h3 id='sel_je' class='p'>je</h3>\
<h3 id='sel_jb' class='p'>jb</h3>\
<h3 id='sel_jl' class='p'>jl</h3>\
<h3 id='msw' class='p' style='clear:left'>mw</h3>\
<h3 id='ir' class='p'>ir</h3>\
<h3 id='ip' class='p'>ip</h3>\
<h3 id='sp' class='p'>sp</h3>\
<h3 id='ar' class='p'>ar</h3>\
<h3 id='br' class='p'>br</h3>\
<h3 id='tr' class='p'>tr</h3>\
<h3 id='alu' class='p' style='clear:left'>alu</h3>\
</div>\
</body>"


/*

function set(name, data, node){\
s = '';\
k = new Array();\
c = document.getElementById('c');\
a = document.getElementById(node);\
n = document.getElementById(name + 'n');\
b = c.parentNode;\
for (var key in data)\
{\
k.push(key);\
s += data[key];\
}\
if (n)\
{\
    for (i = 0; i < s.length; i += 4)\
    {\
        n.innerText += hex[s.substring(i, i + 4)];\
    }\
    n.innerText += 'H';\
}\
else\
{\
    k.sort();\
}\
for (var i in k)\
{\
    id = k[i];\
    t = c.cloneNode(true);\
    t.id = name + i;\
    t.innerText = id;\
    t.style.float = 'left';\
    t.style.width = '22px';\
    t.style.height = '22px';\
    t.style['margin-left'] = '5px';\
    t.style['border-radius'] = '50%';\
    t.style['text-align'] = 'center';\
    t.style.background = (data[id] == 1 ? 'red' : 'gray');\
    b.insertBefore(t, a);\
}\
}\

var hex = {'0000':'0','0001':'1','0010':'2','0011':'3','0100':'4','0101':'5','0110':'6','0111':'7',\
'1000':'8','1001':'9','1010':'A','1011':'B','1100':'C','1101':'D','1110':'E','1111':'F'}\

request = new XMLHttpRequest()\
request.open('GET', '/cpu')\
request.send(null)\
request.onload = function(){\
if (request.readyState != 4 || request.status != 200){console.log(request.status);return;}\
r = JSON.parse(request.responseText)\
set('db',  r.db,  'ab');\
set('ab',  r.ab,  'fb');\
set('fb',  r.fb,  'sb');\
set('sb',  r.sb,  'tb');\
<d id='c'></d>\
<d id='db'  class='t0'>数据总线:</d><d id='dbn' class='nm'>0x0123</d>\
<d id='ab'  class='t0'>地址总线:</d><d id='abn' class='nm'>0x4567</d>\
<d id='fb'  class='t0'>ALU输出: </d><d id='fbn' class='nm'>0x89AB</d>\
<d id='sb'  class='t0'>移位输出:</d><d id='sbn' class='nm'>0xCDEF</d>\


set('tb',  r.tb,  'ib');\
set('ib',  r.ib,  'mb');\
set('mb',  r.mb,  'mia');\
set('mia', r.mia, 'min');\
set('min', r.min, 'sel');\
set('sel', r.sel, 'ir');\
set('ir',  r.ir,  'msw');\
set('msw', r.msw, 'ip');\
set('ip',  r.ip,  'sp');\
set('sp',  r.sp,  'ar');\
set('ar',  r.ar,  'br');\
set('br',  r.br,  'tr');\
set('tr',  r.tr,  'alu');\
set('alu', r.alu, 'mem');\
set('mem', r.mem, 'jne');\
set('jne', r.jne, 'jmp');\
set('jmp', r.jmp, 'ses');\
set('ses', r.ses, 'clk');\
set('clk', r.clk, 'int');\
set('int', r.int, 'end');\
<d id='tb'  class='t0'>临时总线:</d><d id='tbn' class='nm'></d>\
<d id='ib'  class='t0'>指令总线:</d><d id='ibn' class='nm'></d>\
<d id='mb'  class='t0'>内存数据:</d><d id='mbn' class='nm'></d>\
<d id='mia' class='t0'>当前地址:</d><d id='mian'class='nm'></d>\
<d id='min' class='t0'>下条地址:</d><d id='minn'class='nm'></d>\
<d id='sel' class='t1'>微指令SEL</d><d id='ir'  class='t1'>微指令IR</d>\
<d id='msw' class='t1'>微指令MSW</d><d id='ip'  class='t1'>微指令IP</d>\
<d id='sp'  class='t1'>微指令SP</d><d id='ar'  class='t1'>微指令AR</d>\
<d id='br'  class='t1'>微指令BR</d><d id='tr'  class='t1'>微指令TR</d>\
<d id='alu' class='t1'>微指令ALU</d><d id='mem' class='t1'>微指令MEM</d>\
<d id='jne' class='t1'>ALU不等信号</d><d id='jmp' class='t1'>跳转信号</d>\
<d id='ses' class='t1'>选择器信号</d><d id='clk' class='t1'>心跳信号</d>\
<d id='int' class='t1'>中断信号</d>\
*/

#define HTTP_FILE_CONFIG "\
<form action='/update_http'>\
<input name='port' value='80'/>\
<input type='submit' value='OK'/>\
</form>\
<form action='/update_wifi'>\
<input name='net' value='AP/STA'/>\
<input name='ssid' value='ssid'/>\
<input name='pwd' value='pwd'/>\
<input type='submit' value='OK'/>\
</form>\
<form action='/update_light'>\
<input name='cmd' value='add/del/mod'/>\
<input name='id' value='id'/>\
<input name='name' value='name'/>\
<input name='color' value='red/green/blue/yellow'/>\
<input name='on' value='on/off'/>\
<input type='submit' value='OK'/>\
</form>\
<form action='/reboot'>\
<input type='submit' value='reboot'/>\
</form>"


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

typedef struct _config_light
{
    bool on;
    uint color;         // 0-red,1-green,2-blue,3-yellow
    char id[8];         // 应唯一
    char name[32];

}config_light_t, *p_config_light;

static cJSON_Hooks          s_hooks;
static EventGroupHandle_t   s_wifi_event_group;

static const char          *TAG = "esp_test";
static config_http_t        s_config_http = { 80 };
static config_wifi_t        s_config_wifi = { WIFI_TYPE_STA, "ChinaNet-EUG9", "xt9852@dx.com" };
static config_light_t       s_config_light[LIGHT_MAX_COUNT];
static uint                 s_config_light_count;
static char                 s_color_map[][7] = { "red", "green", "blue", "yellow" };


/**
 * \brief      任务回调函数
 * \param[in]  void* pvParameters  参数
 * \return     无
 */
static void restart_task(void *pvParameters)
{
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "---------------------esp_restart()---------------------");
    esp_restart();
}

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
 * \brief      解析配置JSON字符串
 * \param[in]  const char *json     JSON字符串
 * \param[out] p_config_http conf   配置数据
 * \return     int 0-成功，其它失败
 */
int parse_config_http_json(const char *json, p_config_http http)
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

    ESP_LOGI(TAG, "config http port:%d", http->port);

    cJSON_Delete(root);
    return 0;
}

/**
 * \brief      生成JSON字符串
 * \param[in]  const p_config_http http     配置数据
 * \param[out] char *json                   JSON字符串
 * \return     int 0-成功，其它失败
 */
int create_config_http_json(const p_config_http http, char *json)
{
    cJSON *root = cJSON_CreateObject();

    if (NULL == root)
    {
        ESP_LOGE(TAG, "Failed create json");
        return -1;
    }

    cJSON_AddItemToObject(root, CONFIG_HTTP_PORT, cJSON_CreateNumber(http->port));

    int ret = 0;
    char *data = cJSON_Print(root);

    if (NULL != data)
    {
        strcpy(json, data);
        cJSON_free(data);
        ESP_LOGI(TAG, "create http %s", json);
    }
    else
    {
        ret = -1;
        ESP_LOGE(TAG, "Failed cJSON_Print");

        sprintf(json, "{\"%s\":%d}", CONFIG_HTTP_PORT, http->port);
        ESP_LOGI(TAG, "--test--create http %s", json);
    }

    cJSON_Delete(root);
    return ret;
}

/**
 * \brief      解析配置JSON字符串
 * \param[in]  char *json           JSON字符串
 * \param[out] p_config_wifi wifi   配置数据
 * \return     int 0-成功，其它失败
 */
int parse_config_wifi_json(const char *json, p_config_wifi wifi)
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

    ESP_LOGI(TAG, "config wifi type:%s ssid:%s password:%s",
                 (wifi->type == WIFI_TYPE_AP ? "AP" : "STA" ),
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
int create_config_wifi_json(const p_config_wifi wifi, char *json)
{
    cJSON *root = cJSON_CreateObject();

    if (NULL == root)
    {
        ESP_LOGE(TAG, "Failed create json");
        return -1;
    }

    cJSON_AddItemToObject(root, CONFIG_WIFI_TYPE,     cJSON_CreateNumber(wifi->type));
    cJSON_AddItemToObject(root, CONFIG_WIFI_SSID,     cJSON_CreateString(wifi->ssid));
    cJSON_AddItemToObject(root, CONFIG_WIFI_PASSWORD, cJSON_CreateString(wifi->password));

    int ret = 0;
    char *data = cJSON_Print(root);

    if (NULL != data)
    {
        strcpy(json, data);
        cJSON_free(data);
        ESP_LOGI(TAG, "create wifi %s", json);
    }
    else
    {
        ret = -1;
        ESP_LOGE(TAG, "Failed cJSON_Print");

        sprintf(json, "{\"%s\":%d,\"%s\":\"%s\",\"%s\":\"%s\"}",
                      CONFIG_WIFI_TYPE, wifi->type,
                      CONFIG_WIFI_SSID, wifi->ssid,
                      CONFIG_WIFI_PASSWORD, wifi->password);

        ESP_LOGI(TAG, "--test--create wifi %s", json);
    }

    cJSON_Delete(root);
    return ret;
}

/**
 * \brief      解析配置JSON字符串
 * \param[in]  char *json           JSON字符串
 * \param[out] p_config_light light 配置数据
 * \return     int 0-成功，其它失败
 */
int parse_config_light_json(const char *json, p_config_light light)
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

    s_config_light_count = cJSON_GetArraySize(root); /*获取数组长度*/

    if (s_config_light_count > LIGHT_MAX_COUNT)
    {
        s_config_light_count = LIGHT_MAX_COUNT;
    }

    for (i = 0; i < s_config_light_count; i++)
    {
        item = cJSON_GetArrayItem(root, i);

        id = cJSON_GetObjectItem(item, CONFIG_LIGHT_ID);

        if (NULL == id)
        {
            ESP_LOGE(TAG, "Failed json no %s", CONFIG_LIGHT_ID);
            return -2;
        }

        if (NULL == id->valuestring)
        {
            ESP_LOGE(TAG, "Failed json %s type error", CONFIG_LIGHT_ID);
            return -3;
        }

        strncpy(light->id, id->valuestring, sizeof(light->id) - 1);


        name = cJSON_GetObjectItem(item, CONFIG_LIGHT_NAME);

        if (NULL == name)
        {
            ESP_LOGE(TAG, "Failed json no %s", CONFIG_LIGHT_NAME);
            return -4;
        }

        if (NULL == name->valuestring)
        {
            ESP_LOGE(TAG, "Failed json %s type error", CONFIG_LIGHT_NAME);
            return -5;
        }

        strncpy(light->name, name->valuestring, sizeof(light->name) - 1);


        color = cJSON_GetObjectItem(item, CONFIG_LIGHT_COLOR);

        if (NULL == color)
        {
            ESP_LOGE(TAG, "Failed json no %s", CONFIG_LIGHT_COLOR);
            return -6;
        }

        light->color = color->valueint;


        on = cJSON_GetObjectItem(item, CONFIG_LIGHT_ON);

        if (NULL == on)
        {
            ESP_LOGE(TAG, "Failed json no %s", CONFIG_LIGHT_ON);
            return -7;
        }

        light->on = on->valueint;

        ESP_LOGI(TAG, "config light id:%s name:%s color:%s on:%d",
                  light->id,
                  light->name,
                  s_color_map[light->color],
                  light->on);

        light++;
    }

    cJSON_Delete(root);
    return 0;
}

/**
 * \brief      生成JSON字符串
 * \param[in]  const p_config_light light 配置数据
 * \param[out] char *json           JSON字符串
 * \return     int 0-成功，其它失败
 */
int create_config_light_json(const p_config_light light, char *json)
{
    p_config_light light_tmp1 = light;
    cJSON *root = cJSON_CreateArray();

    if (NULL == root)
    {
        ESP_LOGE(TAG, "Failed create json");
        return -1;
    }

    int i;

    for (i = 0; i < s_config_light_count; i++)
    {
        cJSON *item = cJSON_CreateObject();

        if (NULL == root)
        {
            ESP_LOGE(TAG, "Failed create json item");
            return -2;
        }

        cJSON_AddItemToObject(item, CONFIG_LIGHT_ID,    cJSON_CreateString(light_tmp1->id));
        cJSON_AddItemToObject(item, CONFIG_LIGHT_ON,    cJSON_CreateNumber(light_tmp1->on));
        cJSON_AddItemToObject(item, CONFIG_LIGHT_NAME,  cJSON_CreateString(light_tmp1->name));
        cJSON_AddItemToObject(item, CONFIG_LIGHT_COLOR, cJSON_CreateNumber(light_tmp1->color));

        cJSON_AddItemToArray(root, item);

        light_tmp1++;
    }

    int ret = 0;
    char *data = cJSON_Print(root);

    if (NULL != data)
    {
        strcpy(json, data);
        cJSON_free(data);
        ESP_LOGI(TAG, "create light %s", json);
    }
    else
    {
        ret = 2;
        ESP_LOGE(TAG, "Failed cJSON_Print");

        json[0] = '[';
        json[1] = '\0';
        int count = 1;
        p_config_light light_tmp2 = light;

        for (i = 0; i < s_config_light_count; i++)
        {
            count += sprintf(&json[count],
                      "{\"%s\":\"%s\",\"%s\":%d,\"%s\":\"%s\",\"%s\":%d},",
                      CONFIG_LIGHT_ID,    light_tmp2->id,
                      CONFIG_LIGHT_ON,    light_tmp2->on,
                      CONFIG_LIGHT_NAME,  light_tmp2->name,
                      CONFIG_LIGHT_COLOR, light_tmp2->color);

            light_tmp2++;
        }

        json[count - 1] = ']';
        json[count] = '\0';

        ESP_LOGI(TAG, "--test--create light %s", json);
    }

    cJSON_Delete(root);
    return ret;
}

/**
 * \brief      初始化JSON
 * \return     无
 */
void init_json()
{
    s_hooks.free_fn = free;
    s_hooks.malloc_fn = malloc;
    cJSON_InitHooks(&s_hooks);
}

/**
 * \brief      读取配置文件，内容为JSON字符串，解析JOSN
 * \param[out] p_config_http *http
 * \param[out] p_config_wifi *wifi
 * \param[out] p_config_light *light
 * \return     int 0-成功，其它失败
 */
esp_err_t get_config_data(p_config_http http, p_config_wifi wifi, p_config_light light)
{
    ESP_LOGI(TAG, "get_config_data");

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

    // http
    ESP_LOGI(TAG, "open:%s", CONFIG_HTTP_FILENAME);

    FILE *fp = fopen(CONFIG_HTTP_FILENAME, "rb+");

    if (fp == NULL)
    {
        esp_vfs_spiffs_unregister(NULL);
        ESP_LOGE(TAG, "Failed to open %s", CONFIG_HTTP_FILENAME);
        return -5;
    }

    char json[1024];
    fgets(json, sizeof(json) -1, fp);
    parse_config_http_json(json, http);
    fclose(fp);

    // wifi
    ESP_LOGI(TAG, "open:%s", CONFIG_WIFI_FILENAME);

    fp = fopen(CONFIG_WIFI_FILENAME, "rb+");

    if (fp == NULL)
    {
        esp_vfs_spiffs_unregister(NULL);
        ESP_LOGE(TAG, "Failed to open %s", CONFIG_WIFI_FILENAME);
        return -6;
    }

    fgets(json, sizeof(json) -1, fp);
    parse_config_wifi_json(json, wifi);
    fclose(fp);

    // light
    ESP_LOGI(TAG, "open:%s", CONFIG_LIGHT_FILENAME);

    fp = fopen(CONFIG_LIGHT_FILENAME, "rb+");

    if (fp == NULL)
    {
        esp_vfs_spiffs_unregister(NULL);
        ESP_LOGE(TAG, "Failed to open %s", CONFIG_LIGHT_FILENAME);
        return -7;
    }

    fgets(json, sizeof(json) -1, fp);
    parse_config_light_json(json, light);
    fclose(fp);

    esp_vfs_spiffs_unregister(NULL);
    return ret;
}

/**
 * \brief      生成JOSN字符串，写入配置文件
 * \param[in]  p_config_http *http
 * \param[in]  p_config_wifi *wifi
 * \param[in]  p_config_light *light
 * \return     int 0-成功，其它失败
 */
int put_config_data(p_config_http http, p_config_wifi wifi, p_config_light light)
{
    ESP_LOGI(TAG, "put_config_data");

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

    if (NULL != http)
    {
        ESP_LOGI(TAG, "create:%s", CONFIG_HTTP_FILENAME);

        FILE *fp = fopen(CONFIG_HTTP_FILENAME, "wb+");

        if (fp == NULL)
        {
            esp_vfs_spiffs_unregister(NULL);
            ESP_LOGE(TAG, "Failed to create %s", CONFIG_HTTP_FILENAME);
            return -5;
        }

        char json[1024];
        create_config_http_json(http, json);
        fwrite(json, strlen(json), 1, fp);
        fclose(fp);
    }

    if (NULL != wifi)
    {
        ESP_LOGI(TAG, "create:%s", CONFIG_WIFI_FILENAME);

        FILE *fp = fopen(CONFIG_WIFI_FILENAME, "wb+");

        if (fp == NULL)
        {
            esp_vfs_spiffs_unregister(NULL);
            ESP_LOGE(TAG, "Failed to create %s", CONFIG_WIFI_FILENAME);
            return -6;
        }

        char json[1024];
        create_config_wifi_json(wifi, json);
        fwrite(json, strlen(json), 1, fp);
        fclose(fp);
    }

    if (NULL != light)
    {
        ESP_LOGI(TAG, "create:%s", CONFIG_LIGHT_FILENAME);

        FILE *fp = fopen(CONFIG_LIGHT_FILENAME, "wb+");

        if (fp == NULL)
        {
            esp_vfs_spiffs_unregister(NULL);
            ESP_LOGE(TAG, "Failed to create %s", CONFIG_LIGHT_FILENAME);
            return -7;
        }

        char json[1024];
        create_config_light_json(light, json);
        fwrite(json, strlen(json), 1, fp);
        fclose(fp);
    }

    esp_vfs_spiffs_unregister(NULL);
    return 0;
}

/**
 * \brief      初始化WIFI
 * \param[in]  p_config_data conf   配置数据
 * \return     无
 */
void init_wifi(p_config_wifi wifi)
{
    ESP_LOGI(TAG, "init_wifi...");

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
}

#define GPIO_LED_NUM 2

/**
 * \brief      初始化GPIO,1秒间隔闪灯
 * \return     无
 */
void init_gpio()
{
    gpio_config_t gpio_conf;
    gpio_conf.pin_bit_mask = (1ULL << GPIO_LED_NUM);/* 选择gpio2 */
    gpio_conf.mode = GPIO_MODE_OUTPUT;              /* 输出模式 */
    gpio_conf.pull_up_en = 0;                       /* 不上拉 */
    gpio_conf.pull_down_en = 0;                     /* 不下拉 */
    gpio_conf.intr_type = GPIO_INTR_DISABLE;        /* 禁止中断 */
    gpio_config(&gpio_conf);

    while(1)
    {
        gpio_set_level(GPIO_LED_NUM, 0);         /* 熄灭 */
        vTaskDelay(3000 / portTICK_PERIOD_MS);   /* 延时500ms*/
        gpio_set_level(GPIO_LED_NUM, 1);         /* 点亮 */
        vTaskDelay(3000 / portTICK_PERIOD_MS);   /* 延时500ms*/
    }
}

#define CPU_GPIO_QH     16
#define CPU_GPIO_LD     5
#define CPU_GPIO_CLK    4
#define GPIO_OUTPUT_PIN_SEL  ((1<<CPU_GPIO_LD) | (1<<CPU_GPIO_CLK))

/**
 * \brief      读取74ls165数据,组装成数据
 * \param[in]  int count   组装数据的个数
 * \return     数据
 */
int get_gpio(int count)
{
    int i;
    unsigned int b;
    unsigned int data = 0;

    for (i = 0; i < count; i++)
    {
        b = gpio_get_level(CPU_GPIO_QH);            // 读取数据
        vTaskDelay(2 / portTICK_PERIOD_MS);
        gpio_set_level(CPU_GPIO_CLK, 0);            // 上升沿输出已存入寄存器的数据
        vTaskDelay(2 / portTICK_PERIOD_MS);
        gpio_set_level(CPU_GPIO_CLK, 1);
        vTaskDelay(2 / portTICK_PERIOD_MS);
        data |= (b << i);
    }

    return data;
}


/**
 * \brief      初始化GPIO,读取74ls165数据
 * \return     无
 */
int get_cpu_gpio(char *data)
{
    gpio_config_t gpio_conf;
    gpio_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    gpio_conf.mode = GPIO_MODE_OUTPUT;              // 输出模式
    gpio_conf.intr_type = GPIO_INTR_DISABLE;        // 禁止中断
    gpio_conf.pull_up_en = 0;                       // 不上拉
    gpio_conf.pull_down_en = 0;                     // 不下拉
    gpio_config(&gpio_conf);

    gpio_conf.pin_bit_mask = (1ULL << CPU_GPIO_QH);
    gpio_conf.mode = GPIO_MODE_INPUT;
    gpio_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_conf.pull_up_en = 0;
    gpio_conf.pull_down_en = 0;
    gpio_config(&gpio_conf);

    int mi_addr = 0;
    int mi_addr_next = 0;
    int mi_addr_next_true = 0;
    int bus_data = 0;
    int bus_addr = 0;
    int out_alu = 0;
    int out_shf = 0;
    int sel_int = 0;
    int sel_ir = 0;
    int sel_jp = 0;
    int sel_je = 0;
    int sel_jb = 0;
    int sel_jl = 0;
    int sel = 0;
    int alu = 0;
    int msw = 0;
    int ir = 0;
    int ip = 0;
    int sp = 0;
    int ar = 0;
    int br = 0;
    int tr = 0;

    gpio_set_level(CPU_GPIO_LD, 0);             // 将并行数据存入寄存器
    vTaskDelay(2 / portTICK_PERIOD_MS);
    gpio_set_level(CPU_GPIO_LD, 1);             // 设置为读取模式
    vTaskDelay(2 / portTICK_PERIOD_MS);

    mi_addr = get_gpio(12);
    mi_addr_next = get_gpio(12);
    mi_addr_next_true = get_gpio(12);
    bus_data = get_gpio(16);
    bus_addr = get_gpio(16);
    out_alu = get_gpio(16);
    out_shf = get_gpio(16);
    sel_int = get_gpio(1);
    sel_ir = get_gpio(1);
    sel_jp = get_gpio(1);
    sel_je = get_gpio(1);
    sel_jb = get_gpio(1);
    sel_jl = get_gpio(1);
    sel = get_gpio(2);
    alu = 0;//get_gpio(2);
    msw = 1;//get_gpio(3);
    ir = 2;//get_gpio(2);
    ip = 3;//get_gpio(3);
    sp = 4;//get_gpio(3);
    ar = 5;//get_gpio(3);
    br = 6;//get_gpio(3);
    tr = 7;//get_gpio(3);

    return sprintf(data, "{\
\"mi_addr\":\"0x%03X\",\"mi_addr_next\":\"0x%03X\",\"mi_addr_next_true\":\"0x%03X\",\
\"bus_data\":\"0x%04X\",\"bus_addr\":\"0x%04X\",\
\"out_alu\":\"0x%04X\",\"out_shf\":\"0x%04X\",\
\"sel\":%d,\"sel_int\":%d,\"sel_ir\":%d,\"sel_jp\":%d,\"sel_je\":%d,\"sel_jb\":%d,\"sel_jl\":%d,\
\"alu\":%d,\"msw\":%d,\"ir\":%d,\"ip\":%d,\
\"sp\":%d,\"ar\":%d,\"br\":%d,\"tr\":%d}",
                             mi_addr, mi_addr_next, mi_addr_next_true,
                             bus_data, bus_addr, out_alu, out_shf,
                             sel, sel_int, sel_ir, sel_jp, sel_je, sel_jb, sel_jl,
                             alu, msw, ir, ip, sp, ar, br, tr);
}

#define CONFIG_EXAMPLE_IPV4

/**
 * \brief      创建监听socket
 * \param[in]  int port   端口
 * \return     0-成功，其它失败
 */
int http_create_listen_socket(int port)
{
    int addr_family;
    int ip_protocol;
    char addr_str[16];

#ifdef CONFIG_EXAMPLE_IPV4
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;
        struct sockaddr_in listen_addr;
        listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        listen_addr.sin_family = AF_INET;
        listen_addr.sin_port = htons(port);
        inet_ntoa_r(listen_addr.sin_addr, addr_str, sizeof(addr_str) - 1);
#else // IPV6
        addr_family = AF_INET6;
        ip_protocol = IPPROTO_IPV6;
        struct sockaddr_in6 listen_addr;
        bzero(&listen_addr.sin6_addr.un, sizeof(listen_addr.sin6_addr.un));
        listen_addr.sin6_family = AF_INET6;
        listen_addr.sin6_port = htons(port);
        inet6_ntoa_r(listen_addr.sin6_addr, addr_str, sizeof(addr_str) - 1);
#endif

    int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);

    if (listen_sock < 0)
    {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        return -1;
    }

    ESP_LOGI(TAG, "Create listen socket %d", listen_sock);

    int err = bind(listen_sock, (struct sockaddr *)&listen_addr, sizeof(listen_addr));

    if (err != 0)
    {
        close(listen_sock);
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        return -2;
    }

    ESP_LOGI(TAG, "Socket binded");

    err = listen(listen_sock, 1);

    if (err != 0)
    {
        close(listen_sock);
        ESP_LOGE(TAG, "Error occured during listen: errno %d", errno);
        return -3;
    }

    ESP_LOGI(TAG, "Socket listening %s:%d", addr_str, port);
    return listen_sock;
}

/**
 * \brief      处理客户端的请求
 * \param[in]  int client_sock   客户端socket
 * \return     0-成功，其它失败
 */
int http_process_client_request(int client_sock)
{
    bool restart = false;
    char buff[1024];
    int data_len = recv(client_sock, buff, sizeof(buff) - 1, 0);

    if (data_len < 0)
    {
        ESP_LOGE(TAG, "recv failed: errno %d", errno);
        return -1;
    }
    else if (data_len == 0) // Connection closed
    {
        ESP_LOGI(TAG, "Connection closed");
        return -2;
    }
    else
    {
        ESP_LOGI(TAG, "Recv data len:%d", data_len);

        buff[data_len] = 0;

        if (buff[0] != 'G' || buff[1] != 'E' || buff[2] != 'T' || buff[3] != ' ')
        {
            ESP_LOGI(TAG, "Request is not GET");
            return 1;
        }

        ESP_LOGI(TAG, "\n\n%s", buff);

        char *uri = strchr(&buff[4], ' ');

        if (NULL == uri)
        {
            ESP_LOGI(TAG, "Request uri error");
            return 2;
        }

        *uri = '\0';
        uri = &buff[4];
        ESP_LOGI(TAG, "URI:%s", uri);

        char *params = strchr(uri, '?');

        if (NULL != params)
        {
            *params++ = '\0';
            ESP_LOGI(TAG, "Params:%s", params);
        }

        int ret = 404;
        int head_len;
        int content_len;
        char *head;
        char *content;
        char content_len_str[8];

        if (0 == strcmp(uri, "/"))
        {
            ret = 200;
            head = HTTP_HEAD_200;
            content = HTTP_FILE_INDEX;
            head_len = sizeof(HTTP_HEAD_200) -1;
            content_len = sizeof(HTTP_FILE_INDEX) -1;
        }
        else if (0 == strcmp(uri, "/cpu.html"))
        {
            ret = 200;
            head = HTTP_HEAD_200;
            content = HTTP_FILE_CPU;
            head_len = sizeof(HTTP_HEAD_200) -1;
            content_len = sizeof(HTTP_FILE_CPU) -1;
        }
        else if (0 == strcmp(uri, "/light.html"))
        {
            ret = 200;
            head = HTTP_HEAD_200;
            content = HTTP_FILE_LIGHT;
            head_len = sizeof(HTTP_HEAD_200) -1;
            content_len = sizeof(HTTP_FILE_LIGHT) -1;
        }
        else if (0 == strcmp(uri, "/config.html"))
        {
            ret = 200;
            head = HTTP_HEAD_200;
            content = HTTP_FILE_CONFIG;
            head_len = sizeof(HTTP_HEAD_200) - 1;
            content_len = sizeof(HTTP_FILE_CONFIG) -1;
        }
        else if (0 == strcmp(uri, "/cpu"))
        {
            ret = 200;
            head = HTTP_HEAD_200;
            head_len = sizeof(HTTP_HEAD_200) - 1;

            content = buff;
            content_len = get_cpu_gpio(content);
        }
        else if (0 == strcmp(uri, "/index"))
        {
            int v = 220 + rand()%20 - 10;
            int i = rand()%20;
            int w = v * i;

            ret = 200;
            head = HTTP_HEAD_200;
            head_len = sizeof(HTTP_HEAD_200) - 1;
            content = buff;
            content_len = sprintf(content, "{\"t1\":%d,\"t2\":%d,\"t3\":%d,\"t4\":%d,\"t5\":%d,\"t6\":%d,\"t7\":%d,\"t8\":%d}",
                              rand()%50, rand()%100,
                              rand()%10000, rand()%10000, rand()%10000,
                              v, i, w);
        }
        else if (0 == strcmp(uri, "/light"))
        {
            ret = 200;
            head = HTTP_HEAD_200;
            head_len = sizeof(HTTP_HEAD_200) - 1;
            content = buff;
            create_config_light_json(s_config_light, content);
            content_len = strlen(content);
        }
        else if (0 == strcmp(uri, "/reboot"))
        {
            restart = true;

            ret = 200;
            head = HTTP_HEAD_200;
            content = HTTP_FILE_200;
            head_len = sizeof(HTTP_HEAD_200) - 1;
            content_len = sizeof(HTTP_FILE_200) -1;
        }
        else if (0 == strcmp(uri, "/update_http"))
        {
            int port = 0;

            if (1 == sscanf(params,"port=%d", &port)) // 函数将返回成功赋值的字段个数
            {
                s_config_http.port = port;
                put_config_data(&s_config_http, NULL, NULL);

                ESP_LOGI(TAG, "update port:%d", s_config_http.port);

                ret = 200;
                head = HTTP_HEAD_200;
                content = HTTP_FILE_200;
                head_len = sizeof(HTTP_HEAD_200) - 1;
                content_len = sizeof(HTTP_FILE_200) -1;
            }
        }
        else if (0 == strcmp(uri, "/update_wifi"))
        {
            int parm_count = 0;
            char net[32] = "";
            char ssid[32] = "";
            char password[32] = "";

            // 函数将返回成功赋值的字段个数
            parm_count = sscanf(params,"net=%[^&]&ssid=%[^&]&pwd=%[^&]",
                                        net,
                                        ssid,
                                        password);

            ESP_LOGI(TAG, "net:%s ssid:%s password:%s", net, ssid, password);

            if (3 == parm_count && 0 == strcmp(net, "AP"))
            {
                s_config_wifi.type = WIFI_TYPE_AP;
                strcpy(s_config_wifi.ssid, ssid);
                strcpy(s_config_wifi.password, password);
                put_config_data(NULL, &s_config_wifi, NULL);

                ESP_LOGI(TAG, "update wifi:%d ssid:%s password:%s",
                               s_config_wifi.type,
                               s_config_wifi.ssid,
                               s_config_wifi.password);

                ret = 200;
                head = HTTP_HEAD_200;
                content = HTTP_FILE_200;
                head_len = sizeof(HTTP_HEAD_200) - 1;
                content_len = sizeof(HTTP_FILE_200) -1;
            }
            else if (3 == parm_count && 0 == strcmp(net, "STA"))
            {
                s_config_wifi.type = WIFI_TYPE_STA;
                strcpy(s_config_wifi.ssid, ssid);
                strcpy(s_config_wifi.password, password);
                put_config_data(NULL, &s_config_wifi, NULL);

                ESP_LOGI(TAG, "update wifi:%d ssid:%s password:%s",
                               s_config_wifi.type,
                               s_config_wifi.ssid,
                               s_config_wifi.password);

                ret = 200;
                head = HTTP_HEAD_200;
                content = HTTP_FILE_200;
                head_len = sizeof(HTTP_HEAD_200) - 1;
                content_len = sizeof(HTTP_FILE_200) -1;
            }
            else
            {
                ESP_LOGE(TAG, "update wifi net error");
            }
        }
        else if (0 == strcmp(uri, "/update_light"))
        {
            int i;
            int color_id = -1;
            int parm_count = 0;
            bool is_on = false;;
            bool check_on = false;
            bool check_color = false;
            char on[4];
            char id[16];
            char cmd[8];
            char name[32];
            char color[8];

            head = HTTP_HEAD_200;
            content = HTTP_FILE_200;
            head_len = sizeof(HTTP_HEAD_200) - 1;
            content_len = sizeof(HTTP_FILE_200) -1;

            // 函数将返回成功赋值的字段个数
            parm_count = sscanf(params,"cmd=%[^&]&id=%[^&]&name=%[^&]&color=%[^&]&on=%[^&]",
                                        cmd,
                                        id,
                                        name,
                                        color,
                                        on);

            ESP_LOGI(TAG, "cmd:%s id:%s name:%s color:%s on:%s",
                           cmd,
                           id,
                           name,
                           color,
                           on);

            if (5 == parm_count && 0 == strcmp(cmd, "add"))
            {
                for (i = 0; i < LIGHT_MAX_COLOR_COUNT; i++)
                {
                    if (0 == strcmp(color, s_color_map[i]))
                    {
                        color_id = i;
                        check_color = true;
                        break;
                    }
                }

                if (0 == strcmp(on, "on"))
                {
                    is_on = true;
                    check_on = true;
                }
                else if (0 == strcmp(on, "off"))
                {
                    is_on = false;
                    check_on = true;
                }

                if (s_config_light_count < (LIGHT_MAX_COUNT - 1) && check_color && check_on)
                {
                    p_config_light light = &(s_config_light[s_config_light_count]);
                    light->on = is_on;
                    light->color = color_id;
                    strcpy(light->id, id);
                    strcpy(light->name, name);
                    s_config_light_count++;
                    put_config_data(NULL, NULL, s_config_light);

                    ESP_LOGI(TAG, "add ok id:%s name:%s color:%d on:%d",
                                           id,
                                           light->name,
                                           light->color,
                                           light->on);
                    ret = 200;
                }
                else
                {
                    ESP_LOGE(TAG, "update light count color or on error");
                }
            }
            else if (5 == parm_count && 0 == strcmp(cmd, "del"))
            {
                int count = s_config_light_count;
                bool del = false;

                for (i = 0; i < count; i++)
                {
                    if (!del && 0 == strcmp(s_config_light[i].id, id))
                    {
                        ret = 200;
                        del = true;
                        s_config_light_count--;
                        ESP_LOGI(TAG, "del ok id:%s", id);
                        break;
                    }
                    else if (del)
                    {
                        s_config_light[i - 1] = s_config_light[i];
                    }
                }

                if (del)
                {
                    put_config_data(NULL, NULL, s_config_light);
                }
            }
            else if (5 == parm_count && 0 == strcmp(cmd, "mod"))
            {
                for (i = 0; i < LIGHT_MAX_COLOR_COUNT; i++)
                {
                    if (0 == strcmp(color, s_color_map[i]))
                    {
                        color_id = i;
                        check_color = true;
                        continue;
                    }
                }

                if (0 == strcmp(on, "on"))
                {
                    is_on = true;
                    check_on = true;
                }
                else if (0 == strcmp(on, "off"))
                {
                    is_on = false;
                    check_on = true;
                }

                if (check_color && check_on)
                {
                    for (i = 0; i < s_config_light_count; i++)
                    {
                        if (0 == strcmp(s_config_light[i].id, id))
                        {
                            ret = 200;
                            p_config_light light = &(s_config_light[i]);
                            light->on = is_on;
                            light->color = color_id;
                            strcpy(light->name, name);

                            put_config_data(NULL, NULL, s_config_light);

                            ESP_LOGI(TAG, "update ok id:%s name:%s color:%d on:%d",
                                           id,
                                           light->name,
                                           light->color,
                                           light->on);
                            break;
                        }
                    }
                }
                else
                {
                    ESP_LOGE(TAG, "update light color or on error");
                }
            }
        }
        else if (0 == strcmp(uri, "/update_light_on"))
        {
            int i;
            int parm_count;
            bool is_on = false;;
            bool check_on = false;
            char on[4];
            char id[16];

            head = HTTP_HEAD_200;
            content = HTTP_FILE_200;
            head_len = sizeof(HTTP_HEAD_200) - 1;
            content_len = sizeof(HTTP_FILE_200) -1;

            // 函数将返回成功赋值的字段个数
            parm_count = sscanf(params,"id=%[^&]&on=%[^&]", id, on);

            ESP_LOGI(TAG, "id:%s on:%s", id, on);

            if (0 == strcmp(on, "on"))
            {
                is_on = true;
                check_on = true;
            }
            else if (0 == strcmp(on, "off"))
            {
                is_on = false;
                check_on = true;
            }

            if (2 == parm_count && check_on)
            {
                for (i = 0; i < s_config_light_count; i++)
                {
                    if (0 == strcmp(s_config_light[i].id, id))
                    {
                        s_config_light[i].on = is_on;

                        put_config_data(NULL, NULL, s_config_light);

                        ESP_LOGI(TAG, "update ok id:%s on:%d",
                                       id,
                                       s_config_light[i].on);

                        sprintf(buff, "%d", s_config_light[i].color);

                        ret = 200;
                        head = HTTP_HEAD_200;
                        content = buff;
                        head_len = sizeof(HTTP_HEAD_200) - 1;
                        content_len = strlen(content);
                        break;
                    }
                }
            }
            else
            {
                ESP_LOGE(TAG, "update light_on on error");
            }
        }

        if (ret == 404)
        {
            head = HTTP_HEAD_404;
            content = HTTP_FILE_404;
            head_len = sizeof(HTTP_HEAD_404) - 1;
            content_len = sizeof(HTTP_FILE_404) - 1;
        }

        sprintf(content_len_str, "%d\n\n", content_len); // 加2个\n
        ret = send(client_sock, head, head_len, 0);
        ESP_LOGI(TAG, "head send:%d", ret);
        ret = send(client_sock, content_len_str, strlen(content_len_str), 0);
        ESP_LOGI(TAG, "content_len send:%d", ret - 2);
        ret = send(client_sock, content, content_len, 0);
        ESP_LOGI(TAG, "content send:%d", ret);

        ESP_LOGI(TAG, "\n%s%s\n\n%s", head, content_len_str, content);

        if (restart)
        {
            xTaskCreate(restart_task, "restart_task", 1024, NULL, 5, NULL);
        }
    }

    return 0;
}

/**
 * \brief      处理客户端的连接
 * \param[in]  int listen_sock   监听socket
 * \return     0-成功，其它失败
 */
int http_process_client_connect(int listen_sock)
{
#ifdef CONFIG_EXAMPLE_IPV4
    struct sockaddr_in client_addr;
#else
    struct sockaddr_in6 client_addr; // Large enough for both IPv4 or IPv6
#endif
    uint addr_len = sizeof(client_addr);

    int client_sock = accept(listen_sock, (struct sockaddr *)&client_addr, &addr_len);

    if (client_sock < 0)
    {
        ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
        return -1;
    }

    char addr_str[32];

#ifdef CONFIG_EXAMPLE_IPV4
    inet_ntoa_r(client_addr.sin_addr.s_addr, addr_str, sizeof(addr_str) - 1);
#else
    if (client_addr.sin6_family == PF_INET)
    {
        inet_ntoa_r(client_addr.sin_addr.s_addr, addr_str, sizeof(addr_str) - 1);
    }
    else if (client_addr.sin6_family == PF_INET6)
    {
        inet6_ntoa_r(client_addr.sin6_addr, addr_str, sizeof(addr_str) - 1);
    }
#endif

    do
    {
        ESP_LOGI(TAG, "Accepted client socket:%d addr:%s", client_sock, addr_str);
    }
    while (http_process_client_request(client_sock) >= 0);

    if (client_sock != -1)
    {
        ESP_LOGE(TAG, "Shutting down client socket");
        shutdown(client_sock, 0);
        close(client_sock);
    }

    return 0;
}

/**
 * \brief      任务回调函数
 * \param[in]  void* pvParameters  参数
 * \return     无
 */
static void http_server_task(void *pvParameters)
{
    while (1)
    {
        int listen_sock = http_create_listen_socket(80);

        while (listen_sock > 0)
        {
            http_process_client_connect(listen_sock);
        }

        shutdown(listen_sock, 0);
        close(listen_sock);
    }

    vTaskDelete(NULL);
}

void app_main()
{
    ESP_LOGI(TAG, "--main beg------------------------------------------------");
    ESP_LOGI(TAG, "----------------------------------------------------------");

    int i = 0;

    init_gpio();
    init_json(); //--TEST-----------

    s_config_http.port = 80;
    s_config_wifi.type = WIFI_TYPE_STA;
    strcpy(s_config_wifi.ssid, "ChinaNet-EUG9");
    strcpy(s_config_wifi.password, "xt9852@dx.com");
    s_config_light_count = LIGHT_MAX_COUNT;

    for (i = 0; i < LIGHT_MAX_COUNT; i++)
    {
        s_config_light[i].on = i % 2;
        s_config_light[i].color = i % LIGHT_MAX_COLOR_COUNT;
        sprintf(s_config_light[i].id, "s%02d", i);
        sprintf(s_config_light[i].name, "灯%02d", i);
    }

    // cJSON_Print总是失败,cJSON_Parse成功
    put_config_data(&s_config_http, &s_config_wifi, s_config_light);

    get_config_data(&s_config_http, &s_config_wifi, s_config_light);

    init_wifi(&s_config_wifi);

    xTaskCreate(http_server_task, "http_server", 4096, NULL, 5, NULL);

    ESP_LOGI(TAG, "--main end----------------------------------------------------");
}
