/**
 * Copyright:   2024, XT Tech. Co., Ltd.
 * Description: 主模块实现
 * Author:      张海涛
 * Version:     0.0.1
 * Code:        UTF-8(无BOM)
 * Date:        2024-06-18
 * History:     2024-06-18 创建此文件。
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include "cJSON.h"
#include "md5.h"
#include "miniz.h"
#include "sha256.h"
#include "xt_base64.h"

#define BUFF_SIZE      16*1024*1024
#define CVS_TYPE_ID    "app-0,data-1"
#define CVS_SUBTYPE_ID "factory-0,test-20,ota-00,phy-01,nvs-02,coredump-03,nvs_keys-04,efuse-05,ota_0-10,ota_1-11,ota_2-12,ota_3-13,ota_4-14,ota_5-15,ota_6-16,ota_7-17,ota_8-18,ota_9-19,ota_10-1a,ota_11-1b,ota_12-1c,ota_13-1d,ota_14-1e,ota_15-1f,esphttpd-80,fat-81,spiffs-82"

#define STUB_DATA_ADDR  0x3fffaba4
#define STUB_TEXT_ADDR  0x4010e000
#define STUB_ENTRY_ADDR 0x4010e004
#define STUB_TEXT       "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"
#define STUB_DATA       "eJx10T1Lw0AYwPG7pEmaprVXHao4VHHsUhEnB086ioP0Cyh+BB0EF4ugn0BcBAcdXBz9ACK4Ojo6CFpf6IsvxUX0H3PBEOnBjyeX3PPc3ZOT6ve842SEZXvCtgOxeK/0Nq6w96D0WV/pW2KtpfRuK5pvPir9ifOnaB5qPEex1lY66Chd7/x965nnblfpg57SX69KX78pffr+tyZ28xHFQ2y9KL3M3hXeHbFfm/33iVW+bfT/56aVWbuEWfKlEMKCjQwcePBRQAljmMQ0FrCKJi4gKdCEpIikiKSIdJFFHgqjmEANGiti4JCGlThXfLb4fCHX8Iys4Rs5IzDyRsGIR/LebqKWn8gP1xdNL0ZQxjgqmEIVM5hDHQ2sYR07OMYl7sI7ycF3H9SLdE/SvUn3KN2rWHKo0rDIer6bc4JM3i5YQ7L4+y+lyf0BypxxDQ=="

enum
{
    APP = 0,
    OTA,                                    // 生成OTA数据
    CSV,                                    // 生成分区表数据
    BIN,                                    // 生成BIN数据
    ROM,                                    // 烧录固件
    RAM,                                    // 向内存写入数据
};

typedef struct _arg
{
    unsigned int            type;           // 类型:OTA,CVS,BIN,FLASH
    unsigned int            com;            // 串口号
    unsigned int            size;           // 数据的长度
    unsigned int            addr;           // 数据的地址
    unsigned int            version;        // 版本:1,3
    const char              *input;         // 输入文件
    const char              *output;        // 输出文件

} t_arg, *p_arg;

typedef struct _cvs
{
    unsigned short          magic;          // 0x50AA
    unsigned char           type;           // app-0,data-1
    unsigned char           type_sub;       // factory-0x00,test-0x20,ota-0x00, phy-0x01, nvs-0x02, coredump-0x03, nvs_keys-0x04, efuse-0x05, esphttpd-0x80, fat-0x81, spiffs-0x82
    unsigned int            offset;         // 位置
    unsigned int            size;           // 大小
    char                    name[16];       // 名称
    unsigned int            flags;          // 标记

} t_cvs, *p_cvs;

typedef struct _bin
{
    unsigned char           magic;          // 固定0xE9
    unsigned char           sec_num;        // 段数量
    unsigned char           flash_mod;      // FLASH模式          QIO,QOUT,DIO,DOUT,FAST_READ,SLOW_READ
    unsigned char           flash_sf;       // FLASH大小和频率    大小:1MB,2MB,4MB,8MB,16MB 频率40M,26M,20M,80M
    unsigned int            entry;          // 入口地址

} t_bin, *p_bin;

typedef struct _sec_head
{
    unsigned int            addr;           // 段内存地址
    unsigned int            size;           // 段大小

} t_sec_head, *p_sec_head;

typedef struct _sec
{
    t_sec_head              head;           // 段头
    unsigned int            offset;         // 段数据在文件中的位置
    unsigned int            pad;            // 4字节对齐

} t_sec, *p_sec;

typedef struct _Elf32_Ehdr
{
    unsigned char           e_ident[16];    // 0x7F+ELF
    unsigned short          e_type;         // 该文件的类型
    unsigned short          e_machine;      // 体系结构0x5e
    unsigned int            e_version;      // 文件的版本
    unsigned int            e_entry;        // 入口地址
    unsigned int            e_phoff;        // Program header table 在文件中的偏移量
    unsigned int            e_shoff;        // Section header table 在文件中的偏移量
    unsigned int            e_flags;        // IA32而言，此项为0
    unsigned short          e_ehsize;       // ELF header大小
    unsigned short          e_phentsize;    // Program header table中每一个条目的大小
    unsigned short          e_phnum;        // Program header table中有多少个条目
    unsigned short          e_shentsize;    // Section header table中的每一个条目的大小
    unsigned short          e_shnum;        // Section header table中有多少个条目
    unsigned short          e_shstrndx;     // 包含节名称的字符串是第几个节

} Elf32_Ehdr, *p_Elf32_Ehdr;

typedef struct _Elf32_Shdr
{
    unsigned int            sh_name;        // Section name (string tbl index)
    unsigned int            sh_type;        // Section type
    unsigned int            sh_flags;       // Section flags
    unsigned int            sh_addr;        // Section virtual addr at execution
    unsigned int            sh_offset;      // Section file offset
    unsigned int            sh_size;        // Section size in bytes
    unsigned int            sh_link;        // Link to another section
    unsigned int            sh_info;        // Additional section information
    unsigned int            sh_addralign;   // Section alignment
    unsigned int            sh_entsize;     // Entry size if section holds table

} Elf32_Shdr, *p_Elf32_Shdr;

typedef struct _esp_loader_req_head
{
    unsigned char           zero;           // 固定为0x00
    unsigned char           op;             // 命令码
    unsigned short          data_len;       // 数据长度
    unsigned int            checksum;       // 校验码

} t_esp_loader_req_head, *p_esp_loader_req_head;

#pragma pack(push)
#pragma pack(1)

typedef struct _esp_loader_req
{
    t_esp_loader_req_head   head;           // 命令头
    unsigned int            data[1];        // 数据

} t_esp_loader_req, *p_esp_loader_req;

typedef struct _esp_loader_rsp
{
    unsigned char           one;            // 固定为0x01
    unsigned char           op;             // 结果
    unsigned short          data_len;       // 数据长度
    unsigned int            value;          // 值,内存读取命令应答使用
    unsigned char           data[1];        // 数据

} t_esp_loader_rsp, *p_esp_loader_rsp;

#pragma pack(pop)

static t_arg    g_arg;
static HANDLE   g_com;

unsigned char com_checksum(unsigned char *data, unsigned int data_len, unsigned char checksum)
{
    for (unsigned int i = 0; i < data_len; i++)
    {
        checksum ^= data[i];
    }

    return checksum;
}

int get_file_data(const char *filename, void *buf, unsigned int *len)
{
    unsigned int max = *len;

    FILE *fp = NULL;

    if (fopen_s(&fp, filename, "rb") != 0)
    {
        printf("open:%s error %d\n", filename, errno);
        return -1;
    }

    printf("open:%s ok\n", filename);

    *len = fread(buf, 1, max, fp);

    fclose(fp);

    if (*len == max)
    {
        printf("file too big max:%d\n", max);
        return -2;
    }

    printf("read len:%d\n", *len);
    return 0;
}

int put_file_data(const char *filename, void *buf, unsigned int len)
{
    FILE *fp = NULL;

    if (fopen_s(&fp, filename, "wb+") != 0)
    {
        printf("make:%s error %d\n", filename, errno);
        return -1;
    }

    printf("make:%s ok\n", filename);

    len = fwrite(buf, 1, len, fp);
    fclose(fp);

    printf("write len:%d\n", len);
    return 0;
}

void com_printf_data(unsigned char *data, unsigned int len)
{
    for (unsigned int i = 0; i < len; i++)
    {
        printf("%02x ", data[i]);
    }

    printf("\n");
}

int com_open(int com, int BaudRate, int ByteSize, int Parity, int StopBits, int reboot, HANDLE *handle)
{
    char name[16];
    snprintf(name, sizeof(name) - 1, "com%d", com);

    *handle = CreateFileA(name,                           // 串口名
                          GENERIC_READ | GENERIC_WRITE,   // 支持读写
                          0,                              // 独占方式，串口不支持共享
                          NULL,                           // 安全属性指针，默认值为NULL
                          OPEN_EXISTING,                  // 打开现有的串口文件
                          0,                              // 0：同步方式，FILE_FLAG_OVERLAPPED：异步方式
                          NULL);                          // 用于复制文件句柄，默认值为NULL，对串口而言该参数必须置为NULL

    if (INVALID_HANDLE_VALUE == *handle)
    {
        printf("COM%d open fail\n", com);
        return -1;
    }

    DCB p = { 0 };
    p.DCBlength = sizeof(p);
    p.BaudRate  = BaudRate;     // 波特率
    p.ByteSize  = ByteSize;     // 数据位
    p.Parity    = Parity;       // 无校验
    p.StopBits  = StopBits;     // 1位停止位

    if (!SetCommState(*handle, &p))
    {
        printf("COM%d set fail (%d %d %d %d)\n", com, BaudRate, ByteSize, Parity, StopBits);
        CloseHandle(*handle);
        return -2;
    }

    printf("COM%d open success (%d %d %d %d)\n", com, BaudRate, ByteSize, Parity, StopBits);

    if (!PurgeComm(*handle, PURGE_TXCLEAR | PURGE_RXCLEAR)) // 清空串口缓冲区
    {
        printf("COM%d PurgeComm fail\n", com);
        CloseHandle(*handle);
        return -3;
    }

    return 0;
}

void com_close(HANDLE handle)
{
    printf("close handle %x\n", (unsigned int)handle);
    CloseHandle(handle);
}

int com_reboot_to_loader(unsigned int com)
{
    if (com_open(com, 74880, 8, 0, 1, 1, &g_com) != 0)
    {
        printf("com_reboot_to_loader fail\n");
        return -1;
    }

    printf("set reboot to ROM Loader\n");

    EscapeCommFunction(g_com, SETRTS); // 重启1
    EscapeCommFunction(g_com, CLRRTS); // 重启2
    EscapeCommFunction(g_com, SETDTR); // 设置GPIO,ESP8266进入到串口下载模式
    Sleep(100);                         // 等待不能删除
    EscapeCommFunction(g_com, CLRDTR); // 清空
    EscapeCommFunction(g_com, CLRRTS); // 清空

    com_close(g_com);
    return 0;
}

int com_compress(const char *in, unsigned int in_len, unsigned char *out, unsigned int *out_len)
{
    if (mz_compress2(out, out_len, in, in_len, 9) != MZ_OK)
    {
        printf("mz_compress2 fail\n");
        return -1;
    }

    printf("compress data in_len:%d out_len:%d\n", in_len, *out_len);
    return 0;
}

int com_decompress(const char *in, unsigned int in_len, unsigned char *out, unsigned int *out_len)
{
    printf("base64 string len:%d\n", in_len);

    unsigned char *tmp     = (unsigned char*)malloc(BUFF_SIZE);
    unsigned int   tmp_len = BUFF_SIZE;

    if (base64_decode(in, in_len, tmp, &tmp_len) != 0)
    {
        free(tmp);
        printf("base64_decode fail\n");
        return -1;
    }

    printf("base64 data   len:%d\n", tmp_len);

    int flags = TINFL_FLAG_PARSE_ZLIB_HEADER; // 还有数据时 |= TINFL_FLAG_HAS_MORE_INPUT
    tinfl_decompressor inflator;
    tinfl_init(&inflator);

    if (tinfl_decompress(&inflator, tmp, &tmp_len, out, out, out_len, flags) != 0)
    {
        free(tmp);
        printf("tinfl_decompress fail\n");
        return -2;
    }

    free(tmp);
    printf("unzip  data   len:%d\n", *out_len);
    return 0;
}

int com_send(HANDLE handle, unsigned char *buf, unsigned int len)
{
    unsigned int        tmp;
    unsigned int        num     = 1;
    unsigned int        pos     = 0;
    unsigned char       out[5]  = { 0xc0, 0xdb, 0xdc, 0xdb, 0xdd };
    p_esp_loader_req    req     = (p_esp_loader_req)buf;

    printf("send magic:%d op:%x data_len:%d checksum:0x%08x\n", req->head.zero, req->head.op, req->head.data_len, req->head.checksum);

    com_printf_data(buf, (req->head.op == 0x03 || req->head.op == 0x07 || req->head.op == 0x11) ? 16 : req->head.data_len);

    if (!WriteFile(handle, out, 1, &tmp, NULL)) // SLIP协议固定头0xc0
    {
        printf("com:%d send head(0xc0) %d %d\n", (int)handle, errno, GetLastError());
        return -1;
    }

    for (unsigned int i = 0; i < len; i++)
    {
        if (0xc0 == buf[i])
        {
            if (!WriteFile(handle, buf + pos, i - pos, &tmp, NULL))
            {
                printf("com:%d send data2 %d %d\n", (int)handle, errno, GetLastError());
                return -2;
            }

            if (!WriteFile(handle, out + 1, 2, &tmp, NULL))
            {
                printf("com:%d send data3 %d %d\n", (int)handle, errno, GetLastError());
                return -3;
            }

            num += tmp;
            pos = i + 1;
        }
        else if (0xdb == buf[i])
        {
            if (!WriteFile(handle, buf + pos, i - pos, &tmp, NULL))
            {
                printf("com:%d send data4 %d %d\n", (int)handle, errno, GetLastError());
                return -4;
            }

            if (!WriteFile(handle, out + 3, 2, &tmp, NULL))
            {
                printf("com:%d send data5 %d %d\n", (int)handle, errno, GetLastError());
                return -5;
            }

            num += tmp;
            pos = i + 1;
        }
    }

    if (pos < len)
    {
        if (!WriteFile(handle, buf + pos, len - pos, &tmp, NULL))
        {
            printf("com:%d send data %d6 %d\n", (int)handle, errno, GetLastError());
            return -6;
        }
    }

    num += tmp;

    if (!WriteFile(handle, out, 1, &tmp, NULL))  // SLIP协议固定尾0xc0
    {
        printf("com:%d send end(0xc0) %d %d\n", (int)handle, errno, GetLastError());
        return -7;
    }

    num += tmp;

    printf("send data %d %d\n", len, num);
    return len;
}

int com_recv(HANDLE handle, unsigned char *buf, unsigned int max_len)
{
    unsigned int one;
    unsigned int len;

    for (unsigned int i = 0; i < max_len; i++)
    {
        if (!ReadFile(handle, &buf[i], 1, &one, NULL))
        {
            printf("COM:%d recv pack head(0x0c) fail %d %d\n", (int)handle, errno, GetLastError());
            return -1;
        }

        if (0 == i && 0xc0 != buf[0])
        {
            printf("COM:%d recv pack head is not 0xc0 0x%2x %d %d\n", (int)handle, buf[0], errno, GetLastError());
            return -2;
        }

        if (0 != i && 0xc0 == buf[i])
        {
            len = i + 1;
            break;
        }
    }

    com_printf_data(buf, len);

    unsigned int   j   = 0;
    unsigned char *out = buf;

    for (unsigned int i = 0; i < len; i++)
    {
        if (0xdb == buf[i] && 0xdc == buf[i + 1])
        {
            i++;
            out[j++] = 0xc0;
        }
        else if (0xdb == buf[i] && 0xdd == buf[i + 1])
        {
            i++;
            out[j++] = 0xdb;
        }
        else
        {
            out[j++] = buf[i];
        }
    }

    int ret = 0;
    p_esp_loader_rsp rsp = (p_esp_loader_rsp)(buf + 1);

    if (0x48 != rsp->op)
    {
        ret = rsp->data[rsp->data_len - 1];
        printf("recv data %d %d\n", len, j);
        printf("magic:%d op:%x data_len:%d value:0x%08x ret:%d\n", rsp->one, rsp->op, rsp->data_len, rsp->value, ret);
    }
    else
    {
        printf("recv data OHAI !!!\n");
    }

    printf("--------------------------------------------------\n");
    return ret;
}

int com_send_recv(HANDLE handle, unsigned char *buf, unsigned int max_len, const char *info)
{
    printf("send %s\n", info);

    p_esp_loader_req req = (p_esp_loader_req)buf;

    if (com_send(handle, buf, sizeof(req->head) + req->head.data_len) <= 0)
    {
        return -1;
    }

    printf("recv wait\n");

    return com_recv(g_com, buf, max_len);
}

int com_sync(HANDLE handle, unsigned char *buf, unsigned int max_len)
{
    // 0x08|同步|0x07,0x07,0x12,0x20,0x55*32
    p_esp_loader_req req = (p_esp_loader_req)buf;
    req->head.zero       = 0;
    req->head.op         = 0x08;
    req->head.data_len   = 36;
    req->head.checksum   = 0;
    req->data[0]         = 0x20120707;
    memset(req->data + 1, 0x55, 32);

    printf("send sync1\n");

    if (com_send(handle, buf, sizeof(req->head) + req->head.data_len) <= 0) // 发送2个同步命令,收到8个应答,只发1个不应答
    {
        return -1;
    }

    printf("send sync2\n");

    if (com_send(handle, buf, sizeof(req->head) + req->head.data_len) <= 0)
    {
        return -2;
    }

    for (int i = 0; i < 8; i++)
    {
        printf("recv sync %d\n", i);

        if (com_recv(g_com, buf, max_len) != 0)
        {
            return -3;
        }
    }

    return 0;
}

int com_read_mem(HANDLE handle, unsigned char *buf, unsigned int max_len)
{
    // 读取内存|地址
    p_esp_loader_req req = (p_esp_loader_req)buf;
    req->head.zero       = 0;
    req->head.op         = 0x0a;
    req->head.data_len   = 4;
    req->head.checksum   = 0;
    req->data[0]         = 0x3ff0005c;

    return com_send_recv(handle, buf, max_len, "read mem");
}

int com_set_flash_param(HANDLE handle, unsigned char *buf, unsigned int max_len)
{
    // 0x0b|设置FLASH参数|id,总大小,块大小,扇区大小,页大小,掩码
    p_esp_loader_req req = (p_esp_loader_req)buf;
    req->head.zero       = 0;
    req->head.op         = 0x0b;
    req->head.data_len   = 24;
    req->head.checksum   = 0;
    req->data[0]         = 0;           //      ID
    req->data[1]         = 0x00400000;  // 4M   总大小
    req->data[2]         = 0x00010000;  // 64K  块大小
    req->data[3]         = 0x00001000;  // 4K   扇区大小
    req->data[4]         = 0x00000100;  // 256B 页大小
    req->data[5]         = 0x0000ffff;  //      掩码

    return com_send_recv(handle, buf, max_len, "set flash param");
}

int com_upload_rom(HANDLE handle, unsigned char *buf, unsigned int max_len, unsigned char *data, unsigned int data_len, unsigned int addr, int reboot)
{
    unsigned int   in_len = BUFF_SIZE;
    unsigned char *in = (unsigned char*)malloc(BUFF_SIZE);

    if (com_compress(data, data_len, in, &in_len) != 0) return -1;

    unsigned int size = 0x4000;
    unsigned int num = (in_len + size - 1) / size;

    // 0x10|向Flash写数据开始|未压缩大小,包数量,包大小,地址
    p_esp_loader_req req = (p_esp_loader_req)buf;
    req->head.zero       = 0;
    req->head.op         = 0x10;
    req->head.data_len   = 16;
    req->head.checksum   = 0;
    req->data[0]         = data_len;
    req->data[1]         = num;
    req->data[2]         = size;
    req->data[3]         = addr;

    if (com_send_recv(handle, buf, max_len, "write rom begin") != 0) return -2;

    for (unsigned int i = 0; i < num; i++)
    {
        int len = ((i != (num - 1)) ? size : (in_len % size));

        // 0x11|向Flash写数据|数据大小,序列号,0x00,0x00
        req->head.zero       = 0;
        req->head.op         = 0x11;
        req->head.data_len   = len + 16;
        req->head.checksum   = com_checksum(in + size * i, len, 0xef);
        req->data[0]         = len;
        req->data[1]         = i;
        req->data[2]         = 0;
        req->data[3]         = 0;
        memcpy(req->data + 4, in + size * i, len);

        if (com_send_recv(handle, buf, max_len, "write rom data") != 0) return -3;
    }

    // 0x13|Flash数据MD5|地址,大小,0x00,0x00
    req->head.zero       = 0;
    req->head.op         = 0x13;
    req->head.data_len   = 16;
    req->head.checksum   = 0;
    req->data[0]         = addr;
    req->data[1]         = data_len;
    req->data[2]         = 0;
    req->data[3]         = 0;

    if (com_send_recv(handle, buf, max_len, "write rom md5") != 0) return -4;

    unsigned char md5_data[16];
    md5(data, data_len, md5_data);

    p_esp_loader_rsp rsp = (p_esp_loader_rsp)(buf + 1);

    printf("md5 check %s\n", (memcmp(rsp->data, md5_data, sizeof(md5_data)) == 0) ? "ok" : "fail!!!");

    if (!reboot) return 0;

    printf("set esp8266 reboot\n");
    EscapeCommFunction(handle, SETRTS); // 重启1
    EscapeCommFunction(handle, CLRRTS); // 重启2
    return 0;
}

int com_upload_ram(HANDLE handle, unsigned char *buf, unsigned int max_len, unsigned char *data, unsigned int data_len, unsigned int addr, unsigned int entry)
{
    unsigned int size = 0x1800;
    unsigned int num = (data_len + size - 1) / size;

    // 内存写数据开始|总大小,包数量,包大小,地址
    p_esp_loader_req req = (p_esp_loader_req)buf;
    req->head.zero       = 0;
    req->head.op         = 0x05;
    req->head.data_len   = 16;
    req->head.checksum   = 0;
    req->data[0]         = data_len;
    req->data[1]         = num;
    req->data[2]         = size;
    req->data[3]         = addr;

    if (com_send_recv(handle, buf, max_len, "write ram begin") != 0) return -3;

    for (unsigned int i = 0; i < num; i++)
    {
        int len = ((i != (num - 1)) ? size : (data_len % size));

        // 向内存写数据|数据大小,序列号,0x00,0x00
        req->head.zero       = 0;
        req->head.op         = 0x07;
        req->head.data_len   = len + 16;
        req->head.checksum   = com_checksum(data + size * i, len, 0xef);
        req->data[0]         = len;
        req->data[1]         = i;
        req->data[2]         = 0;
        req->data[3]         = 0;
        memcpy(req->data + 4, data + size * i, len);

        if (com_send_recv(handle, buf, max_len, "write ram data") != 0) return -4;
    }

    if (!entry) return 0;

    // 0x06|向内存写数据结束|执行标记,入口地址
    req->head.zero       = 0;
    req->head.op         = 0x06;
    req->head.data_len   = 8;
    req->head.checksum   = 0;
    req->data[0]         = 0;
    req->data[1]         = entry;

    if (com_send_recv(handle, buf, max_len, "write ram end") != 0) return -7;

    printf("recv OHAI\n");

    return com_recv(g_com, buf, max_len);
}

int com_upload_stub(HANDLE handle, unsigned char *buf, unsigned int max_len)
{
    unsigned char   *sub_text   = (unsigned char*)malloc(BUFF_SIZE);
    unsigned char   *sub_data   = (unsigned char*)malloc(BUFF_SIZE);
    unsigned int    text_len    = BUFF_SIZE;
    unsigned int    data_len    = BUFF_SIZE;
    int             ret;

    do
    {
        if (com_decompress(STUB_TEXT, sizeof(STUB_TEXT), sub_text, &text_len) != 0)
        {
            printf("com_decompress sub_text fail\n");
            ret = -1;
            break;
        }

        if (com_decompress(STUB_DATA, sizeof(STUB_DATA), sub_data, &data_len) != 0)
        {
            printf("com_decompress sub_data fail\n");
            ret = -2;
            break;
        }

        if (com_upload_ram(handle, buf, max_len, sub_text, text_len, STUB_TEXT_ADDR, 0) != 0)
        {
            printf("com_upload_ram sub_text fail\n");
            ret = -3;
            break;
        }

        if (com_upload_ram(handle, buf, max_len, sub_data, data_len, STUB_DATA_ADDR, STUB_ENTRY_ADDR) != 0)
        {
            printf("com_upload_ram sub_data fail\n");
            ret = -4;
            break;
        }
    }
    while(0);

    free(sub_text);
    free(sub_data);
    return 0;
}

/**
 * \brief   打印信息
 * \param   [in]  无
 * \return        无
 */
