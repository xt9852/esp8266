#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_test_cfg.h"
#include "esp_test_http_cfg_page.h"


#define HTTP_FILE_CONFIG "<form action='/reboot'><input type='submit' value='reboot'/></form>"\
"<form action='/cfg-http'>"\
    "<input name='" CONFIG_HTTP_PORT "' value='80' maxlength='5' oninput=\"value=value.replace(/[^\\d]/g,'')\"/>"\
    "<input type='submit' value='OK'/>"\
"</form>"\
"<form action='/cfg-wifi'>"\
    "<select name='" CONFIG_WIFI_TYPE "'>"\
        "<option value ='" STR(WIFI_TYPE_STA) "'>" WIFI_TYPE_STA_NAME "</option>"\
        "<option value ='" STR(WIFI_TYPE_AP) "'>" WIFI_TYPE_AP_NAME "</option>"\
    "</select>"\
    "<input name='" CONFIG_WIFI_SSID "' value='ChinaNet-EUG9' maxlength='32'/>"\
    "<input name='" CONFIG_WIFI_PASSWORD "' value='xt9852@dx.com' maxlength='32'/>"\
    "<input type='submit' value='OK'/>"\
"</form>"\
"<form action='/cfg-light'>"\
    "<select name='" LIGHT_CMD_NAME "'>"\
        "<option value ='" STR(LIGHT_CMD_ADD) "'>" LIGHT_CMD_ADD_NAME "</option>"\
        "<option value ='" STR(LIGHT_CMD_DEL) "'>" LIGHT_CMD_DEL_NAME "</option>"\
        "<option value ='" STR(LIGHT_CMD_MOD) "'>" LIGHT_CMD_MOD_NAME "</option>"\
    "</select>"\
    "<input name='" CONFIG_LIGHT_ID "' value='id' maxlength='8' oninput=\"value=value.replace(/[^\\d]/g,'')\"/>"\
    "<input name='" CONFIG_LIGHT_NAME "' value='name' maxlength='32'/>"\
    "<select name='" CONFIG_LIGHT_COLOR "'>"\
        "<option value ='" STR(LIGHT_COLOR_RED) "'>" LIGHT_COLOR_RED_NAME "</option>"\
        "<option value ='" STR(LIGHT_COLOR_BLUE) "'>" LIGHT_COLOR_BLUE_NAME "</option>"\
        "<option value ='" STR(LIGHT_COLOR_GREEN) "'>" LIGHT_COLOR_GREEN_NAME "</option>"\
        "<option value ='" STR(LIGHT_COLOR_YELLOW) "'>" LIGHT_COLOR_YELLOW_NAME "</option>"\
    "</select>"\
    "<select name='" CONFIG_LIGHT_ON "'>"\
        "<option value ='" STR(LIGHT_ON_ON) "'>" LIGHT_ON_ON_NAME "</option>"\
        "<option value ='" STR(LIGHT_ON_OFF) "'>" LIGHT_ON_OFF_NAME "</option>"\
    "</select>"\
    "<input type='submit' value='OK'/>"\
"</form>"


/**
 * \brief      配置页面
 * \param[out] char *content        数据体
 * \param[out] uint *content_len    数据体长度
 * \return     0-成功，其它失败
 */
int http_cfg_page(char *content, uint *content_len)
{
    strncpy(content, HTTP_FILE_CONFIG, *content_len);
    *content_len = sizeof(HTTP_FILE_CONFIG) - 1;
    return 200;
}
