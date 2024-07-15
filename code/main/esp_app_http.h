#pragma once // 只编译一次
#include "esp_app_cfg.h"

/**
 *\brief        http处理函数
 *\param[in]    arg             URL请求参数
 *\param[in]    param           自定义参数
 *\param[out]   content         数据体
 *\param[out]   content_len     输入缓冲区大小,输出数据体长度
 *\return       0               成功
 */
typedef int (*HTTP_PATH_PROC)(const char *arg, void *param, char *content, unsigned int *content_len);

/// 路径与函数对应
typedef struct _path_proc
{
    char*           path;
    HTTP_PATH_PROC  proc;
    void*           param;

} t_path_proc, *p_path_proc;

/**
 *\brief        初始化http
 *\param[in]    port            商品呺
 *\param[in]    data            路径与函数对应数据
 *\param[in]    data_count      路径与函数对应数据数量
 *\return       0               成功
 */
int http_init(unsigned short port, p_path_proc data, unsigned int data_count);
