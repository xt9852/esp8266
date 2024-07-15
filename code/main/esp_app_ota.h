#pragma once // 只编译一次
#include "esp_app_cfg.h"

/**
 *\brief        初始化ota
 *\param[in]    queue       队列
 *\return       0           成功
 */
int ota_init(void *queue);
