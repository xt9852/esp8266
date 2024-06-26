#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_app_http.h"
#include "esp_app_http_cpu_page.h"
#include "esp_app_http_cpu_data.h"
#include "esp_app_http_cfg_page.h"
#include "esp_app_http_cfg_data.h"

#define NETWORK_IPV4
#define HTTP_HEAD_200   "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: "
#define HTTP_HEAD_400   "HTTP/1.1 400 Bad Request\nContent-Type: text/html\nContent-Length: "
#define HTTP_HEAD_404   "HTTP/1.1 404 Not Found\nContent-Type: text/html\nContent-Length: "
#define HTTP_FILE_404   "404"

static char          *g_buff;
static uint           g_size;
static p_config       g_config;
static p_config_wifi  g_wifi;
static p_config_mqtt  g_mqtt;
static p_config_http  g_http;
static p_config_light g_light;


/**
 * \brief      创建监听socket
 * \param[in]  uint port   端口
 * \return     0-成功，其它失败
 */
int http_create_listen_socket(uint port)
{
    int addr_family;
    int ip_protocol;
    char addr_str[16];

#ifdef NETWORK_IPV4
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
        ESP_LOGE(TAG, "Socket errno:%d", errno);
        return -1;
    }

    ESP_LOGI(TAG, "Socket %d", listen_sock);

    int err = bind(listen_sock, (struct sockaddr *)&listen_addr, sizeof(listen_addr));

    if (err != 0)
    {
        close(listen_sock);
        ESP_LOGE(TAG, "Socket bind errno:%d", errno);
        return -2;
    }

    ESP_LOGI(TAG, "Socket binded");

    err = listen(listen_sock, 1);

    if (err != 0)
    {
        close(listen_sock);
        ESP_LOGE(TAG, "Socket listen errno:%d", errno);
        return -3;
    }

    ESP_LOGI(TAG, "Socket listened %s:%d", addr_str, port);
    return listen_sock;
}

/**
 * \brief      得到URI中的参数,/cpu-data?clk=1 HTTP/1.1
 * \param[in]  char *uri    URI地址
  * \return    char* URI中参数指针
 */
char* http_get_arg(char *uri)
{
    char *ch = strchr(uri, ' ');

    if (NULL == ch)
    {
        ESP_LOGI(TAG, "request uri:%s error", uri);
        return NULL;
    }

    *ch = '\0';

    ch = strchr(uri, '?');

    if (NULL != ch)
    {
        *ch++ = '\0';
        return ch;
    }

    return NULL;
}

/**
 * \brief      处理客户端的请求
 * \param[in]  int client_sock   客户端socket
 * \return     0-成功，其它失败
 */
