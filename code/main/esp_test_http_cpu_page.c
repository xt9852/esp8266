#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_test_http_cpu_page.h"

#define HTTP_FILE_CPU "\
<meta http-equiv='Content-Type' content='charset=utf-8'/>\
<style>\
div{margin:5;padding:0 20;background:green;float:left;}\
.k{margin:0 20 19 0;width:25;border-radius:50%;float:left;background:gray;cursor:pointer;}\
.p{margin:0 20 19 0;width:25;border-radius:50%;float:left;}\
</style>\
<script>\
function load()\
{\
req=new XMLHttpRequest();\
req.open('GET', '/cpu');\
req.send(null);\
req.onload=function()\
{\
if(req.readyState!=4 || req.status!=200){console.log(req.status);return;}\
txt=['SEL0选取下一条地址为微指令地址','SEL1选取0x[IR]0组成微指令地址',\
'SEL2选取中断微指令地址0x011','SEL3选取条件跳转微指令地址0x000'];\
opr=['gray','blue','yellow','red','Aqua','yellow','Magenta','yellow'];\
int=['gray','red'];\
rp=JSON.parse(req.responseText);\
mi_addr=document.getElementById('mi_addr');\
mi_next=document.getElementById('mi_next');\
mi_true=document.getElementById('mi_true');\
data=document.getElementById('bus_data');\
addr=document.getElementById('bus_addr');\
out_alu=document.getElementById('out_alu');\
out_shf=document.getElementById('out_shf');\
sel=document.getElementById('sel');\
sel_int=document.getElementById('sel_int');\
sel_ir=document.getElementById('sel_ir');\
sel_jp=document.getElementById('sel_jp');\
sel_je=document.getElementById('sel_je');\
sel_jb=document.getElementById('sel_jb');\
sel_jl=document.getElementById('sel_jl');\
alu=document.getElementById('alu');\
msw=document.getElementById('msw');\
ir=document.getElementById('ir');\
ip=document.getElementById('ip');\
sp=document.getElementById('sp');\
ar=document.getElementById('ar');\
br=document.getElementById('br');\
tr=document.getElementById('tr');\
mi_addr.innerText='指令当前地址：'+rp['mi_addr'];\
mi_next.innerText='指令下条地址：'+rp['mi_addr_next'];\
mi_true.innerText='真实下条地址：'+rp['mi_addr_next_true'];\
out_alu.innerText='逻辑输出：'+rp['out_alu'];\
out_shf.innerText='移位输出：'+rp['out_shf'];\
bus_data.innerText='数据总线：'+rp['bus_data'];\
bus_addr.innerText='地址总线：'+rp['bus_addr'];\
sel.innerText=txt[rp['sel']];\
sel_int.style.background=int[rp['sel_ir']];\
sel_ir.style.background=int[rp['sel_ir']];\
sel_jp.style.background=int[rp['sel_jp']];\
sel_je.style.background=int[rp['sel_je']];\
sel_jb.style.background=int[rp['sel_jb']];\
sel_jl.style.background=int[rp['sel_jl']];\
alu.style.background=opr[rp['alu']];\
msw.style.background=opr[rp['msw']];\
ir.style.background=opr[rp['ir']];\
ip.style.background=opr[rp['ip']];\
sp.style.background=opr[rp['sp']];\
ar.style.background=opr[rp['ar']];\
br.style.background=opr[rp['br']];\
tr.style.background=opr[rp['tr']];\
}\
}\
</script>\
<body onload='load()'>\
<div>\
<h3 id='mi_addr'/>\
<h3 id='mi_next'/>\
<h3 id='mi_true'/>\
<h3 id='clk' class='k'>clk</h3>\
<h3 id='int0' class='k'>int0</h3>\
<h3 id='int1' class='k'>int1</h3>\
</div>\
<div>\
<h3 id='bus_data'/>\
<h3 id='bus_addr'/>\
<h3 id='out_alu'/>\
<h3 id='out_shf'/>\
</div>\
<div>\
<h3 id='sel'/>\
<h3 id='sel_int' class='p'>int</h3>\
<h3 id='sel_ir' class='p'>ir</h3>\
<h3 id='sel_jp' class='p'>jp</h3>\
<h3 id='sel_je' class='p'>je</h3>\
<h3 id='sel_jb' class='p'>jb</h3>\
<h3 id='sel_jl' class='p'>jl</h3>\
<h3 id='msw' class='p' style='clear:left'>mw</h3>\
<h3 id='ir' class='p'>ir</h3>\
<h3 id='ip' class='p'>ip</h3>\
<h3 id='sp' class='p'>sp</h3>\
<h3 id='ar' class='p'>ar</h3>\
<h3 id='br' class='p'>br</h3>\
<h3 id='tr' class='p'>tr</h3>\
<h3 id='alu' class='p' style='clear:left'>alu</h3>\
</div>\
</body>"


/*

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

#define HTTP_FILE_CONFIG "\
<form action='/update_http'>\
<input name='port' value='80'/>\
<input type='submit' value='OK'/>\
</form>\
<form action='/update_wifi'>\
<input name='net' value='AP/STA'/>\
<input name='ssid' value='ssid'/>\
<input name='pwd' value='pwd'/>\
<input type='submit' value='OK'/>\
</form>\
<form action='/update_light'>\
<input name='cmd' value='add/del/mod'/>\
<input name='id' value='id'/>\
<input name='name' value='name'/>\
<input name='color' value='red/green/blue/yellow'/>\
<input name='on' value='on/off'/>\
<input type='submit' value='OK'/>\
</form>\
<form action='/reboot'>\
<input type='submit' value='reboot'/>\
</form>"

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