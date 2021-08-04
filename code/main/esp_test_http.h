#pragma once // 只编译一次
#include "esp_test_cfg.h"


/**
 * \brief      初始化http
 * \param[in]  p_config_http  http
 * \param[in]  p_config_wifi  wifi
 * \param[in]  p_config_light light
 * \param[in]  uint          *light_count
 * \param[in]  uint           light_max
 * \param[in]  char          *buff          缓存
 * \param[in]  uint           buff_size     缓存大小
 * \return     0-成功，其它失败
 */
int http_init(p_config_http http, p_config_wifi wifi, 
              p_config_light light, uint *light_count, uint light_max,
              char *buff, uint buff_size);

/**
 * \brief      创建监听socket
 * \param[in]  uint port   端口
 * \return     0-成功，其它失败
 */
int http_create_listen_socket(uint port);

/**
 * \brief      处理客户端的请求
 * \param[in]  int client_sock   客户端socket
 * \return     0-成功，其它失败
 */
int http_process_client_request(int client_sock);

/**
 * \brief      处理客户端的连接
 * \param[in]  int listen_sock   监听socket
 * \return     0-成功，其它失败
 */
int http_process_client_connect(int listen_sock);
