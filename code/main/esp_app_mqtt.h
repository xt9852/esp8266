#pragma once // 只编译一次
#include "esp_app_cfg.h"

/**
 *\brief        初始化MQTT
 *\param[in]    mqtt        配置数据
 *\return       0           成功
 */
int mqtt_init(p_config_mqtt mqtt);