void printf_info()
{
    //printf("esp_app.exe ota -size=0x1000 -o=output.bin\n");
    //printf("esp_app.exe csv -i=input.csv -o=output.bin\n");
    //printf("esp_app.exe bin -i=input.elf -o=output.bin -model=12F -version={1|3}\n");
    //printf("esp_app.exe rom -i=input.bin -addr=0x10000 -com=1\n");
    //printf("esp_app.exe ram -i=input.bin -com=1\n");

    printf("esp_app.exe ota -o=3_otadata.bin -size=0x2000\n");
    printf("esp_app.exe csv -o=2_partitions.bin -i=..\\conf\\partitions_two_ota.csv \n");
    printf("esp_app.exe bin -o=1_bootloader.bin -i=..\\conf\\bootloader.elf -version=1\n");
    printf("esp_app.exe bin -o=4_app.bin -i=..\\conf\\esp_app.elf -version=3\n");
    printf("esp_app.exe rom -i=1_bootloader.bin -addr=0x0000 -com=5\n");
    printf("esp_app.exe rom -i=2_partitions.bin -addr=0x8000 -com=5\n");
    printf("esp_app.exe rom -i=3_otadata.bin -addr=0xd000 -com=5\n");
    printf("esp_app.exe rom -i=4_app.bin -addr=0x10000 -com=5\n");
    printf("esp_app.exe ram -i=4_app.bin -com=5\n");
}

