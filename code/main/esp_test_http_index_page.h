#pragma once // 只编译一次


/**
 * \brief      主页面
 * \param[out] char *content        数据体
 * \param[out] uint *content_len    数据体长度
 * \return     0-成功，其它失败
 */
int http_index_page(char *content, uint *content_len);
