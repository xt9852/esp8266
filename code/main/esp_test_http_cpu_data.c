#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_test_cfg.h"
#include "esp_test_gpio.h"
#include "esp_test_http_cpu_data.h"

static uint g_clk  = 0;
static uint g_int0 = 0;
static uint g_int1 = 0;

/**
 * \brief      CPU页面数据
 * \param[in]  const char   *param          URL请求参数
 * \param[out] char         *content        数据体
 * \param[out] uint         *content_len    数据体长度
 * \return     0-成功，其它失败
 */
int http_cpu_data(const char *param, char *content, uint *content_len)
{
    int len;
    uint clk;
    uint int0;
    uint int1;
    char format[64];

    sprintf(format, "%s=%%d&%s=%%d&%s=%%d", CPU_CLK_NAME, CPU_INT0_NAME, CPU_INT1_NAME);

    ESP_LOGI(TAG, "format:%s", format);

    if (param == NULL || 3 != sscanf(param, format, &clk, &int0, &int1))
    {
        *content_len = snprintf(content,
                                *content_len,
                                "arg %s %s %s error",
                                CPU_CLK_NAME,
                                CPU_INT0_NAME,
                                CPU_INT1_NAME);
        ESP_LOGE(TAG, content);
        return 400;
    }

    if (2 != clk) // 不发送数据
    {
        g_clk  = clk;
        g_int0 = int0;
        g_int1 = int1;

        // led灯
        gpio_led(!clk);

        // 输出数据
        gpio_cpu_set_data(0);
        gpio_cpu_set_data(0);
        gpio_cpu_set_data(0);
        gpio_cpu_set_data(0);

        gpio_cpu_set_data(0);
        gpio_cpu_set_data(int0);
        gpio_cpu_set_data(int1);
        gpio_cpu_set_data(clk);

        gpio_cpu_out_data();
    }
    else
    {

    }

    // 载入数据
    gpio_cpu_load_data();

    uint mi_addr_curr = gpio_cpu_get_data(12);
    uint mi_addr_next = gpio_cpu_get_data(12);
    uint mi_addr_true = gpio_cpu_get_data(12);

    uint bus_data     = gpio_cpu_get_data(8);
    uint bus_addr     = gpio_cpu_get_data(16);
    uint bus_alu      = gpio_cpu_get_data(8);

    uint sc           = 0;//gpio_cpu_get_data(3);
    uint sd           = 1;//gpio_cpu_get_data(3);
    uint ss           = 2;//gpio_cpu_get_data(3);
    uint rp           = 3;//gpio_cpu_get_data(3);
    uint rs           = 4;//gpio_cpu_get_data(3);
    uint ra           = 5;//gpio_cpu_get_data(3);
    uint rb           = 6;//gpio_cpu_get_data(3);
    uint rc           = 7;//gpio_cpu_get_data(3);
    uint rd           = rand()%8;//gpio_cpu_get_data(3);
    uint mem          = rand()%8;//gpio_cpu_get_data(3);
    uint alu          = gpio_cpu_get_data(7);

    uint chk_int      = gpio_cpu_get_data(1);
    uint chk_je       = gpio_cpu_get_data(1);
    uint chk_jne      = gpio_cpu_get_data(1);
    uint chk_jb       = gpio_cpu_get_data(1);
    uint chk_jbe      = gpio_cpu_get_data(1);
    uint chk_jl       = gpio_cpu_get_data(1);
    uint chk_jle      = gpio_cpu_get_data(1);

    uint r0           = gpio_cpu_get_data(3);
    uint r1           = gpio_cpu_get_data(3);

    uint ri           = gpio_cpu_get_data(2);
    uint rf           = gpio_cpu_get_data(2);

    uint al           = gpio_cpu_get_data(2);
    uint ah           = gpio_cpu_get_data(1);

    uint dev          = gpio_cpu_get_data(4);
    uint ram          = gpio_cpu_get_data(5);
    uint rom          = gpio_cpu_get_data(3);
    uint sel          = gpio_cpu_get_data(2);

    /*
    if (al == 0 || al == 2) // al-clr=0,清空中断标记
    {
        g_int0 = 0;
        g_int1 = 0;
    }
    */

    len = snprintf(content, *content_len, "{"
          "\"addr\":{\"curr\":%d,\"next\":%d,\"next_true\":%d},"
          "\"addr_len\":{\"curr\":3,\"next\":3,\"next_true\":3},"
          "\"bus\":{\"alu\":%d,\"data\":%d,\"addr\":%d},"
          "\"bus_len\":{\"alu\":2,\"data\":2,\"addr\":4},"
          "\"chk\":{\"int\":%d,\"je\":%d,\"jne\":%d,\"jb\":%d,\"jbe\":%d,\"jl\":%d,\"jle\":%d},"
          "\"reg\":{\"sc\":%d,\"sd\":%d,\"ss\":%d,\"rp\":%d,\"rs\":%d,"
                   "\"ra\":%d,\"rb\":%d,\"rc\":%d,\"rd\":%d,"
                   "\"r0\":%d,\"r1\":%d},"
          "\"mem\":%d,\"alu\":%d,"
          "\"ri\":%d,\"rf\":%d,\"ah\":%d,\"al\":%d,"
          "\"dev\":%d,\"ram\":%d,\"rom\":%d,\"sel\":%d,"
          "\"input\":{\"clk\":%d,\"int0\":%d,\"int1\":%d}}",
          mi_addr_curr, mi_addr_next, mi_addr_true,
          bus_alu, bus_data, bus_addr,
          chk_int, chk_je, chk_jne, chk_jb, chk_jbe, chk_jl, chk_jle,
          sc, sd, ss, rp, rs,
          ra, rb, rc, rd,
          r0, r1,
          mem, alu,
          ri, rf, ah, al,
          dev, ram, rom, sel,
          g_clk, g_int0, g_int1);

    *content_len = len;
    return 200;
}
