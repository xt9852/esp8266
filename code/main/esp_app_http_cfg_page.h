#pragma once // 只编译一次
#include "esp_app_cfg.h"

/**
 * \brief      配置页面
 * \param[in]  p_config config      配置数据
 * \param[out] char    *content     数据体
 * \param[out] uint    *content_len 数据体长度
 * \return     0-成功，其它失败
 */
int http_cfg_page(p_config config, char *content, uint *content_len);
