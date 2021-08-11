#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_test_http_cpu_page.h"

#define HTTP_FILE_CPU "<meta http-equiv='Content-Type' content='charset=utf-8'/>"\
"<style>"\
    "div{margin:0 0 5;padding:0 19;background:green;float:left;}"\
    ".k{margin:3 9;width:26;border-radius:50%;float:left;background:gray;cursor:pointer;}"\
    ".p{margin:3;width:26;border-radius:50%;float:left;}"\
    ".t{margin:3;border-left:13px solid transparent;border-right:13px solid transparent;border-bottom:13px solid #ccc;float:left;}"\
    ".b{margin:3;border-left:13px solid transparent;border-right:13px solid transparent;border-top:13px solid #ccc;float:left;}"\
"</style>"\
"<script>"\
    "function load()"\
    "{"\
        "req=new XMLHttpRequest();"\
        "req.open('GET', '/cpu-data');"\
        "req.send(null);"\
        "req.onload=function()"\
        "{"\
            "if(req.readyState!=4 || req.status!=200){console.log(req.status);return;}"\
            "chk=['gray','red'];"\
            "rrw=['blue','red','gray','red'];"\
            "rtc=['green','red','green','solid'];"\
            "rbc=['solid','red','green','green'];"\
            "ttc=['solid','red','green','green'];"\
            "tbc=['green','red','green','solid'];"\
            "rw=['blue','blue','red','red','blue','blue','red','red'];"\
            "ts=['t','t','b','t','b','t','b','t'];"\
            "bs=['b','b','t','t','b','b','b','b'];"\
            "tc=['solid','green','solid','green','solid','green','solid','green'];"\
            "bc=['solid','solid','solid','solid','green','green','green','green'];"\
            "rp=JSON.parse(req.responseText);"\
            "document.getElementById('mi_addr').innerText='指令当前地址：'+rp['mi_addr'];"\
            "document.getElementById('mi_next').innerText='指令下条地址：'+rp['mi_addr_next'];"\
            "document.getElementById('mi_true').innerText='真实下条地址：'+rp['mi_addr_next_true'];"\
            "document.getElementById('bus_data').innerText='数据总线：'+rp['bus_data'];"\
            "document.getElementById('bus_addr').innerText='地址总线：'+rp['bus_addr'];"\
            "document.getElementById('bus_alu').innerText='逻辑输出：'+rp['bus_alu'];"\
            "document.getElementById('chk_bi').style.background=chk[rp['chk_bi']];"\
            "document.getElementById('chk_je').style.background=chk[rp['chk_je']];"\
            "document.getElementById('chk_jb').style.background=chk[rp['chk_jb']];"\
            "document.getElementById('chk_jl').style.background=chk[rp['chk_jl']];"\
            "document.getElementById('chk_int').style.background=chk[rp['chk_int']];"\
            "document.getElementById('chk_jne').style.background=chk[rp['chk_jne']];"\
            "document.getElementById('chk_jbe').style.background=chk[rp['chk_jbe']];"\
            "document.getElementById('chk_jle').style.background=chk[rp['chk_jle']];"\
            "document.getElementById('ri').style.background=rrw[rp['ri']];"\
            "document.getElementById('rf').style.background=rw[rp['rf']];"\
            "document.getElementById('sc').style.background=rw[rp['sc']];"\
            "document.getElementById('sd').style.background=rw[rp['sd']];"\
            "document.getElementById('ss').style.background=rw[rp['ss']];"\
            "document.getElementById('rp').style.background=rw[rp['rp']];"\
            "document.getElementById('rs').style.background=rw[rp['rs']];"\
            "document.getElementById('ra').style.background=rw[rp['ra']];"\
            "document.getElementById('rb').style.background=rw[rp['rb']];"\
            "document.getElementById('rc').style.background=rw[rp['rc']];"\
            "document.getElementById('rd').style.background=rw[rp['rd']];"\
            "document.getElementById('rt').style.background=rrw[rp['rt']];"\
            "document.getElementById('ri_t').className='b';"\
            "document.getElementById('rf_t').className=ts[rp['rf']];"\
            "document.getElementById('sc_t').className=ts[rp['sc']];"\
            "document.getElementById('sd_t').className=ts[rp['sd']];"\
            "document.getElementById('ss_t').className=ts[rp['ss']];"\
            "document.getElementById('rp_t').className=ts[rp['rp']];"\
            "document.getElementById('rs_t').className=ts[rp['rs']];"\
            "document.getElementById('ra_t').className=ts[rp['ra']];"\
            "document.getElementById('rb_t').className=ts[rp['rb']];"\
            "document.getElementById('rc_t').className=ts[rp['rc']];"\
            "document.getElementById('rd_t').className=ts[rp['rd']];"\
            "document.getElementById('rt_t').className='t';"\
            "document.getElementById('ri_t').style.borderColor=rtc[rp['ri']];"\
            "document.getElementById('rf_t').style.borderColor=tc[rp['rf']];"\
            "document.getElementById('sc_t').style.borderColor=tc[rp['sc']];"\
            "document.getElementById('sd_t').style.borderColor=tc[rp['sd']];"\
            "document.getElementById('ss_t').style.borderColor=tc[rp['ss']];"\
            "document.getElementById('rp_t').style.borderColor=tc[rp['rp']];"\
            "document.getElementById('rs_t').style.borderColor=tc[rp['rs']];"\
            "document.getElementById('ra_t').style.borderColor=tc[rp['ra']];"\
            "document.getElementById('rb_t').style.borderColor=tc[rp['rb']];"\
            "document.getElementById('rc_t').style.borderColor=tc[rp['rc']];"\
            "document.getElementById('rd_t').style.borderColor=tc[rp['rd']];"\
            "document.getElementById('rt_t').style.borderColor=ttc[rp['rt']];"\
            "document.getElementById('ri_b').className='b';"\
            "document.getElementById('rf_b').className=bs[rp['rf']];"\
            "document.getElementById('sc_b').className=bs[rp['sc']];"\
            "document.getElementById('sd_b').className=bs[rp['sd']];"\
            "document.getElementById('ss_b').className=bs[rp['ss']];"\
            "document.getElementById('rp_b').className=bs[rp['rp']];"\
            "document.getElementById('rs_b').className=bs[rp['rs']];"\
            "document.getElementById('ra_b').className=bs[rp['ra']];"\
            "document.getElementById('rb_b').className=bs[rp['rb']];"\
            "document.getElementById('rc_b').className=bs[rp['rc']];"\
            "document.getElementById('rd_b').className=bs[rp['rd']];"\
            "document.getElementById('rt_b').className='t';"\
            "document.getElementById('ri_b').style.borderColor=rbc[rp['ri']];"\
            "document.getElementById('rf_b').style.borderColor=bc[rp['rf']];"\
            "document.getElementById('sc_b').style.borderColor=bc[rp['sc']];"\
            "document.getElementById('sd_b').style.borderColor=bc[rp['sd']];"\
            "document.getElementById('ss_b').style.borderColor=bc[rp['ss']];"\
            "document.getElementById('rp_b').style.borderColor=bc[rp['rp']];"\
            "document.getElementById('rs_b').style.borderColor=bc[rp['rs']];"\
            "document.getElementById('ra_b').style.borderColor=bc[rp['ra']];"\
            "document.getElementById('rb_b').style.borderColor=bc[rp['rb']];"\
            "document.getElementById('rc_b').style.borderColor=bc[rp['rc']];"\
            "document.getElementById('rd_b').style.borderColor=bc[rp['rd']];"\
            "document.getElementById('rt_b').style.borderColor=tbc[rp['rt']];;"\
        "}"\
    "}"\
