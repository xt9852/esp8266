#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_test_http_index_page.h"

#define HTTP_FILE_INDEX "<meta http-equiv='Content-Type' content='charset=utf-8'/>\
<style>div{margin:5;padding:0 20;background:green;float:left;}</style>"\
"<body onload='load()'>"\
    "<div>"\
        "<h3 id='t3'/>"\
        "<h3 id='t4'/>"\
        "<h3 id='t5'/>"\
    "</div>"\
    "<div>"\
        "<h3 id='t6'/>"\
        "<h3 id='t7'/>"\
        "<h3 id='t8'/>"\
    "</div>"\
    "<div>"\
        "<h3 id='t1'/>"\
        "<h3 id='t2'/>"\
    "</div>"\
"</body>"\
"<script>"\
    "function load()"\
    "{"\
        "req = new XMLHttpRequest();"\
        "req.open('GET', '/index.json');"\
        "req.send(null);"\
        "req.onload=function()"\
        "{"\
            "if(req.readyState!=4 || req.status!=200){console.log(req.status);return;}"\
            "rp=JSON.parse(req.responseText);"\
            "t1=document.getElementById('t1');"\
            "t2=document.getElementById('t2');"\
            "t3=document.getElementById('t3');"\
            "t4=document.getElementById('t4');"\
            "t5=document.getElementById('t5');"\
            "t6=document.getElementById('t6');"\
            "t7=document.getElementById('t7');"\
            "t8=document.getElementById('t8');"\
            "t1.innerText='温度：'+rp['t1']+'\u2103';"\
            "t2.innerText='湿度：'+rp['t2']+'%';"\
            "t3.innerText='水表：'+rp['t3'];"\
            "t4.innerText='电表：'+rp['t4'];"\
            "t5.innerText='气表：'+rp['t5'];"\
            "t6.innerText='电压：'+rp['t6']+'V';"\
            "t7.innerText='电流：'+rp['t7']+'A';"\
            "t8.innerText='功率：'+rp['t6']*rp['t7']+'W';"\
        "}"\
    "}"\
"</script>"

/**
 * \brief      主页面
 * \param[out] char *content        数据体
 * \param[out] uint *content_len    数据体长度
 * \return     0-成功，其它失败
 */
int http_index_page(char *content, uint *content_len)
{
    strncpy(content, HTTP_FILE_INDEX, *content_len);
    *content_len = sizeof(HTTP_FILE_INDEX) - 1;
    return 200;
}
