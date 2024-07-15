#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include "cJSON.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "esp_ota_ops.h"
#include "esp_spi_flash.h"
#include "esp_app_ota.h"

extern const unsigned int   g_buf_size;     ///< 缓存大小

extern char                 g_buf[];        ///< 缓存,esp8266的栈比较小,所以使用堆区内存

typedef enum esp_ota_firm_state
{
    ESP_OTA_INIT,
    ESP_OTA_RECVED,
    ESP_OTA_FINISH,

} esp_ota_firm_state_t;

int ota_connect_server(const char *server, uint port)
{
    int socket_id = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_id == -1)
    {
        ESP_LOGE(TAG, "Create socket failed!");
        return -1;
    }

    struct sockaddr_in sock_info;
    memset(&sock_info, 0, sizeof(struct sockaddr_in));
    sock_info.sin_family = AF_INET;
    sock_info.sin_addr.s_addr = inet_addr(server);
    sock_info.sin_port = htons(port);

    int ret = connect(socket_id, (struct sockaddr *)&sock_info, sizeof(sock_info));

    if (ret == -1)
    {
        ESP_LOGI(TAG, " OTA conn %s:%d errno:%d", server, port, errno);
        close(socket_id);
        return -2;
    }

    ESP_LOGI(TAG, " OTA conn %s:%d ok", server, port);
    return socket_id;
}

int ota_send_request(const char *server, uint port, const char *uri)
{
    int socket_id = ota_connect_server(server, port);

    if (socket_id < 0)
    {
        return -1;
    }

    int len = snprintf(g_buf, g_buf_size, "GET %s HTTP/1.0\r\nHost: %s:%d\r\nUser-Agent: esp-idf/1.0 esp32\r\n\r\n", uri, server, port);

    if (len < 0) {
        ESP_LOGE(TAG, "Failed to allocate memory for GET request buffer");
        close(socket_id);
        return -2;
    }

    len = send(socket_id, g_buf, len, 0);

    if (len < 0)
    {
        ESP_LOGE(TAG, " OTA Send %s errno:%d", uri, errno);
        close(socket_id);
        return -3;
    }

    ESP_LOGI(TAG, " OTA send %s ok", uri);
    return socket_id;
}

const esp_partition_t* ota_begin(esp_ota_handle_t *handle)
{
    const esp_partition_t *partition_boot   = esp_ota_get_boot_partition();            // 配置的启动应用程序分区
    const esp_partition_t *partition_run    = esp_ota_get_running_partition();         // 正在运行的应用程序分区
    const esp_partition_t *partition_update = esp_ota_get_next_update_partition(NULL); // 应该写入的应用程序分区

    if (partition_boot != partition_run) {
        ESP_LOGW(TAG, "Configured OTA boot partition at offset 0x%08x, but running from offset 0x%08x", partition_boot->address, partition_run->address);
        ESP_LOGW(TAG, "(This can happen if either the OTA boot data or preferred boot image become corrupted somehow.)");
    }

    ESP_LOGI(TAG, " Running partition type %d subtype %d offset 0x%08x", partition_run->type, partition_run->subtype, partition_run->address);

    if (NULL == partition_update)
    {
        ESP_LOGE(TAG, "Update partition NULL");
        return NULL;
    }

    ESP_LOGI(TAG, "Writing partition type %d subtype %d offset 0x%08x", partition_update->type, partition_update->subtype, partition_update->address);

    esp_err_t err = esp_ota_begin(partition_update, OTA_SIZE_UNKNOWN, handle);

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "esp_ota_begin failed, error=%d", err);
        return NULL;
    }

    ESP_LOGI(TAG, "OTA init ok handle:%d", *handle);
    return partition_update;
}

void ota_end(esp_ota_handle_t handle, const esp_partition_t *partition_update)
{
    if (esp_ota_end(handle) != ESP_OK)
    {
        ESP_LOGE(TAG, "esp_ota_end failed!");
        return;
    }

    esp_err_t err = esp_ota_set_boot_partition(partition_update);

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "esp_ota_set_boot_partition failed! err=0x%x", err);
        return;
    }

    ESP_LOGI(TAG, "OTA success!!!!!! Prepare to restart system!");
    esp_restart();
}

