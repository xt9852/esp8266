#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_app_http_cpu_page.h"

#define HTTP_FILE_CPU "<meta http-equiv='Content-Type' content='charset=utf-8'/>\
<style>\
 div{margin:0 0 2;padding:0 4;background:green;float:left;}\
 .p,.k{margin:2;width:24;border-radius:50%;float:left;}\
 .k{margin:2 9;cursor:pointer;}\
 /*向上三角形*/.t,.b{margin:8;border-left:6px solid transparent;border-right:6px solid transparent;border-bottom:6px solid #ccc;float:left;}\
 /*向下三角形*/.b{border-bottom:none;border-top:6px solid #ccc}\
</style>\
<script>\
    function set_text(data, len){\
        for(var key in data){\
            txt=document.getElementById(key).innerText;\
            title=txt.substr(0, txt.lastIndexOf(':0x')+3);\
            number=data[key].toString(16).toUpperCase();\
            document.getElementById(key).innerText=title+number.padStart(len[key], '0');\
        }\
    }\
    function set_back(data, color){\
        for(var key in data){\
            document.getElementById(key).style.background=color[data[key]];\
        }\
    }\
    function set_bus(data, name, type, color){\
        for(var key in data){\
            item=document.getElementById(key+name);\
            item.className=type[data[key]];\
            item.style.borderColor=color[data[key]];\
        }\
    }\
    function on_load(url){\
        req=new XMLHttpRequest();\
        req.open('GET', url);\
        req.send(null);\
        req.onload=function(){\
            if(req.readyState!=4 || req.status!=200){console.log(req.status);return;}\
            rp=JSON.parse(req.responseText);\
            chk=['gray','red'];\
            reg=['blue','blue','red','red','blue','blue','red','red'];\
            dbs=['t','t','b','t','b','t','b','t'];/*数据总线类型*/\
            abs=['b','b','t','t','b','b','b','b'];/*地址总线类型*/\
            dbc=['solid','green','solid','green','solid','green','solid','green'];/*数据总线颜色*/\
            abc=['solid','solid','solid','solid','green','green','green','green'];/*地址总线颜色*/\
            set_text(rp['addr'], rp['addr_len']);\
            set_text(rp['bus'], rp['bus_len']);\
            set_back(rp['chk'], chk);\
            set_back(rp['reg'], reg);\
            set_back(rp['input'], chk);\
            set_bus(rp['reg'], '_d', dbs, dbc);\
            set_bus(rp['reg'], '_a', abs, abc);\
        }\
    }\
    function on_int(item){\
        item.style.background=(item.style.background=='red')?'gray':'red';\
    }\
    function on_clk(item){\
        var clk=(item.style.background=='red')?'0':'1';\
        var int0=(document.getElementById('int0').style.background=='red')?'1':'0';\
        var int1=(document.getElementById('int1').style.background=='red')?'1':'0';\
        on_load('/cpu-data?clk='+clk+'&int0='+int0+'&int1='+int1);\
    }\
</script>\
<body onload=\"on_load('/cpu-data?clk=0&int0=0&int1=0')\">\
    <div>\
        <h3 id='curr'>指令当前地址:0x</h3>\
        <h3 id='next'>指令下条地址:0x</h3>\
        <h3 id='next_true'>真实下条地址:0x</h3>\
    </div>\
    <div>\
        <h3 id='alu'>逻辑输出:0x</h3>\
        <h3 id='data'>数据总线:0x</h3>\
        <h3 id='addr'>地址总线:0x</h3>\
    </div>\
    <div style='clear:left;text-align:center'>\
        <h3 id='int' class='p'>int</h3>\
        <h3 id='je'  class='p'>je</h3>\
        <h3 id='jne' class='p'>jne</h3>\
        <h3 id='jb'  class='p'>jb</h3>\
        <h3 id='jbe' class='p'>jbe</h3>\
        <h3 id='jl'  class='p'>jl</h3>\
        <h3 id='jle' class='p'>jle</h3>\
        <h3 id='int0'class='k' onclick='on_int(this)'>int0</h3>\
        <h3 id='int1'class='k' onclick='on_int(this)'>int1</h3>\
        <h3 id='clk' class='k' onclick='on_clk(this)'>clk</h3>\
    </div>\
    <div style='clear:left;margin:0'>\
        <h3 id='sc_d' class='t'/>\
        <h3 id='sd_d' class='t'/>\
        <h3 id='ss_d' class='t'/>\
        <h3 id='rp_d' class='t'/>\
        <h3 id='rs_d' class='t'/>\
        <h3 id='ra_d' class='t'/>\
        <h3 id='rb_d' class='t'/>\
        <h3 id='rc_d' class='t'/>\
        <h3 id='rd_d' class='t'/>\
        <h3 id='r0_d' class='t'/>\
        <h3 id='r1_d' class='t'/>\
        <h3 id='ri_d' class='t'/>\
        <h3 id='rf_d' class='t'/>\
        <h3 id='ah_d' class='t'/>\
        <h3 id='al_d' class='t'/>\
        <h3 id='mem_d' class='t'/>\
    </div>\
    <div style='clear:left;margin:0;text-align:center'>\
        <h3 id='sc' class='p'>sc</h3>\
        <h3 id='sd' class='p'>sd</h3>\
        <h3 id='ss' class='p'>ss</h3>\
        <h3 id='rp' class='p'>rp</h3>\
        <h3 id='rs' class='p'>rs</h3>\
        <h3 id='ra' class='p'>ra</h3>\
        <h3 id='rb' class='p'>rb</h3>\
        <h3 id='rc' class='p'>rc</h3>\
        <h3 id='rd' class='p'>rd</h3>\
        <h3 id='r0' class='p'>r0</h3>\
        <h3 id='r1' class='p'>r1</h3>\
        <h3 id='ri' class='p'>ri</h3>\
        <h3 id='rf' class='p'>rf</h3>\
        <h3 id='ah' class='p'>ah</h3>\
        <h3 id='al' class='p'>al</h3>\
        <h3 id='mem' class='p'>m</h3>\
    </div>\
    <div style='clear:left;margin:0'>\
        <h3 id='sc_a' class='b'/>\
        <h3 id='sd_a' class='b'/>\
        <h3 id='ss_a' class='b'/>\
        <h3 id='rp_a' class='b'/>\
        <h3 id='rs_a' class='b'/>\
        <h3 id='ra_a' class='b'/>\
        <h3 id='rb_a' class='b'/>\
        <h3 id='rc_a' class='b'/>\
        <h3 id='rd_a' class='b'/>\
        <h3 id='r0_a' class='b'/>\
        <h3 id='r1_a' class='b'/>\
        <h3 id='ri_a' class='b'/>\
        <h3 id='rf_a' class='b'/>\
        <h3 id='ah_a' class='b'/>\
        <h3 id='al_a' class='b'/>\
        <h3 id='mem_a' class='b'/>\
    </div>\
</body>"

/**
 *\brief        cpu页面,模板HTTP_PATH_PROC
 *\param[in]    arg             URL请求参数
 *\param[in]    param           自定义参数
 *\param[out]   content         数据体
 *\param[out]   content_len     输入缓冲区大小,输出数据体长度
 *\return       0               成功
 */
int http_cpu_page(const char *arg, void *param, char *content, unsigned int *content_len)
{
    strncpy(content, HTTP_FILE_CPU, *content_len);
    *content_len = sizeof(HTTP_FILE_CPU) -1;
    return 200;
}
