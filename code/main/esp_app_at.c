#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_app_at.h"

#define BUF_SIZE        (1024)

static bool g_init = false;

enum
{
    AT_AT0,
    AT_MQTTSTATU,
    AT_MCONFIG,
    AT_MIPSTART,
    AT_MCONNECT,
    AT_MSUB
};

/**
 *\brief        串口1发送AT命令
 *\param[in]    cmd             AT命令字符串
 *\param[in]    len             AT命令字符串长
 *\return                       无
 */
void at_uart1_send_cmd(const char *cmd, int len)
{
    ESP_LOGI(TAG, cmd);
    uart_write_bytes(UART_NUM_1, cmd, len);
}

/**
 *\brief        发送AT命令任务
 *\param[in]    param           mqtt信息,订阅的主题数组下标,串口号
 *\return                       无
 */
static void at_uart1_send_task(void *param)
{
    vTaskDelay(pdMS_TO_TICKS(5000));

    at_uart1_send_cmd("ATE0\r", 5); ///< 关闭指令回显

    vTaskDelete(NULL);
}

/**
 *\brief        串口事件任务
 *\param[in]    param           串口事件队列,消息输出队列,串口号
 *\return                       无
 */
static void at_uart0_event_task(void *param)
{
    uint* arg = (uint*)param;
    QueueHandle_t uart_event_queue = (QueueHandle_t)arg[0];
    p_config_mqtt mqtt = (p_config_mqtt)arg[1];
    free(param);

    ESP_LOGI(TAG, "at_uart_event_task event queue: %x", (unsigned int)uart_event_queue);
    ESP_LOGI(TAG, "at_uart_event_task mqtt: %x", (unsigned int)mqtt);

    uart_event_t event;
    int at = AT_AT0;
    int id = 1;
    int len;
    char *cmd = (char*)malloc(BUF_SIZE);
    char *tmp = (char*)malloc(BUF_SIZE);

    for (;;)
    {
        if (xQueueReceive(uart_event_queue, (void *)&event, (portTickType)portMAX_DELAY))
        {
            switch (event.type)
            {
                case UART_DATA:
                {
                    tmp[event.size] = '\0';
                    uart_read_bytes(UART_NUM_0, (uint8_t*)tmp, event.size, portMAX_DELAY);
                    ESP_LOGI(TAG, "at:%d len:%d data:%s", at, event.size, tmp);

                    if (AT_AT0 == at && strstr(tmp, "OK"))                                                  ///< 关闭指令回显
                    {
                        at = AT_MQTTSTATU;
                        at_uart1_send_cmd("AT+MQTTSTATU\r", 13);                                            ///< 查询MQTT连接状态
                    }
                    else if (AT_MQTTSTATU == at && strstr(tmp, "+MQTTSTATU: 1"))                            ///< MQTT已建立连接
                    {
                        at = AT_MSUB;
                    }
                    else if (AT_MQTTSTATU == at && strstr(tmp, "+MQTTSTATU: 0"))                            ///< MQTT未建立连接
                    {
                        at = AT_MCONFIG;
                        len = snprintf(cmd, BUF_SIZE, "AT+MCONFIG=\"%s/4g%s%s%s%s\"\r",                     ///< 配置MQTT帐户信息
                                       mqtt->clientid,
                                       ('\0' == mqtt->username[0] ? "" : ","), mqtt->username,
                                       ('\0' == mqtt->password[0] ? "" : ","), mqtt->password);
                        at_uart1_send_cmd(cmd, len);
                    }
                    else if (AT_MCONFIG == at && strstr(tmp, "OK"))                                         ///< 配置MQTT帐户信息成功
                    {
                        at = AT_MIPSTART;
                        len = snprintf(cmd, BUF_SIZE, "AT+MIPSTART=\"%s\",%d,4\r", mqtt->addr, mqtt->port); ///< 配置MQTT服务器信息
                        at_uart1_send_cmd(cmd, len);
                    }
                    else if (AT_MIPSTART == at && strstr(tmp, "+MIPSTART: SUCCESS"))                        ///< 配置MQTT服务器信息成功
                    {
                        at = AT_MCONNECT;
                        at_uart1_send_cmd("AT+MCONNECT=0,60\r", 17);                                        ///< 连接MQTT服务器
                    }
                    else if (AT_MCONNECT == at && strstr(tmp, "+MCONNECT: SUCCESS"))                        ///< 连接MQTT服务器成功
                    {
                        at = AT_MSUB;
                        len = snprintf(cmd, BUF_SIZE, "AT+MSUB=\"%s\",0\r\n", mqtt->topic[0]);              ///< 订阅主题
                        at_uart1_send_cmd(cmd, len);
                    }
                    else if (AT_MSUB == at && strstr(tmp, "+MSUB: SUCCESS"))                                ///< 订阅主题成功
                    {
                        if (id >= mqtt->topic_count) break;
                        len = snprintf(cmd, BUF_SIZE, "AT+MSUB=\"%s\",0\r\n", mqtt->topic[id++]);
                        at_uart1_send_cmd(cmd, len);
                    }
                    else if (AT_MSUB == at && strncmp(tmp, "+MSUB: \"", 8) == 0)                            ///< MQTT订阅的消息
                    {
                        BaseType_t ret = pdFALSE;

                        for (int i = 0; i < mqtt->topic_count; i++)
                        {
                            if (strncmp(tmp + 8, mqtt->topic[i], strlen(mqtt->topic[i])) == 0)
                            {
                                t_msg_head msg;
                                msg.len = event.size;
                                msg.data = (char*)malloc(event.size + 1);
                                msg.data[msg.len] = '\0';
                                memcpy(msg.data, tmp, event.size);
                                ret = xQueueSend(mqtt->queue[i], &msg, portMAX_DELAY);
                                break;
                            }
                        }

                        ESP_LOGI(TAG, "Send queue %s", (pdTRUE == ret) ? "ok" : "fail");
                    }
                    break;
                }
                case UART_FIFO_OVF:
                {
                    ESP_LOGI(TAG, "hw fifo overflow");
                    uart_flush_input(UART_NUM_0);
                    xQueueReset(uart_event_queue);
                    break;
                }
                case UART_BUFFER_FULL:
                {
                    ESP_LOGI(TAG, "ring buffer full");
                    uart_flush_input(UART_NUM_0);
                    xQueueReset(uart_event_queue);
                    break;
                }
                case UART_PARITY_ERR:
                {
                    ESP_LOGI(TAG, "uart parity error");
                    break;
                }
                case UART_FRAME_ERR:
                {
                    ESP_LOGI(TAG, "uart frame error");
                    break;
                }
                default:
                {
                    ESP_LOGI(TAG, "uart event type: %d", event.type);
                    break;
                }
            }
        }
    }

    free(cmd);
    free(tmp);
    vTaskDelete(NULL);
}