/**
 * \brief   检测主函数外传入的参数
 * \param   [in]  int               argc            参数数量
 * \param   [in]  char**            argv            参数
 * \return        int                               0:成功,其它失败
 */
int check_args(int argc, char **argv, p_arg arg)
{
    const char *com;
    const char *addr;
    const char *size;
    const char *input;
    const char *output;
    const char *version;

    if (strcmp(argv[1], "ota") == 0 && NULL != argv[2] && NULL != argv[3] && (output = strstr(argv[2], "-o=")) && (size = strstr(argv[3], "-size=0x")))
    {
        arg->type   = OTA;
        arg->size   = strtol(size + 8, NULL, 16);
        arg->output = output + 3;

        printf("output:%s\nsize:0x%x\n", arg->output, arg->size);
    }
    else if (strcmp(argv[1], "csv") == 0 && NULL != argv[2] && NULL != argv[3] && (output = strstr(argv[2], "-o=")) && (input = strstr(argv[3], "-i=")))
    {
        arg->type   = CSV;
        arg->input  = input + 3;
        arg->output = output + 3;

        printf("output:%s\ninput:%s\n", arg->output, arg->input);
    }
    else if (strcmp(argv[1], "bin") == 0 && NULL != argv[2] && NULL != argv[3] && NULL != argv[4] && (output = strstr(argv[2], "-o=")) && (input = strstr(argv[3], "-i=")) && (version = strstr(argv[4], "-version=")))
    {
        arg->type    = BIN;
        arg->input   = input + 3;
        arg->output  = output + 3;
        arg->version = atoi(version + 9);

        printf("output:%s\ninput:%s\nversion:%d\n", arg->output, arg->input, arg->version);
    }
    else if (strcmp(argv[1], "rom") == 0 && NULL != argv[2] && NULL != argv[3] && NULL != argv[4] &&
            (input = strstr(argv[2], "-i=")) && (addr = strstr(argv[3], "-addr=0x")) && (com = strstr(argv[4], "-com=")))
    {
        arg->type  = ROM;
        arg->input = input + 3;
        arg->addr  = strtol(addr + 8, NULL, 16);
        arg->com   = atoi(com + 5);

        printf("input:%s\naddr=0x%x\ncom:%d\n", arg->input, arg->addr, arg->com);
    }
    else if (strcmp(argv[1], "ram") == 0 && NULL != argv[2] && NULL != argv[3] && (input = strstr(argv[2], "-i=")) && (com = strstr(argv[3], "-com=")))
    {
        arg->type  = RAM;
        arg->input = input + 3;
        arg->com   = atoi(com + 5);

        printf("input:%s\ncom:%d\n", arg->input, arg->com);
    }
    else
    {
        return -1;
    }

    return 0;
}

