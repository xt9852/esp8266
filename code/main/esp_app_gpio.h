#pragma once // 只编译一次

#ifndef bool
#define bool unsigned char
#endif

/**
 *\brief        初始化NodeMCU板载led灯
 *\param[in]    led         是否点亮led,0-关,1-亮,2-闪
 *\return                   无
 */
void gpio_led_init(unsigned int led);

/**
 *\brief        led灯
 *\param[in]    led         是否点亮led,0-关,1-亮,2-闪
 *\return                   无
 */
void gpio_led(unsigned int led);

/**
 *\brief        初始化74LS595使用的GPIO
 *\return                   无
 */
void gpio_74ls595_init();

/**
 *\brief        向74LS595串行输入数据
 *\param[in]    data        布尔值
 *\return                   无
 */
void gpio_74ls595_save(bool data);

/**
 *\brief       从74LS595并行输出数据
 *\param[in]   无
 *\return      无
 */
void gpio_74ls595_output();

/**
 *\brief       初始化74LS165使用的GPIO
 *\return      无
 */
void gpio_74ls165_init();

/**
 *\brief       向74LS165存入并行数据
 *\return      无
 */
void gpio_74ls165_load_data();

/**
 *\brief       从74LS165串行读取数据
 *\param[in]   count        组装数据的个数
 *\return                   数据
 */
unsigned int gpio_74ls165_get_data(unsigned int count);
