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
#include "esp_system.h"
#include "esp_log.h"
#include "esp_test_http.h"
#include "esp_test_http_index_page.h"
#include "esp_test_http_index_data.h"
#include "esp_test_http_cfg_page.h"
#include "esp_test_http_cfg_data.h"

#define NETWORK_IPV4
#define HTTP_HEAD_200   "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: "
#define HTTP_HEAD_400   "HTTP/1.1 400 Bad Request\nContent-Type: text/html\nContent-Length: "
#define HTTP_HEAD_404   "HTTP/1.1 404 Not Found\nContent-Type: text/html\nContent-Length: "
#define HTTP_FILE_404   "404"

static p_config_http  s_http;
static p_config_wifi  s_wifi;
static p_config_light s_light;
static uint          *s_light_count;
static uint           s_light_max;
static uint           s_buff_size;
static char          *s_buff;


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
    ESP_LOGI(TAG, "%s beg", __FUNCTION__);

    int data_len = recv(client_sock, s_buff, s_buff_size, 0);

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

        s_buff[data_len] = 0;

        if (s_buff[0] != 'G' || s_buff[1] != 'E' || s_buff[2] != 'T' || s_buff[3] != ' ')
        {
            ESP_LOGI(TAG, "Request is not GET");
            return 1;
        }

        ESP_LOGI(TAG, "\n%s", s_buff);

        char *uri = strchr(&s_buff[4], ' ');

        if (NULL == uri)
        {
            ESP_LOGI(TAG, "Request uri error");
            return 2;
        }

        *uri = '\0';
        uri = &s_buff[4];
        ESP_LOGI(TAG, "URI:%s", uri);

        char *param = strchr(uri, '?');

        if (NULL != param)
        {
            *param++ = '\0';
            ESP_LOGI(TAG, "arg:%s", param);
        }

        int ret = 404;
        char *content = s_buff;
        uint content_len = s_buff_size;

        if (0 == strcmp(uri, "/"))
        {
            ret = http_index_page(content, &content_len);
        }
        else if (0 == strcmp(uri, "/index.json"))
        {
            ret = http_index_data(content, &content_len);
        }
        else if (0 == strcmp(uri, "/cfg.html"))
        {
            ret = http_cfg_page(content, &content_len);
        }
        else if (0 == strcmp(uri, "/cfg-http"))
        {
            ret = http_cfg_http(param, s_http, content, &content_len);
        }
        else if (0 == strcmp(uri, "/cfg-wifi"))
        {
            ret = http_cfg_wifi(param, s_wifi, content, &content_len);
        }
        else if (0 == strcmp(uri, "/cfg-light"))
        {
            ret = http_cfg_light(param, s_light, s_light_count, s_light_max, content, &content_len);
        }
        else if (0 == strcmp(uri, "/reboot"))
        {
            ret = http_reboot(content, &content_len);
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

        ESP_LOGI(TAG, "head len:%d content len:%d", head_len, content_len);

        char content_len_str[8];
        sprintf(content_len_str, "%d\n\n", content_len);    // 内容长度,加2个\n

        ret = send(client_sock, head, head_len, 0);         // 发送头部
        ESP_LOGI(TAG, "send head len:%d", ret);

        ret = send(client_sock, content_len_str, strlen(content_len_str), 0);   // 发送内容长度
        ESP_LOGI(TAG, "send content_len len:%d", ret - 2);

        ret = send(client_sock, content, content_len, 0);   // 发送内容
        ESP_LOGI(TAG, "send content len:%d", ret);

        ESP_LOGI(TAG, "\n%s%s", head, content_len_str);
    }

    ESP_LOGI(TAG, "%s end", __FUNCTION__);
    return 0;
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
    p_config_http http = (p_config_http)pvParameters;

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
 * \param[in]  p_config_http  http
 * \param[in]  p_config_wifi  wifi
 * \param[in]  p_config_light light
 * \param[in]  uint          *light_count
 * \param[in]  uint           light_max
 * \param[in]  char          *buff          缓存
 * \param[in]  uint           buff_size     缓存大小
 * \return     0-成功，其它失败
 */
int http_init(p_config_http http, p_config_wifi wifi, 
              p_config_light light, uint *light_count, uint light_max,
              char *buff, uint buff_size)
{
    s_http        = http;
    s_wifi        = wifi;
    s_light       = light;
    s_light_max   = light_max;
    s_light_count = light_count;
    s_buff        = buff;
    s_buff_size   = buff_size;

    xTaskCreate(http_server_task, "http_server", 4096, http, 5, NULL);
    return 0;
}