/**
 * \brief   生成OTA数据
 * \param   [in]  unsigned int      size            大小
 * \param   [in]  const char *      output          输出文件路径
 * \return        int                               0:成功,其它失败
 */
int process_ota(unsigned int size, const char *output)
{
    unsigned char *in = (unsigned char *)malloc(size);
    memset(in, 0xFF, size);

    if (put_file_data(output, in, size) != 0)
    {
        free(in);
        return -1;
    }

    free(in);
    printf("%s size:0x%x ok\n", output, size);
    return 0;
}

/**
 * \brief   生成分区表数据
 *
 * 分区表文件:
 * # Name,   Type, SubType, Offset,   Size, Flags
 * # Note: if you change the phy_init or app partition offset, make sure to change the offset in Kconfig.projbuild
 * nvs,      data, nvs,     0x9000,  0x4000
 * otadata,  data, ota,     0xd000,  0x2000
 * phy_init, data, phy,     0xf000,  0x1000
 * ota_0,    0,    ota_0,   0x10000, 0xF0000
 * ota_1,    0,    ota_1,   0x110000,0xF0000
 *
 * Type字段
 * 可以指定为 app (0) 或 data (1)，也可以直接使用数字 0-254（或十六进制 0x00-0xFE）。注意，0x00-0x3F 不得使用（预留给 ESP8266_RTOS_SDK 的核心功能）
 * 如果应用程序需要保存数据，则需要在 0x40-0xFE 内添加一个自定义分区类型
 * 注意，bootloader 将忽略 app (0) 和 data (1) 以外的其他分区类型
 *
 * App Subtypes
 * 当 Type 定义为 app 时，SubType 字段可以指定为 factory(0x00), ota_0 (0x10) ~ ota_15 (0x1F), test (0x20)
 * ota_0 (0x10) 是默认的 app 分区。bootloader 将默认加载 ota_0，但如果存在类型为 data/ota 的分区，则 bootloader 将加载 data/ota 分区中的数据，进而判断启动哪个 OTA 镜像文件
 * ota_0 (0x10) ~ ota_15 (0x1F) 为 OTA 应用程序分区。在使用 OTA 功能时，应用程序应至少拥有 2 个 OTA 应用程序分区(ota_0 和 ota_1)
 *
 * Data Subtypes
 * 当 Type 定义为 data 时，SubType 字段可以指定为 ota(0x00)，phy(0x01)，nvs(0x02), coredump(0x03), nvs_keys(0x04), efuse(0x05), esphttpd(0x80), fat(0x81), spiffs(0x82)
 * ota (0) 即 OTA 数据分区 ，用于存储当前所选的 OTA 应用程序的信息。这个分区的大小应设定为 0x2000 (8 KB)
 * phy (1) 分区用于存放 PHY 初始化数据，从而保证可以为每个设备单独配置 PHY，而非必须采用固件中的统一 PHY 初始化数据
 *         默认配置下，phy 分区并不启用，而是直接将 phy 初始化数据编译至应用程序中，从而节省分区表空间(可直接将此分区删掉)
 *         如果需要从此分区加载 phy 初始化数据，make menuconfig 并使能 CONFIG_ESP32_PHY_INIT_DATA_IN_PARTITION 选项。此时，
 *         还需要手动将 phy 初始化数据烧至设备 flash（ESP8266_RTOS_SDK 编译系统并不会自动完成此操作）
 * nvs (2) 是专门给非易失性存储 (NVS) API 使用的分区：
 *         NVS 用于存储每台设备的 PHY 校准数据(不是 PHY 初始化数据)
 *         NVS 用于存储 Wi-Fi 数据(如果使用了 esp_wifi_set_storage(WIFI_STORAGE_FLASH) 初始化函数)
 *         NVS API 还可用于其他应用程序数据；
 *         强烈建议在项目中包含一个至少 0x3000 (12 KB) 的 NVS 分区；
 *         如果使用 NVS API 存储大量数据，则需增加 NVS 分区的大小, 默认为 0x6000 (24 KB)
 *
 * Offset 和 Size 字段
 * app 分区必须位于一个完整的 1 MB 分区内。否则，应用程序将崩溃
 *
 * 分区表数据:
 * 每条信息32字字=AA 50(2字节) + Type(1字节) + SubType(1字节) + Offset(4字节) + Length(4字节) + Usage(20字节)
 * 最多96条3KB
 *
 * \param   [in]  const char *      input           输入文件路径
 * \param   [in]  const char *      output          输出文件路径
 * \return        int                               0:成功,其它失败
 */
