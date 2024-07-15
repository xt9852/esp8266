#pragma once // 只编译一次
#include "esp_app_cfg.h"

/**
 *\brief        重启,模板HTTP_PATH_PROC
 *\param[in]    arg             URL请求参数
 *\param[in]    param           自定义参数
 *\param[out]   content         数据体
 *\param[out]   content_len     输入缓冲区大小,输出数据体长度
 *\return       0               成功
 */
int http_reboot(const char *arg, void *param, char *content, unsigned int *content_len);

/**
 *\brief        配置wifi,模板HTTP_PATH_PROC
 *\param[in]    arg             URL请求参数
 *\param[in]    param           wifi数据
 *\param[out]   content         数据体
 *\param[out]   content_len     输入缓冲区大小,输出数据体长度
 *\return       0               成功
 */
int http_cfg_wifi(const char *arg, void *param, char *content, unsigned int *content_len);

/**
 *\brief        配置http,模板HTTP_PATH_PROC
 *\param[in]    arg             URL请求参数
 *\param[in]    param           http数据
 *\param[out]   content         数据体
 *\param[out]   content_len     输入缓冲区大小,输出数据体长度
 *\return       0               成功
 */
int http_cfg_http(const char *arg, void *param, char *content, unsigned int *content_len);

/**
 *\brief        配置mqtt,模板HTTP_PATH_PROC
 *\param[in]    arg             URL请求参数
 *\param[in]    param           mqtt数据
 *\param[out]   content         数据体
 *\param[out]   content_len     数据体长度
 *\return       200             成功
 */
int http_cfg_mqtt(const char *arg, void *param, char *content, unsigned int *content_len);

/**
 *\brief        配置light,模板HTTP_PATH_PROC
 *\param[in]    arg             URL请求参数
 *\param[in]    param           light数据
 *\param[out]   content         数据体
 *\param[out]   content_len     数据体长度
 *\return       200             成功
 */
int http_cfg_light(const char *arg, void *param, char *content, unsigned int *content_len);
