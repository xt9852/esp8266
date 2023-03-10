#pragma once // 只编译一次
#include "esp_test_cfg.h"


/**
 * \brief      初始化http
 * \param[in]  char          *buff      缓存
 * \param[in]  uint           size      缓存大小
 * \param[in]  p_config_http  http
 * \param[in]  p_config_wifi  wifi
 * \param[in]  p_config_light light
 * \return     0-成功，其它失败
 */
int http_init(char *buff, uint size, p_config_http http, p_config_wifi wifi, p_config_light light);