int process_csv(const char *input, const char *output)
{
    int  line_num;
    char line[96][1024];
    FILE *fp;

    if (fopen_s(&fp, input, "rb") != 0)
    {
        printf("open %s error\n", input);
        return -1;
    }

    for(line_num = 0; line_num < 96 && fgets(line[line_num], sizeof(line[0]) - 1, fp) != NULL; line_num++);
    fclose(fp);

    char  *id;
    char  sub[64];
    char  type[64];
    int   len     = 0;
    int   num     = 0;
    int   end     = 0;
    int   align[] = { 0x1000, 0x04 }; // app, data
    t_cvs *ptr    = NULL;
    t_cvs cvs[96];

    memset(&cvs, 0xFF, sizeof(cvs));

    for (int i = 0; i < line_num; i++)
    {
        if (sscanf_s(line[i], "%s", type, sizeof(type) - 1) == 1 && '#' == type[0]) continue;// 注释

        ptr = &(cvs[num++]);

        if (sscanf_s(line[i], "%[^,], %[^,], %[^,], 0x%x, 0x%x", ptr->name, sizeof(ptr->name) - 1, type, sizeof(type) - 1, sub, sizeof(sub) - 1, &(ptr->offset), &(ptr->size)) != 5)
        {
            printf("error line:%s\n", line[i]);
            return -2;
        }

        ptr->magic = 0x50AA;
        ptr->flags = 0;

        len = strlen(ptr->name);
        memset(ptr->name + len, 0, sizeof(ptr->name) - len);

        id = strstr(CVS_TYPE_ID, type);
        ptr->type =  (unsigned char)atoi((NULL == id) ? type : (id + strlen(type) + 1));

        id = strstr(CVS_SUBTYPE_ID, sub);
        ptr->type_sub = (NULL == id) ? 0 : (unsigned char)strtol(id + strlen(sub) + 1, NULL, 16);

        if (ptr->offset % align[ptr->type] || ptr->size % align[ptr->type])
        {
            printf("name:%s (offset,size) align:0x%x\n", ptr->name, align[ptr->type]);
            return -3;
        }

        if (ptr->offset <= (unsigned int)end)
        {
            printf("name:%s offset:0x%x <= last end:0x%x\n", ptr->name, ptr->offset, end);
            return -4;
        }

        end = ptr->offset + ptr->size - 1;

        if (APP == ptr->type && (ptr->offset / 0x100000) != (end / 0x100000)) // app分区必须在1M区间,不能跨越
        {
            printf("name:%s app must in 1M and do not span\n", ptr->name);
            return -5;
        }

        printf("name: %19s type: %x sub: %2x offset: %8x size: %x\n", ptr->name, ptr->type, ptr->type_sub, ptr->offset, ptr->size);
    }

    printf("%s num:%d ok\n", output, num);

    return put_file_data(output, &cvs, sizeof(cvs));
}

