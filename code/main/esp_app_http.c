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

extern const unsigned int   g_buf_size;     ///< 缓存大小

extern char                 g_buf[];        ///< 缓存,esp8266的栈比较小,所以使用堆区内存

static p_path_proc          g_data;         ///< 路径与函数对应数据

static unsigned int         g_data_count;   ///< 数量

/**
 *\brief        创建监听socket
 *\param[in]    port        端口号
 *\return       0           成功
 */
int http_create_listen_socket(unsigned short port)
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
 *\brief        处理客户端的请求
 *\param[in]    client_sock     客户端socket
 *\return       0               成功
 */
int http_process_client_request(int client_sock)
{
    int data_len = recv(client_sock, g_buf, g_buf_size, 0);

    if (data_len < 0)
    {
        ESP_LOGE(TAG, "Recv errno:%d", errno);
        return -1;
    }

    if (data_len == 0)
    {
        ESP_LOGI(TAG, "Connection closed");
        return -2;
    }

    g_buf[data_len] = 0;

    ESP_LOGI(TAG, "Socket %d recv len:%d", client_sock, data_len);

    if (0 != strncmp(g_buf, "GET ", 4))
    {
        ESP_LOGI(TAG, "Request is not GET");
        return -3;
    }

    int          i;
    int          ret         = 404;
    char        *uri         = &g_buf[4];
    char        *end         = strstr(uri, " ");
    char        *mark        = strstr(uri, "?");
    char        *content     = g_buf;
    unsigned int content_len = g_buf_size;

    if (NULL != end)
    {
        *end = '\0';
    }

    if (NULL != mark)
    {
        *mark++ = '\0';
        ESP_LOGI(TAG, mark);
    }

    ESP_LOGI(TAG, uri);

    for (i = 0; i < g_data_count && 0 != strcmp(uri, g_data[i].path); i++);

    ret = (i >= g_data_count) ? 404 : g_data[i].proc(mark, g_data[i].param, content, &content_len);

    char *head;
    uint head_len;

    switch (ret)
    {
        case 200:
        {
            head = HTTP_HEAD_200;
            head_len = sizeof(HTTP_HEAD_200) - 1;
            break;
        }
        case 400:
        {
            head = HTTP_HEAD_400;
            head_len = sizeof(HTTP_HEAD_400) - 1;
            break;
        }
        default:
        {
            head = HTTP_HEAD_404;
            content = HTTP_FILE_404;
            head_len = sizeof(HTTP_HEAD_404) - 1;
            content_len = sizeof(HTTP_FILE_404) - 1;
            break;
        }
    }

    char content_len_str[8];
    sprintf(content_len_str, "%d\n\n", content_len);                        // 内容长度,加2个\n

    ret = send(client_sock, head, head_len, 0);                             // 发送头部
    ESP_LOGI(TAG, "Send head len:%d", ret);

    ret = send(client_sock, content_len_str, strlen(content_len_str), 0);   // 发送内容长度
    ESP_LOGI(TAG, "Send Content-Length len:%d", ret - 2);

    ret = send(client_sock, content, content_len, 0);                       // 发送内容
    ESP_LOGI(TAG, "Send body len:%d", ret);
    return 0;
}

/**
 *\brief        客户端任务回调函数
 *\param[in]    param           参数
 *\return                       无
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
 *\brief        处理客户端的连接
 *\param[in]    listen_sock     监听socket
 *\return       0               成功
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
 *\brief        HTTP主任务回调函数
 *\param[in]    param           参数
 *\return                       无
 */
static void http_server_task(void *param)
{
    unsigned short port = (unsigned int)param;

    while (1)
    {
        int listen_sock = http_create_listen_socket(port);

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
 *\brief        初始化http
 *\param[in]    port            商品呺
 *\param[in]    data            路径与函数对应数据
 *\param[in]    data_count      路径与函数对应数据数量
 *\return       0               成功
 */
int http_init(unsigned short port, p_path_proc data, unsigned int data_count)
{
    g_data = data;
    g_data_count = data_count;
    xTaskCreate(http_server_task, "http_server", 4096, (void*)(unsigned int)port, 5, NULL);
    return 0;
}