#pragma once // 只编译一次
#include "esp_app_cfg.h" // p_config_uart

/**
 *\brief        初始化串口
 *\param[in]    uart    配置数据
 *\return       0       成功
 */
int uart_init();