int ota_process(const char *server, uint port, const char *uri)
{
    esp_ota_handle_t handle = 0;

    const esp_partition_t *partition_update = ota_begin(&handle);

    if (NULL == partition_update || 0 == handle)
    {
        ESP_LOGE(TAG, "esp_test_ota_begin failed!");
        return -1;
    }

    int socket_id = ota_send_request(server, port, uri);

    if (socket_id <= 0)
    {
        ESP_LOGE(TAG, "esp_test_ota_send_request failed!");
        return -2;
    }

    int tmp = 0;
    int len = 0;
    bool run = true;
    size_t remain = 0;
    esp_err_t err;
    esp_ota_firm_state_t state = ESP_OTA_INIT;

    while (run)
    {
        tmp = recv(socket_id, &g_buf[len], g_buf_size - len, 0);

        if (tmp < 0)
        {
            ESP_LOGE(TAG, "Error: receive data error! errno=%d", errno);
            close(socket_id);
            run = false;
            return -3;
        }
        else if (tmp == 0)
        {
            ESP_LOGI(TAG, "Connection closed, all packets received");
            close(socket_id);
            run = false;
        }
        else
        {
            len += tmp;

            switch (state)
            {
                case ESP_OTA_INIT:
                {
                    g_buf[len] = '\0';
                    ESP_LOGI(TAG, g_buf);

                    char *ptr = strstr(g_buf, "Content-Length: ");

                    if (NULL == ptr)
                    {
                        ESP_LOGI(TAG, "find Content-Length: fail");
                        continue;
                    }

                    remain = atoi(ptr + 16);

                    ESP_LOGI(TAG, "Content-Length: %d", remain);

                    ptr = strstr(g_buf, "\r\n\r\n"); // find data

                    if (NULL == ptr)
                    {
                        ESP_LOGI(TAG, "find body fail");
                        continue;
                    }

                    ptr += 4;
                    len -= (ptr - g_buf); // remain data len
                    memcpy(g_buf, ptr, len);

                    state = ESP_OTA_RECVED;

                    ESP_LOGI(TAG, "ESP_OTA_INIT rmain data len %d", len);
                    break;
                }
                case ESP_OTA_RECVED:
                {
                    err = esp_ota_write(handle, g_buf, len);

                    if (err != ESP_OK)
                    {
                        ESP_LOGE(TAG, "ESP_OTA_RECVED esp_ota_write failed! err=0x%x", err);
                        close(socket_id);
                        run = false;
                        return -4;
                    }

                    remain -= len;

                    if (remain > 0)
                    {
                        ESP_LOGI(TAG, "ESP_OTA_RECVED Write %d Remain %d", len, remain);
                    }
                    else
                    {
                        state = ESP_OTA_FINISH;
                        ESP_LOGI(TAG, "ESP_OTA_FINISH");
                    }

                    len = 0;
                    break;
                }
                case ESP_OTA_FINISH:
                {
                    ESP_LOGI(TAG, "ESP_OTA_FINISH");
                    close(socket_id);
                    run = false;
                    break;
                }
            }
        }
    }

    ota_end(handle, partition_update);
    return 0;
}

/**
 *\brief        ota任务
 *\param[in]    param       队列
 *\return                   无
 */
static void ota_task(void *param)
{
    t_msg_head msg;
    xQueueHandle queue = (xQueueHandle)param;

    while (xQueueReceive(queue, &msg, portMAX_DELAY) == pdTRUE)
    {
        ESP_LOGI(TAG, "Rece len:%d data:%s", msg.len, msg.data);

        cJSON *root = cJSON_Parse(msg.data);

        if (NULL == root)
        {
            ESP_LOGE(TAG, "Failed parse json");
            free(msg.data);
            continue;
        }

        cJSON *type = cJSON_GetObjectItem(root, "type");

        if (NULL == type || NULL == type->valuestring || 0 != strcmp(type->valuestring, "ota"))
        {
            ESP_LOGE(TAG, "Failed json type error");
            cJSON_Delete(root);
            free(msg.data);
            continue;
        }

        cJSON *server = cJSON_GetObjectItem(root, "server");

        if (NULL == server || NULL == server->valuestring)
        {
            ESP_LOGE(TAG, "Failed json server error");
            cJSON_Delete(root);
            free(msg.data);
            continue;
        }

        cJSON *port = cJSON_GetObjectItem(root, "port");

        if (NULL == port)
        {
            ESP_LOGE(TAG, "Failed json port error");
            cJSON_Delete(root);
            free(msg.data);
            continue;
        }

        cJSON *uri = cJSON_GetObjectItem(root, "uri");

        if (NULL == uri || NULL == uri->valuestring)
        {
            ESP_LOGE(TAG, "Failed json uri error");
            cJSON_Delete(root);
            free(msg.data);
            continue;
        }

        ota_process(server->valuestring, port->valueint, uri->valuestring);

        cJSON_Delete(root);
        free(msg.data);
    }

    vTaskDelete(NULL);
}

/**
 *\brief        初始化ota
 *\param[in]    queue       队列
 *\return       0           成功
 */
int ota_init(void *queue)
{
    xTaskCreate(&ota_task, "ota_task", 8192, queue, 5, NULL);
    return 0;
}