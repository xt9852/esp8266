#pragma once // 只编译一次
#include "esp_test_cfg.h"


/**
 * \brief      配置http
 * \param[in]  char         *param          URL请求参数
 * \param[in]  p_config_http http           配置数据
 * \param[out] char         *content        数据体
 * \param[out] uint         *content_len    数据体长度
 * \return     0-成功，其它失败
 */
int http_cfg_http(char *param, p_config_http http, char *content, uint *content_len);

/**
 * \brief      配置wifi
 * \param[in]  char         *param          URL请求参数
 * \param[in]  p_config_wifi wifi           配置数据
 * \param[out] char         *content        数据体
 * \param[out] uint         *content_len    数据体长度
 * \return     0-成功，其它失败
 */
int http_cfg_wifi(char *param, p_config_wifi wifi, char *content, uint *content_len);

/**
 * \brief      配置light
 * \param[in]  char          *param         URL请求参数
 * \param[in]  p_config_light light         配置数据
 * \param[out] char          *content       数据体
 * \param[out] uint          *content_len   数据体长度
 * \return     0-成功，其它失败
 */
int http_cfg_light(char *param, p_config_light light, char *content, uint *content_len);

/**
 * \brief      配置light.on
 * \param[in]  char          *param         URL请求参数
 * \param[in]  p_config_light light         配置数据
 * \param[out] char          *content       数据体
 * \param[out] uint          *content_len   数据体长度
 * \return     0-成功，其它失败
 */
int http_cfg_light_on(char *param, p_config_light light, char *content, uint *content_len);

/**
 * \brief      重启
 * \param[out] char          *content       数据体
 * \param[out] uint          *content_len   数据体长度
 * \return     0-成功，其它失败
 */
int http_reboot(char *content, uint *content_len);