"</script>"\
"<body onload='load()'>"\
    "<div>"\
        "<h3 id='mi_addr'/>"\
        "<h3 id='mi_next'/>"\
        "<h3 id='mi_true'/>"\
    "</div>"\
    "<div>"\
        "<h3 id='bus_data'/>"\
        "<h3 id='bus_addr'/>"\
        "<h3 id='bus_alu'/>"\
    "</div>"\
    "<div style='clear:left;text-align:center'>"\
        "<h3 id='chk_bi'  class='p'>bi</h3>"\
        "<h3 id='chk_int' class='p'>int</h3>"\
        "<h3 id='chk_je'  class='p'>je</h3>"\
        "<h3 id='chk_jne' class='p'>jne</h3>"\
        "<h3 id='chk_jb'  class='p'>jb</h3>"\
        "<h3 id='chk_jbe' class='p'>jbe</h3>"\
        "<h3 id='chk_jl'  class='p'>jl</h3>"\
        "<h3 id='chk_jle' class='p'>jle</h3>"\
        "<h3 id='int0'    class='k'>int0</h3>"\
        "<h3 id='int1'    class='k'>int1</h3>"\
        "<h3 id='clk'     class='k'>clk</h3>"\
    "</div>"\
    "<div style='clear:left;margin:0'>"\
        "<h3 id='ri_t' class='t'></h3>"\
        "<h3 id='rf_t' class='t'></h3>"\
        "<h3 id='sc_t' class='t' style='margin:3 5'></h3>"\
        "<h3 id='sd_t' class='t'></h3>"\
        "<h3 id='ss_t' class='t'></h3>"\
        "<h3 id='rp_t' class='t'></h3>"\
        "<h3 id='rs_t' class='t'></h3>"\
        "<h3 id='ra_t' class='t'></h3>"\
        "<h3 id='rb_t' class='t'></h3>"\
        "<h3 id='rc_t' class='t'></h3>"\
        "<h3 id='rd_t' class='t'></h3>"\
        "<h3 id='rt_t' class='t'></h3>"\
    "</div>"\
    "<div style='clear:left;margin:0;text-align:center'>"\
        "<h3 id='ri' class='p'>ri</h3>"\
        "<h3 id='rf' class='p'>rf</h3>"\
        "<h3 id='sc' class='p' style='margin:3 5'>sc</h3>"\
        "<h3 id='sd' class='p'>sd</h3>"\
        "<h3 id='ss' class='p'>ss</h3>"\
        "<h3 id='rp' class='p'>rp</h3>"\
        "<h3 id='rs' class='p'>rs</h3>"\
        "<h3 id='ra' class='p'>ra</h3>"\
        "<h3 id='rb' class='p'>rb</h3>"\
        "<h3 id='rc' class='p'>rc</h3>"\
        "<h3 id='rd' class='p'>rd</h3>"\
        "<h3 id='rt' class='p'>rt</h3>"\
    "</div>"\
    "<div style='clear:left;margin:0'>"\
        "<h3 id='ri_b' class='b'></h3>"\
        "<h3 id='rf_b' class='b'></h3>"\
        "<h3 id='sc_b' class='b' style='margin:3 5'></h3>"\
        "<h3 id='sd_b' class='b'></h3>"\
        "<h3 id='ss_b' class='b'></h3>"\
        "<h3 id='rp_b' class='b'></h3>"\
        "<h3 id='rs_b' class='b'></h3>"\
        "<h3 id='ra_b' class='b'></h3>"\
        "<h3 id='rb_b' class='b'></h3>"\
        "<h3 id='rc_b' class='b'></h3>"\
        "<h3 id='rd_b' class='b'></h3>"\
        "<h3 id='rt_b' class='b'></h3>"\
    "</div>"\
