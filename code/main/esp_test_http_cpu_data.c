#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_test_cfg.h"
#include "esp_test_gpio.h"
#include "esp_test_http_cpu_data.h"

/**
 * \brief      CPU页面数据
 * \param[in]  const char   *param          URL请求参数
 * \param[out] char         *content        数据体
 * \param[out] uint         *content_len    数据体长度
 * \return     0-成功，其它失败
 */
int http_cpu_data(const char *param, char *content, uint *content_len)
{
    uint clk;
    char format[64];

    sprintf(format, "%s=%%d", CPU_CLK_NAME);

    ESP_LOGI(TAG, "format:%s", format);

    if (param == NULL || 1 != sscanf(param, format, &clk))
    {
        *content_len = snprintf(content, *content_len, "arg %s error", CPU_CLK_NAME);
        ESP_LOGE(TAG, content);
        return 400;
    }

    gpio_cpu_set_clk(clk);

    gpio_cpu_load_data(); // 载入数据

    uint mi_addr_curr = gpio_cpu_get_data(12);
    uint mi_addr_next = gpio_cpu_get_data(12);
    uint mi_addr_true = gpio_cpu_get_data(12);

    uint bus_data     = gpio_cpu_get_data(8);
    uint bus_addr     = gpio_cpu_get_data(16);
    uint bus_alu      = gpio_cpu_get_data(8);

    uint chk_bi       = gpio_cpu_get_data(1);
    uint chk_int      = gpio_cpu_get_data(1);
    uint chk_je       = gpio_cpu_get_data(1);
    uint chk_jne      = gpio_cpu_get_data(1);
    uint chk_jb       = gpio_cpu_get_data(1);
    uint chk_jbe      = gpio_cpu_get_data(1);
    uint chk_jl       = gpio_cpu_get_data(1);
    uint chk_jle      = gpio_cpu_get_data(1);

    uint ri           = rand()%4;//gpio_cpu_get_data(2);
    uint rf           = rand()%4;//gpio_cpu_get_data(3);
    uint sc           = 0;//gpio_cpu_get_data(3);
    uint sd           = 1;//gpio_cpu_get_data(3);
    uint ss           = 2;//gpio_cpu_get_data(3);
    uint rp           = 3;//gpio_cpu_get_data(3);
    uint rs           = 4;//gpio_cpu_get_data(3);
    uint ra           = 5;//gpio_cpu_get_data(3);
    uint rb           = 6;//gpio_cpu_get_data(3);
    uint rc           = 7;//gpio_cpu_get_data(3);
    uint rd           = rand()%4;//gpio_cpu_get_data(3);
    uint rt           = rand()%4;//gpio_cpu_get_data(2);

    int len;

    len = snprintf(content, *content_len, "{"
                   "\"mi_addr\":\"0x%03X\",\"mi_addr_next\":\"0x%03X\",\"mi_addr_next_true\":\"0x%03X\","
                   "\"bus_data\":\"0x%02X\",\"bus_addr\":\"0x%04X\",\"bus_alu\":\"0x%02X\","
                   "\"chk_bi\":%d,\"chk_int\":%d,\"chk_je\":%d,\"chk_jne\":%d,"
                   "\"chk_jb\":%d,\"chk_jbe\":%d,\"chk_jl\":%d,\"chk_jle\":%d,"
                   "\"ri\":%d,\"rf\":%d,"
                   "\"sc\":%d,\"sd\":%d,\"ss\":%d,\"rp\":%d,\"rs\":%d,"
                   "\"ra\":%d,\"rb\":%d,\"rc\":%d,\"rd\":%d,\"rt\":%d}",
                   mi_addr_curr, mi_addr_next, mi_addr_true,
                   bus_data, bus_addr, bus_alu,
                   chk_bi, chk_int, chk_je, chk_jne,
                   chk_jb, chk_jbe, chk_jl, chk_jle,
                   ri, rf, sc, sd, ss, rp, rs, ra, rb, rc, rd, rt);


    *content_len = len;
    return 200;
}
