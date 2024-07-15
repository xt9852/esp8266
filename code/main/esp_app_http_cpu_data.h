#pragma once // 只编译一次

/**
 *\brief        CPU页面数据
 *\param[in]    param           URL请求参数
 *\param[in]    param           自定义参数
 *\param[out]   content         数据体
 *\param[out]   content_len     数据体长度
 *\return       0               成功
 */
int http_cpu_data(const char *arg, void *param, char *content, uint *content_len);