int http_process_client_request(int client_sock)
{
    int data_len = recv(client_sock, g_buff, g_size, 0);

    if (data_len < 0)
    {
        ESP_LOGE(TAG, "Recv errno:%d", errno);
        return -1;
    }
    else if (data_len == 0)
    {
        ESP_LOGI(TAG, "Connection closed");
        return -2;
    }
    else
    {
        ESP_LOGI(TAG, "Socket %d recv len:%d", client_sock, data_len);

        g_buff[data_len] = 0;

        if (0 != strncmp(g_buff, "GET ", 4))
        {
            ESP_LOGI(TAG, "Request is not GET");
            return 1;
        }

        int ret = 404;
        char *uri = &g_buff[4];
        char *arg = http_get_arg(uri);
        char *content = g_buff;
        uint content_len = g_size;

        if (0 == strcmp(uri, "/"))
        {
            ret = http_cpu_page(content, &content_len);
        }
        else if (0 == strcmp(uri, "/cfg.html"))
        {
            ret = http_cfg_page(g_config, content, &content_len);
        }
        else if (0 == strcmp(uri, "/cfg-wifi"))
        {
            ret = http_cfg_wifi(arg, g_wifi, content, &content_len);
        }
        else if (0 == strcmp(uri, "/cfg-mqtt"))
        {
            ret = http_cfg_mqtt(arg, g_mqtt, content, &content_len);
        }
        else if (0 == strcmp(uri, "/cfg-http"))
        {
            ret = http_cfg_http(arg, g_http, content, &content_len);
        }
        else if (0 == strcmp(uri, "/cfg-light"))
        {
            ret = http_cfg_light(arg, g_light, content, &content_len);
        }
        else if (0 == strcmp(uri, "/reboot"))
        {
            ret = http_reboot(content, &content_len);
        }
        else if (0 == strcmp(uri, "/cpu-data"))
        {
            ret = http_cpu_data(arg, content, &content_len);
        }

        char *head;
        uint head_len;

        if (ret == 200)
        {
            head = HTTP_HEAD_200;
            head_len = sizeof(HTTP_HEAD_200) - 1;
        }
        else if (ret == 400)
        {
            head = HTTP_HEAD_400;
            head_len = sizeof(HTTP_HEAD_400) - 1;
        }
        else
        {
            head = HTTP_HEAD_404;
            content = HTTP_FILE_404;
            head_len = sizeof(HTTP_HEAD_404) - 1;
            content_len = sizeof(HTTP_FILE_404) - 1;
        }

        char content_len_str[8];
        sprintf(content_len_str, "%d\n\n", content_len);                        // 内容长度,加2个\n

        ret = send(client_sock, head, head_len, 0);                             // 发送头部
        ESP_LOGI(TAG, "Send head len:%d", ret);

        ret = send(client_sock, content_len_str, strlen(content_len_str), 0);   // 发送内容长度
        ESP_LOGI(TAG, "Send Content-Length len:%d", ret - 2);

        ret = send(client_sock, content, content_len, 0);                       // 发送内容
        ESP_LOGI(TAG, "Send body len:%d", ret);
    }

    return 0;
}

/**
 * \brief      任务回调函数
 * \param[in]  void* param  参数
 * \return     无
 */
static void http_client_task(void *param)
{
    int client_sock = (int)param;

    while (http_process_client_request(client_sock) >= 0);

    ESP_LOGI(TAG, "Shutting down client socket %d", client_sock);
    shutdown(client_sock, 0);
    close(client_sock);

    vTaskDelete(NULL);
}

/**
 * \brief      处理客户端的连接
 * \param[in]  int listen_sock   监听socket
 * \return     0-成功，其它失败
 */
int http_process_client_connect(int listen_sock)
{
#ifdef NETWORK_IPV4
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

    char addr_str[64];

#ifdef NETWORK_IPV4
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

    ESP_LOGI(TAG, "Accepted client socket:%d addr:%s ----------", client_sock, addr_str);
    xTaskCreate(http_client_task, "http_client", 4096, (void*)client_sock, 5, NULL);
    return 0;
}

/**
 * \brief      任务回调函数
 * \param[in]  void* param  参数
 * \return     无
 */
static void http_server_task(void *param)
{
    p_config_http http = (p_config_http)param;

    while (1)
    {
        int listen_sock = http_create_listen_socket(http->port);

        while (listen_sock > 0)
        {
            http_process_client_connect(listen_sock);
        }

        shutdown(listen_sock, 0);
        close(listen_sock);
    }

    vTaskDelete(NULL);
}

/**
 * \brief      初始化http
 * \param[in]  p_config       config
 * \param[in]  char          *buff      缓存
 * \param[in]  uint           size      缓存大小
 * \return     0-成功，其它失败
 */
int http_init(p_config config, char *buff, uint size)
{
    g_buff   = buff;
    g_size   = size;
    g_config = config;
    g_wifi   = &(config->wifi);
    g_mqtt   = &(config->mqtt);
    g_http   = &(config->http);
    g_light  = &(config->light);

    xTaskCreate(http_server_task, "http_server", 4096, g_http, 5, NULL);
    return 0;
}