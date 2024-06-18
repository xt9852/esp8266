#pragma once // 只编译一次
#include "esp_app_cfg.h" // p_config_wifi


/**
 * \brief      初始化WIFI
 * \param[in]  p_config_wifi    wifi    配置数据
 * \return     0-成功，其它失败
 */
int wifi_init(p_config_wifi wifi);
