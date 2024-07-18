#pragma once // 只编译一次
#include "driver/uart.h"
#include "esp_app_cfg.h"

/**
 *\brief        初始化串口
 *\param[in]    uart    串口
 *\return       0       成功
 */
int at_uart_init(uart_port_t uart);

/**
 *\brief        发送AT命令连接mqtt
 *\param[in]    mqtt    配置信息
 *\return       0       成功
 */
int at_mqtt_init(p_config_mqtt mqtt);
