#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "esp_app_cfg.h"

#define EX_UART_NUM     UART_NUM_1
#define BUF_SIZE        (1024)
#define RD_BUF_SIZE     (BUF_SIZE)

static QueueHandle_t    g_uart_queue;

/**
 *\brief        串口事件任务回调函数
 *\param[in]    param           串口ID
 *\return                       无
 */
static void uart_event_task(void *param)
{
    unsigned int id = (unsigned int)param;
    char buf[16];
    int len;
    
    ESP_LOGI(TAG, "uart[%d] send", id);
    
    for (int i = 0; 1; i++)
    {
        len = sprintf(buf, "%d\r", i);
        uart_write_bytes(EX_UART_NUM, buf, len);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    
    /*
    uart_event_t event;
    uint8_t *tmp = (uint8_t*)malloc(RD_BUF_SIZE);
    
    for (;;)
    {
        if (xQueueReceive(g_uart_queue, (void *)&event, (portTickType)portMAX_DELAY))
        {
            bzero(tmp, RD_BUF_SIZE);
            ESP_LOGI(TAG, "uart[%d] event:", EX_UART_NUM);

            switch (event.type)
            {
                case UART_DATA:
                {
                    ESP_LOGI(TAG, "[UART DATA]: %d", event.size);
                    uart_read_bytes(EX_UART_NUM, tmp, event.size, portMAX_DELAY);
                    ESP_LOGI(TAG, "[DATA EVT]:");
                    uart_write_bytes(EX_UART_NUM, (const char *)tmp, event.size);
                    break;
                }
                case UART_FIFO_OVF:
                {
                    ESP_LOGI(TAG, "hw fifo overflow");
                    uart_flush_input(EX_UART_NUM);
                    xQueueReset(g_uart_queue);
                    break;
                }
                case UART_BUFFER_FULL:
                {
                    ESP_LOGI(TAG, "ring buffer full");
                    uart_flush_input(EX_UART_NUM);
                    xQueueReset(g_uart_queue);
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

    free(tmp);
    */
    vTaskDelete(NULL);
}

/**
 *\brief        初始化串口
 *\param[in]    uart    配置数据
 *\return       0       成功
 */
int uart_init()
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    
    uart_param_config(EX_UART_NUM, &uart_config);

    uart_driver_install(EX_UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 100, &g_uart_queue, 0);

    xTaskCreate(uart_event_task, "uart_event_task", 2048, (void*)EX_UART_NUM, 12, NULL);
    
    return 0;
}