"</body>"

/*
"txt=['SEL0选取下一条地址为微指令地址',"\
     "'SEL1选取0x[IR]0组成微指令地址',"\
     "'SEL2选取中断微指令地址0x011',"\
     "'SEL3选取条件跳转微指令地址0x000'];"\

function set(name, data, node){\
s = '';\
k = new Array();\
c = document.getElementById('c');\
a = document.getElementById(node);\
n = document.getElementById(name + 'n');\
b = c.parentNode;\
for (var key in data)\
{\
k.push(key);\
s += data[key];\
}\
if (n)\
{\
    for (i = 0; i < s.length; i += 4)\
    {\
        n.innerText += hex[s.substring(i, i + 4)];\
    }\
    n.innerText += 'H';\
}\
else\
{\
    k.sort();\
}\
for (var i in k)\
{\
    id = k[i];\
    t = c.cloneNode(true);\
    t.id = name + i;\
    t.innerText = id;\
    t.style.float = 'left';\
    t.style.width = '22px';\
    t.style.height = '22px';\
    t.style['margin-left'] = '5px';\
    t.style['border-radius'] = '50%';\
    t.style['text-align'] = 'center';\
    t.style.background = (data[id] == 1 ? 'red' : 'gray');\
    b.insertBefore(t, a);\
}\
}\

var hex = {'0000':'0','0001':'1','0010':'2','0011':'3','0100':'4','0101':'5','0110':'6','0111':'7',\
'1000':'8','1001':'9','1010':'A','1011':'B','1100':'C','1101':'D','1110':'E','1111':'F'}\

request = new XMLHttpRequest()\
request.open('GET', '/cpu')\
request.send(null)\
request.onload = function(){\
if (request.readyState != 4 || request.status != 200){console.log(request.status);return;}\
r = JSON.parse(request.responseText)\
set('db',  r.db,  'ab');\
set('ab',  r.ab,  'fb');\
set('fb',  r.fb,  'sb');\
set('sb',  r.sb,  'tb');\
<d id='c'></d>\
<d id='db'  class='t0'>数据总线:</d><d id='dbn' class='nm'>0x0123</d>\
<d id='ab'  class='t0'>地址总线:</d><d id='abn' class='nm'>0x4567</d>\
<d id='fb'  class='t0'>ALU输出: </d><d id='fbn' class='nm'>0x89AB</d>\
<d id='sb'  class='t0'>移位输出:</d><d id='sbn' class='nm'>0xCDEF</d>\


set('tb',  r.tb,  'ib');\
set('ib',  r.ib,  'mb');\
set('mb',  r.mb,  'mia');\
set('mia', r.mia, 'min');\
set('min', r.min, 'sel');\
set('sel', r.sel, 'ir');\
set('ir',  r.ir,  'msw');\
set('msw', r.msw, 'ip');\
set('ip',  r.ip,  'sp');\
set('sp',  r.sp,  'ar');\
set('ar',  r.ar,  'br');\
set('br',  r.br,  'tr');\
set('tr',  r.tr,  'alu');\
set('alu', r.alu, 'mem');\
set('mem', r.mem, 'jne');\
set('jne', r.jne, 'jmp');\
set('jmp', r.jmp, 'ses');\
set('ses', r.ses, 'clk');\
set('clk', r.clk, 'int');\
set('int', r.int, 'end');\
<d id='tb'  class='t0'>临时总线:</d><d id='tbn' class='nm'></d>\
<d id='ib'  class='t0'>指令总线:</d><d id='ibn' class='nm'></d>\
<d id='mb'  class='t0'>内存数据:</d><d id='mbn' class='nm'></d>\
<d id='mia' class='t0'>当前地址:</d><d id='mian'class='nm'></d>\
<d id='min' class='t0'>下条地址:</d><d id='minn'class='nm'></d>\
<d id='sel' class='t1'>微指令SEL</d><d id='ir'  class='t1'>微指令IR</d>\
<d id='msw' class='t1'>微指令MSW</d><d id='ip'  class='t1'>微指令IP</d>\
<d id='sp'  class='t1'>微指令SP</d><d id='ar'  class='t1'>微指令AR</d>\
<d id='br'  class='t1'>微指令BR</d><d id='tr'  class='t1'>微指令TR</d>\
<d id='alu' class='t1'>微指令ALU</d><d id='mem' class='t1'>微指令MEM</d>\
<d id='jne' class='t1'>ALU不等信号</d><d id='jmp' class='t1'>跳转信号</d>\
<d id='ses' class='t1'>选择器信号</d><d id='clk' class='t1'>心跳信号</d>\
<d id='int' class='t1'>中断信号</d>\

*/


/**
 * \brief      cpu页面
 * \param[out] char *content        数据体
 * \param[out] uint *content_len    数据体长度
 * \return     0-成功，其它失败
 */
int http_cpu_page(char *content, uint *content_len)
{
    strncpy(content, HTTP_FILE_CPU, *content_len);
    *content_len = sizeof(HTTP_FILE_CPU) -1;
    return 200;
}