/**
 * \brief   elf->bin
 * \param   [in]  const char *      input           输入文件路径
 * \param   [in]  const char *      output          输出文件路径
 * \param   [in]  unsigned int      modle           ESP型号
 * \param   [in]  unsigned int      versio          BIN版本
 * \return        int                               0:成功,其它失败
 */
int process_bin(const char *input, const char *output, unsigned int version)
{
    unsigned int    in_len = BUFF_SIZE;
    unsigned char*  in     = (unsigned char*)malloc(BUFF_SIZE);

    if (get_file_data(input, in, &in_len) != 0)
    {
        free(in);
        return -1;
    }

    p_Elf32_Ehdr elf = (p_Elf32_Ehdr)in;
    printf("e_entry:     \t0x%x\n", elf->e_entry);
    printf("e_shoff:     \t0x%x\n", elf->e_shoff);
    printf("e_shentsize: \t0x%x\n", elf->e_shentsize);
    printf("e_shnum:     \t0x%x\n", elf->e_shnum);
    printf("e_shstrndx:  \t0x%x\n", elf->e_shstrndx);

    unsigned int ro;
    unsigned int id;
    unsigned int len;
    unsigned int pad;
    unsigned int num[2] = { 0 };
    p_sec        ptr;
    t_sec        sec[2][512];
    p_Elf32_Shdr section = (p_Elf32_Shdr)(in + elf->e_shoff);
    const char  *name = in + section[elf->e_shstrndx].sh_offset;

    for (int i = 0; i < elf->e_shnum; i++, section++)
    {
        if (1 == section->sh_type  && 0 != section->sh_addr && 0 != section->sh_size)
        {
            id = (strncmp(name + section->sh_name, ".flash.", 7) != 0);
            ro = (strcmp(name + section->sh_name, ".flash.rodata") == 0 ? 8 : 0);

            ptr = &(sec[id][num[id]]);
            ptr->offset   = section->sh_offset + ro;
            ptr->head.addr = section->sh_addr;
            ptr->head.size = section->sh_size - ro;

            len = ptr->head.size;
            pad = (len % 4 == 0) ? 0 : (4 - len % 4); // 4字节对齐,需要补齐
            ptr->head.size += pad;
            memset(in + ptr->offset + len, 0, pad);

            printf("sec[%d][%d]name:\t%s\n",        id, num[id], name + section->sh_name);
            printf("sec[%d][%d]offset:0x%x\n",      id, num[id], section->sh_offset);
            printf("sec[%d][%d]addr:\t0x%x\n",      id, num[id], section->sh_addr);
            printf("sec[%d][%d]size:\t0x%x\n",      id, num[id], section->sh_size);
            printf("sec[%d][%d]pad:\t%d\n",         id, num[id], pad);

            num[id]++;
        }
    }

    if (3 == version)
    {
        t_sec t;

        // 排序
        for (int i = 0; i < 2; i++)
        {
            for (unsigned int j = 0; j < num[i] - 1; j++)
            {
                for (unsigned int k = 1; k < num[i]; k++)
                {
                    if (sec[i][k].head.addr < sec[i][j].head.addr)
                    {
                        t = sec[i][j];
                        sec[i][j] = sec[i][k];
                        sec[i][k] = t;
                    }
                }
            }
        }
    }

    unsigned char *out = (unsigned char*)malloc(BUFF_SIZE);
    unsigned char checksum = 0xef; // 计算xor校验码,只校验段数据部分,最后补齐16字节对齐,为了放校验码最少有1位,最多16位

    // 文件头
    t_bin head = { 0xE9, num[0] + num[1], 0x02, (3 == version) ? 0x20 : 0x40, elf->e_entry};
    len = sizeof(head);
    memcpy(out, &head, len);

    for (int i = 0; i < 2; i++)
    {
        for (unsigned int j = 0; j < num[i]; j++)
        {
            ptr = &(sec[i][j]);
            checksum = com_checksum(in + ptr->offset, ptr->head.size, checksum);

            memcpy(&out[len], &(ptr->head), sizeof(ptr->head));
            len += sizeof(ptr->head);

            memcpy(&out[len], in + ptr->offset, ptr->head.size);
            len += ptr->head.size;

            printf("checksum[%d][%d]:\t0x%2x\n", i, j, checksum);
        }
    }

    pad = (len % 16 == 0) ? 16 : (16 - len % 16); // 16字节对齐
    memset(out + len, 0, pad - 1);
    memset(out + len + pad - 1, checksum, 1);
    len += pad;

    printf("checksum:\t0x%2x pad: %d\n", checksum, pad);

    // SHA256校验全部数据,包括xor校验码
    esp_sha256_t sha256;
    esp_sha256_init(&sha256);
    esp_sha256_update(&sha256, out, len);
    esp_sha256_finish(&sha256);

    for (int i = 0; i < 8; i++)
    {
        id = sha256.state[i];
        sha256.state[i] = ((id & 0xFF000000) >> 24) | ((id & 0x00FF0000) >> 8) | ((id & 0x0000FF00)) << 8 | ((id & 0x000000FF) << 24);
        printf("sha256:\t\t%08x\n", sha256.state[i]);
    }

    if (3 == version)
    {
        memcpy(out + len, &(sha256.state), sizeof(sha256.state));
        len += sizeof(sha256.state);
    }

    put_file_data(output, out, len);
    free(out);
    free(in);
    return 0;
}