/**
 *\brief        初始化串口
 *\param[in]    uart    串口
 *\return       0       成功
 */
int at_uart_init(uart_port_t uart)
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    if (uart_param_config(uart, &uart_config) != ESP_OK)
    {
        ESP_LOGE(TAG, "uart_param_config fail");
        return -1;
    }

    return 0;
}

/**
 *\brief        初始化串口,串口0,接收数据,串口1,发送数据
 *\param[in]    mqtt    配置信息
 *\return       0       成功
 */
int at_uart_driver(p_config_mqtt mqtt)
{
    if (g_init) return 0;

    if (at_uart_init(UART_NUM_0) != 0)
    {
        ESP_LOGE(TAG, "at_uart_init fail");
        return -1;
    }

    QueueHandle_t uart_event_queue;

    if (uart_driver_install(UART_NUM_0, BUF_SIZE * 2, 0, 100, &uart_event_queue, 0) != ESP_OK)
    {
        ESP_LOGE(TAG, "uart_driver_install fail");
        return -2;
    }

    uint *param = (uint*)malloc(sizeof(uint) * 2);
    param[0] = (uint)uart_event_queue;
    param[1] = (uint)mqtt;

    if (xTaskCreate(at_uart0_event_task, "at_uart0_event_task", 2048, param, 12, NULL) != pdPASS)
    {
        ESP_LOGE(TAG, "xTaskCreate fail");
        return -3;
    }

    if (at_uart_init(UART_NUM_1) != 0)
    {
        ESP_LOGE(TAG, "at_uart_init fail");
        return -4;
    }

    if (uart_driver_install(UART_NUM_1, BUF_SIZE * 2, 0, 0, NULL, 0) != ESP_OK)
    {
        ESP_LOGE(TAG, "uart_driver_install fail");
        return -5;
    }

    g_init = true;
    return 0;
}

/**
 *\brief        发送AT命令连接mqtt
 *\param[in]    mqtt    配置信息
 *\return       0       成功
 */
int at_mqtt_init(p_config_mqtt mqtt)
{
    if (NULL == mqtt)
    {
        ESP_LOGE(TAG, "mqtt is null");
        return -1;
    }

    if (at_uart_driver(mqtt) != 0)
    {
        ESP_LOGE(TAG, "at_uart_init fail");
        return -2;
    }

    if (xTaskCreate(at_uart1_send_task, "uart1_send_task", 2048, NULL, 12, NULL) != pdPASS)
    {
        ESP_LOGE(TAG, "xTaskCreate fail");
        return -3;
    }

    return 0;
}
