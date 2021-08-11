#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_test_http_cpu_page.h"

#define HTTP_FILE_CPU "<meta http-equiv='Content-Type' content='charset=utf-8'/>"\
"<style>"\
    "div{margin:0 0 5;padding:0 19;background:green;float:left;}"\
    ".k{margin:3 9;width:26;border-radius:50%;float:left;background:#ccc;cursor:pointer;}"\
    ".p{margin:3;width:26;border-radius:50%;float:left;}"\
    ".t{margin:3;border-left:13px solid transparent;border-right:13px solid transparent;border-bottom:13px solid #ccc;float:left;}"\
    ".b{margin:3;border-left:13px solid transparent;border-right:13px solid transparent;border-top:13px solid #ccc;float:left;}"\
"</style>"\
"<script>"\
    "function load(url)"\
    "{"\
        "req=new XMLHttpRequest();"\
        "req.open('GET', url);"\
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
    "function on_clk(item)"\
    "{"\
        "if (item.style.background=='red')"\
        "{"\
            "item.style.background='#ccc';"\
            "load('/cpu-data?clk=0');"\
        "}"\
        "else"\
        "{"\
            "item.style.background='red';"\
            "load('/cpu-data?clk=1');"\
        "}"\
    "}"\
"</script>"\
"<body onload=\"load('/cpu-data?clk=0')\">"\
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
        "<h3 id='int0' class='k' onclick='on_int()'>int0</h3>"\
        "<h3 id='int1' class='k' onclick='on_int()'>int1</h3>"\
        "<h3 id='clk' class='k' onclick='on_clk(this)'>clk</h3>"\
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