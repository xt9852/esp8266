#pragma once // 只编译一次

/**
 * \brief      CPU页面数据
 * \param[in]  const char   *param          URL请求参数
 * \param[out] char         *content        数据体
 * \param[out] uint         *content_len    数据体长度
 * \return     0-成功，其它失败
 */
int http_cpu_data(const char *param, char *content, uint *content_len);