/**
 * \brief   刷新固件
 * \param   [in]  const char *      input           输入文件路径
 * \param   [in]  unsigned int      addr            写入地址
 * \param   [in]  unsigned int      com             串口号
 * \return        int                               0:成功,其它失败
 */
int process_rom(const char *input, unsigned int addr, unsigned int com)
{
    if (com_reboot_to_loader(com) != 0) return -1;  // 模块重启进入串口下载模式

    unsigned char*  in      = (unsigned char*)malloc(BUFF_SIZE);
    unsigned char*  out     = (unsigned char*)malloc(BUFF_SIZE);
    int             ret     = BUFF_SIZE;

    if (get_file_data(input, in, &ret) != 0) return -2;

    do
    {
        if (com_open(com, 115200, 8, 0, 1, 1, &g_com) != 0)
        {
            ret = -3;
            break;
        }

        if (com_sync(g_com, out, BUFF_SIZE) != 0)
        {
            ret = -4;
            break;
        }

        if (com_upload_stub(g_com, out, BUFF_SIZE) != 0)
        {
            ret = -5;
            break;
        }

        if (com_upload_rom(g_com, out, BUFF_SIZE, in, ret, addr, 1) != 0)
        {
            ret = -6;
            break;
        }

    } while (0);

    free(in);
    free(out);
    CloseHandle(g_com);
    return ret;
}

