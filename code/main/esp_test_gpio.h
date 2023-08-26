#pragma once // 只编译一次


/**
 * \brief      初始化NodeMCU板载led灯
 * \param[in]  uint shine 闪
 * \return     无
 */
void gpio_led_init(uint shine);

/**
 * \brief      led灯
 * \param[in]  uint led 是否点亮led
 * \return     无
 */
void gpio_led(uint led);

/**
 * \brief      初始化74LS595使用的GPIO
 * \return     无
 */
void gpio_74ls595_init();

/**
 * \brief      向74LS595串行输入数据
 * \param[in]  uint data 布尔值
 * \return     无
 */
void gpio_74ls595_save(uint data);

/**
 * \brief      从74LS595并行输出数据
 * \param[in]  无
 * \return     无
 */
void gpio_74ls595_output();

/**
 * \brief      初始化74LS165使用的GPIO
 * \return     无
 */
void gpio_74ls165_init();

/**
 * \brief      向74LS165存入并行数据
 * \return     无
 */
void gpio_74ls165_load_data();

/**
 * \brief      从74LS165串行读取数据
 * \param[in]  int count   组装数据的个数
 * \return     数据
 */
uint gpio_74ls165_get_data(uint count);
