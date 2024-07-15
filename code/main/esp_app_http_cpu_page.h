#pragma once // 只编译一次

/**
 *\brief        cpu页面,模板HTTP_PATH_PROC
 *\param[in]    arg             URL请求参数
 *\param[in]    param           自定义参数
 *\param[out]   content         数据体
 *\param[out]   content_len     输入缓冲区大小,输出数据体长度
 *\return       0               成功
 */
int http_cpu_page(const char *arg, void *param, char *content, unsigned int *content_len);