/**
 * \brief   向内存写入数据
 * \param   [in]  const char *      input           输入文件路径
 * \param   [in]  unsigned int      addr            写入地址
 * \param   [in]  unsigned int      com             串口号
 * \return        int                               0:成功,其它失败
 */
int process_ram(const char *input, unsigned int com)
{
    if (com_reboot_to_loader(com) != 0) return -1; // 模块重启进入串口下载模式

    unsigned char*  in      = (unsigned char*)malloc(BUFF_SIZE);
    unsigned char*  out     = (unsigned char*)malloc(BUFF_SIZE);
    int             ret     = BUFF_SIZE;
    p_bin           bin     = (p_bin)in;

    if (get_file_data(input, in, &ret) != 0) return -2;

    do
    {
        if (com_open(com, 115200, 8, 0, 1, 1, &g_com) != 0)
        {
            ret = -3;
            break;
        }

        if (com_sync(g_com, out, BUFF_SIZE) != 0)
        {
            ret = -4;
            break;
        }

        printf("---------%s\n bin magic:%x sec_num:%x flash_mod:%x flash_sf:%x entry:%x",
                input, bin->magic, bin->sec_num, bin->flash_mod, bin->flash_sf, bin->entry);

        p_sec_head sec;
        unsigned int offset = sizeof(t_bin);

        for (int i = 0; i < bin->sec_num; i++)
        {
            sec = (p_sec_head)(in + offset);

            printf("%i offset:%x addr:%x size:%x\n", i, offset, sec->addr, sec->size);

            offset += sizeof(t_sec_head);

            if (com_upload_ram(g_com, out, BUFF_SIZE, in + offset, sec->size, sec->addr, ((i != (bin->sec_num - 1)) ? 0 : bin->entry)) != 0)
            {
                ret = -5;
                break;
            }

            offset += sec->size;
        }

    } while (0);

    free(in);
    free(out);
    CloseHandle(g_com);
    return ret;
}

/**
 * \brief   主函数
 * \param   [in]  int               argc            参数数量
 * \param   [in]  char**            argv            参数
 * \return        int                               0-成功，其它失败
 */
int main(int argc, char **argv)
{
    if (argc <= 1 || check_args(argc, argv, &g_arg) != 0)
    {
        printf_info();
        return -1;
    }

    switch (g_arg.type)
    {
        case OTA:
        {
            return process_ota(g_arg.size, g_arg.output);
        }
        case CSV:
        {
            return process_csv(g_arg.input, g_arg.output);
        }
        case BIN:
        {
            return process_bin(g_arg.input, g_arg.output, g_arg.version);
        }
        case ROM:
        {
            return process_rom(g_arg.input, g_arg.addr, g_arg.com);
        }
        case RAM:
        {
            return process_ram(g_arg.input, g_arg.com);
        }
        default:
        {
            printf("error %d\n", g_arg.type);
            break;
        }
    }

    return -2;
}