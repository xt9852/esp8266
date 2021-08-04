#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_test_http_index_data.h"

/**
 * \brief      主页面数据
 * \param[out] char *content        数据体
 * \param[out] uint *content_len    数据体长度
 * \return     0-成功，其它失败
 */
int http_index_data(char *content, uint *content_len)
{
    int v = 220 + rand()%20 - 10;
    int i = rand()%20;
    int w = v * i;

    *content_len = (uint)snprintf(content, *content_len,
                                 "{\"t1\":%d,\"t2\":%d,"
                                 "\"t3\":%d,\"t4\":%d,\"t5\":%d,"
                                 "\"t6\":%d,\"t7\":%d,\"t8\":%d}",
                                 rand()%50, rand()%100,
                                 rand()%10000, rand()%10000, rand()%10000,
                                 v, i, w);

    return 200;
}
