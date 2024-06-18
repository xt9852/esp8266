#pragma once // 只编译一次
#include "esp_app_cfg.h"


/**
 * \brief      初始化ota
 * \param[in]  void        *queue       队列
 * \param[in]  char        *buff        缓存
 * \param[in]  uint         size        缓存大小
 * \return     0-成功，其它失败
 */
int ota_init(void *queue, char *buff, uint size);
