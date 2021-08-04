#pragma once // 只编译一次
#include "esp_test_cfg.h" // p_config_wifi


/**
 * \brief      初始化WIFI
 * \param[in]  p_config_data conf   配置数据
 * \return     无
 */
void wifi_init(p_config_wifi wifi);
