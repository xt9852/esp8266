#pragma once // 只编译一次


/**
 * \brief      led灯,1秒间隔闪灯
 * \return     无
 */
void gpio_led_init();

/**
 * \brief      初始化GPIO
 * \return     无
 */
void gpio_cpu_init();

/**
 * \brief      读取cpu数据,组成json串
 * \param[out] char *data       配置数据
 * \param[in]  uint json_size   配置数据缓冲区大小
 * \return     int 0-成功，其它失败
 */
int gpio_cpu_get_json(char *json, uint json_size);
