#pragma once // 只编译一次
#include "esp_app_cfg.h"


/**
 * \brief      初始化http
 * \param[in]  p_config       config
 * \param[in]  char          *buff      缓存
 * \param[in]  uint           size      缓存大小
 * \return     0-成功，其它失败
 */
int http_init(p_config config, char *buff, uint size);
