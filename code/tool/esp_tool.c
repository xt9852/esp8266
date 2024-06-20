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

#define CVS_TYPE_ID    "app-0,data-1"
#define CVS_SUBTYPE_ID "factory-0,test-20,ota-00,phy-01,nvs-02,coredump-03,nvs_keys-04,efuse-05,ota_0-10,ota_1-11,ota_2-12,ota_3-13,ota_4-14,ota_5-15,ota_6-16,ota_7-17,ota_8-18,ota_9-19,ota_10-1a,ota_11-1b,ota_12-1c,ota_13-1d,ota_14-1e,ota_15-1f,esphttpd-80,fat-81,spiffs-82"

enum
{
    APP = 0,
    OTA,                        // 生成OTA数据
    CVS,                        // 生成分区表数据
    BIN,                        // 生成BIN数据
    ROM,                        // 烧录固件
    RAM,                        // 向内存写入数据
    F12,                        // ESP8266型号:12F
};

typedef struct
{
    unsigned int    type;       // 类型:OTA,CVS,BIN,FLASH
    unsigned int    com;        // 串口号
    unsigned int    size;       // 数据的长度
    unsigned int    addr;       // 数据的地址
    unsigned int    model;      // 型号:12F
    unsigned int    version;    // 版本:1,3
    const char     *input;      // 输入文件
    const char     *output;     // 输出文件

} t_arg, *p_arg;

typedef struct
{
    unsigned short magic;       // 0x50AA
    unsigned char  type;        // app-0,data-1
    unsigned char  type_sub;    // factory-0x00,test-0x20,ota-0x00, phy-0x01, nvs-0x02, coredump-0x03, nvs_keys-0x04, efuse-0x05, esphttpd-0x80, fat-0x81, spiffs-0x82
    unsigned int   offset;      // 位置
    unsigned int   size;        // 大小
    char           name[16];    // 名称
    unsigned int   flags;       // 标记

} t_cvs, *p_cvs;

typedef struct
{
    unsigned char   magic;      // 固定0xE9
    unsigned char   sec_num;    // 段数量
    unsigned char   flash_mod;  // FLASH模式          QIO,QOUT,DIO,DOUT,FAST_READ,SLOW_READ
    unsigned char   flash_sf;   // FLASH大小和频率    大小:1MB,2MB,4MB,8MB,16MB 频率40M,26M,20M,80M
    unsigned int    entry;      // 入口地址

} t_bin, *p_bin;

typedef struct
{
    unsigned int    addr;       // 段内存地址
    unsigned int    size;       // 段大小

} t_sec, *p_sec;

typedef struct
{
    t_sec           sec;        // 段头
    unsigned int    offset;     // 段数据在文件中的位置
    unsigned int    pad;        // 4字节对齐

} t_sec_data, *p_sec_data;

typedef struct
{
	unsigned char  e_ident[16]; // 0x7F+ELF
	unsigned short e_type;      // 该文件的类型
	unsigned short e_machine;   // 体系结构0x5e
	unsigned int   e_version;   // 文件的版本
	unsigned int   e_entry;     // 入口地址
	unsigned int   e_phoff;     // Program header table 在文件中的偏移量
	unsigned int   e_shoff;     // Section header table 在文件中的偏移量
	unsigned int   e_flags;     // IA32而言，此项为0
	unsigned short e_ehsize;    // ELF header大小
	unsigned short e_phentsize; // Program header table中每一个条目的大小
	unsigned short e_phnum;     // Program header table中有多少个条目
	unsigned short e_shentsize; // Section header table中的每一个条目的大小
	unsigned short e_shnum;     // Section header table中有多少个条目
	unsigned short e_shstrndx;  // 包含节名称的字符串是第几个节
} Elf32_Ehdr, *p_Elf32_Ehdr;

typedef struct
{
    unsigned int   sh_name;     // Section name (string tbl index)
    unsigned int   sh_type;     // Section type
    unsigned int   sh_flags;    // Section flags
    unsigned int   sh_addr;     // Section virtual addr at execution
    unsigned int   sh_offset;   // Section file offset
    unsigned int   sh_size;     // Section size in bytes
    unsigned int   sh_link;     // Link to another section
    unsigned int   sh_info;     // Additional section information
    unsigned int   sh_addralign;// Section alignment
    unsigned int   sh_entsize;  // Entry size if section holds table
} Elf32_Shdr, *p_Elf32_Shdr;

static t_arg g_arg = { 0 };

#define Ch(x,y,z)       (z ^ (x & (y ^ z)))
#define Maj(x,y,z)      (((x | y) & z) | (x & y))
#define RORc(x, y)      (((((unsigned int) (x) & 0xFFFFFFFFUL) >> (unsigned int) ((y) & 31)) | ((unsigned int) (x) << (unsigned int) (32 - ((y) & 31)))) & 0xFFFFFFFFUL)
#define S(x, n)         RORc((x), (n))
#define R(x, n)         (((x)&0xFFFFFFFFUL)>>(n))
#define Sigma0(x)       (S(x, 2) ^ S(x, 13) ^ S(x, 22))
#define Sigma1(x)       (S(x, 6) ^ S(x, 11) ^ S(x, 25))
#define Gamma0(x)       (S(x, 7) ^ S(x, 18) ^ R(x, 3))
#define Gamma1(x)       (S(x, 17) ^ S(x, 19) ^ R(x, 10))

#define RND(a,b,c,d,e,f,g,h,i) t0 = h + Sigma1(e) + Ch(e, f, g) + K[i] + W[i]; t1 = Sigma0(a) + Maj(a, b, c);  d += t0; h  = t0 + t1;

#define ESP_GET_BE32(a) ((((unsigned int) (a)[0]) << 24) | (((unsigned int) (a)[1]) << 16) | (((unsigned int) (a)[2]) << 8) | ((unsigned int) (a)[3]))

#define ESP_PUT_BE32(a, val)                                                \
    do {                                                                    \
        (a)[0] = (unsigned char) ((((unsigned int) (val)) >> 24) & 0xff);   \
        (a)[1] = (unsigned char) ((((unsigned int) (val)) >> 16) & 0xff);   \
        (a)[2] = (unsigned char) ((((unsigned int) (val)) >> 8)  & 0xff);   \
        (a)[3] = (unsigned char) ( ((unsigned int) (val))        & 0xff);   \
    } while (0)

static const unsigned int K[64] = {
    0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL, 0x3956c25bUL,
    0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL, 0xd807aa98UL, 0x12835b01UL,
    0x243185beUL, 0x550c7dc3UL, 0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL,
    0xc19bf174UL, 0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL,
    0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL, 0x983e5152UL,
    0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL, 0xc6e00bf3UL, 0xd5a79147UL,
    0x06ca6351UL, 0x14292967UL, 0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL,
    0x53380d13UL, 0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
    0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL, 0xd192e819UL,
    0xd6990624UL, 0xf40e3585UL, 0x106aa070UL, 0x19a4c116UL, 0x1e376c08UL,
    0x2748774cUL, 0x34b0bcb5UL, 0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL,
    0x682e6ff3UL, 0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL,
    0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL
};

typedef struct
{
    unsigned int    total[2];
    unsigned int    state[8];
    unsigned char   buffer[64];
} esp_sha256_t;

int esp_sha256_init(esp_sha256_t *ctx)
{
    ctx->total[0] = 0;
    ctx->total[1] = 0;
    ctx->state[0] = 0x6A09E667UL;
    ctx->state[1] = 0xBB67AE85UL;
    ctx->state[2] = 0x3C6EF372UL;
    ctx->state[3] = 0xA54FF53AUL;
    ctx->state[4] = 0x510E527FUL;
    ctx->state[5] = 0x9B05688CUL;
    ctx->state[6] = 0x1F83D9ABUL;
    ctx->state[7] = 0x5BE0CD19UL;
    return 0;
}

void esp_sha256_transform(esp_sha256_t *ctx, const unsigned char *buf)
{
    unsigned int S[8], W[64], t0, t1;
    unsigned int t;
    int i;

    for (i = 0; i < 8; i++)
        S[i] = ctx->state[i];

    for (i = 0; i < 16; i++)
        W[i] = ESP_GET_BE32(buf + (4 * i));

    for (i = 16; i < 64; i++)
        W[i] = Gamma1(W[i - 2]) + W[i - 7] + Gamma0(W[i - 15]) + W[i - 16];

    for (i = 0; i < 64; ++i) {
        RND(S[0], S[1], S[2], S[3], S[4], S[5], S[6], S[7], i);
        t = S[7]; S[7] = S[6]; S[6] = S[5]; S[5] = S[4];
        S[4] = S[3]; S[3] = S[2]; S[2] = S[1]; S[1] = S[0]; S[0] = t;
    }

    for (i = 0; i < 8; i++)
        ctx->state[i] = ctx->state[i] + S[i];
}

int esp_sha256_update(esp_sha256_t *ctx, const void *src, size_t size)
{
    size_t fill;
    unsigned int left;
    const unsigned char *input = (const unsigned char *)src;

    left = ctx->total[0] & 0x3F;
    fill = 64 - left;

    ctx->total[0] += size;
    ctx->total[0] &= 0xFFFFFFFF;

    if (ctx->total[0] < size)
        ctx->total[1]++;

    if (left && size >= fill) {
        memcpy(ctx->buffer + left, input, fill);

        esp_sha256_transform(ctx, ctx->buffer);

        input += fill;
        size  -= fill;
        left = 0;
    }

    while (size >= 64) {
        esp_sha256_transform(ctx, input);

        input += 64;
        size  -= 64;
    }

    if (size > 0)
        memcpy(ctx->buffer + left, input, size);

    return 0;
}

int esp_sha256_finish(esp_sha256_t *ctx)
{
    unsigned int used;
    unsigned int high, low;

    used = ctx->total[0] & 0x3F;

    ctx->buffer[used++] = 0x80;

    if (used <= 56) {
        memset(ctx->buffer + used, 0, 56 - used);
    } else {
        memset(ctx->buffer + used, 0, 64 - used);
        esp_sha256_transform(ctx, ctx->buffer);
        memset(ctx->buffer, 0, 56);
    }

    high = (ctx->total[0] >> 29) | (ctx->total[1] <<  3);
    low  = (ctx->total[0] <<  3);

    ESP_PUT_BE32(ctx->buffer +  56, high);
    ESP_PUT_BE32(ctx->buffer +  60, low);

    esp_sha256_transform(ctx, ctx->buffer);
    return( 0 );
}

HANDLE g_com = NULL;

#pragma pack(push)
#pragma pack(1) // 设定为1字节对齐

typedef struct
{
    unsigned char           zero;       // 固定为0x00
    unsigned char           op;         // 命令码
    unsigned short          data_len;   // 数据长度
    unsigned int            checksum;   // 校验码

} t_esp_loader_req_head, *p_esp_loader_req_head;

typedef struct
{
    t_esp_loader_req_head   head;       // 命令头
    unsigned char           data[1];    // 数据

} t_esp_loader_req, *p_esp_loader_req;

typedef struct _rsp
{
    unsigned char           one;        // 固定为0x01
    unsigned char           op;         // 结果
    unsigned short          data_len;   // 数据长度
    unsigned int            value;      // 值,内存读取命令应答使用
    unsigned char           data[1];    // 数据

} t_esp_loader_rsp, *p_esp_loader_rsp;

#pragma pack(pop)

void com_printf(unsigned char *data, unsigned int len)
{
    printf("   ");

    for (unsigned int i = 0; i < 16; i++)
    {
        printf("%2X ", i);

        if ((i % 8) == 0 && 0 != i)
        {
            printf(" ");
        }
    }

    printf("\n");

    for (unsigned int i = 0; i < len; i++)
    {
        if ((i % 8) == 0 && 0 != i)
        {
            printf(" ");
        }

        if ((i % 16) == 0 && 0 != i)
        {
            printf("\n");
        }

        if ((i % 16) == 0)
        {
            printf("%2X ", i / 16);
        }

        printf("%02x ", data[i]);
    }

    printf("\n");
}

HANDLE com_open(int com, int BaudRate, int ByteSize, int Parity, int StopBits, int reboot)
{
    char name[16];
    snprintf(name, sizeof(name) - 1, "com%d", com);

    HANDLE handle = CreateFileA(name,                           // 串口名
                                GENERIC_READ | GENERIC_WRITE,   // 支持读写
                                0,                              // 独占方式，串口不支持共享
                                NULL,                           // 安全属性指针，默认值为NULL
                                OPEN_EXISTING,                  // 打开现有的串口文件
                                0,                              // 0：同步方式，FILE_FLAG_OVERLAPPED：异步方式
                                NULL);                          // 用于复制文件句柄，默认值为NULL，对串口而言该参数必须置为NULL

    if (handle < 0)
    {
        printf("COM%d open fail\n", com);
        return (HANDLE)-1;
    }

    DCB p = { 0 };
	p.DCBlength = sizeof(p);
	p.BaudRate  = BaudRate;     // 波特率
	p.ByteSize  = ByteSize;     // 数据位
    p.Parity    = Parity;       // 无校验
    p.StopBits  = StopBits;     // 1位停止位

	if (!SetCommState(handle, &p))
	{
        printf("COM%d set (%d %d %d %d) fail\n", com, BaudRate, ByteSize, Parity, StopBits);
        CloseHandle(handle);
        return (HANDLE)-2;
	}

    printf("COM%d open (%d %d %d %d) success\n", com, BaudRate, ByteSize, Parity, StopBits);
/*
	// 超时处理,单位：毫秒
	// 总超时＝时间系数×读或写的字符数＋时间常量
	COMMTIMEOUTS timeOuts = {0};
	timeOuts.ReadIntervalTimeout         = 1000;    // 读间隔超时
	timeOuts.ReadTotalTimeoutMultiplier  = 1;       // 读时间系数
	timeOuts.ReadTotalTimeoutConstant    = 1;       // 读时间常量
	timeOuts.WriteTotalTimeoutMultiplier = 0;       // 写时间系数
	timeOuts.WriteTotalTimeoutConstant   = 0;       // 写时间常量

    //if (!GetCommTimeouts(handle, &timeOuts))
    //{
    //    printf("COM%d GetCommTimeouts fail\n", com);
    //    CloseHandle(handle);
    //    return (HANDLE)-3;
    //}

    printf("COM%d timeout:%d %d %d %d %d\n", com,
            timeOuts.ReadIntervalTimeout, timeOuts.ReadTotalTimeoutMultiplier, timeOuts.ReadTotalTimeoutConstant,
            timeOuts.WriteTotalTimeoutMultiplier, timeOuts.WriteTotalTimeoutConstant);

	if (!SetCommTimeouts(handle, &timeOuts))
    {
        printf("COM%d SetCommTimeouts fail\n", com);
        CloseHandle(handle);
        return (HANDLE)-4;
    }
*/
	if (!PurgeComm(handle, PURGE_TXCLEAR | PURGE_RXCLEAR)) // 清空串口缓冲区
    {
        printf("COM%d PurgeComm fail\n", com);
        CloseHandle(handle);
        return (HANDLE)-5;
    }

    return handle;
}

int com_send(HANDLE handle, const void *data, unsigned int len)
{
    unsigned int   j   = 0;
    unsigned int   num = 0;
    unsigned char *in  = (unsigned char*)data;
    unsigned char *out = (unsigned char*)malloc(len * 2 + 2);

    out[j++] = '\xc0';

    for (unsigned int i = 0; i < len; i++)
    {
        if (in[i] == '\xc0')
        {
            out[j++] = '\xdb';
            out[j++] = '\xdc';
        }
        else if (in[i] == '\xdb')
        {
            out[j++] = '\xdb';
            out[j++] = '\xdd';
        }
        else
        {
            out[j++] = in[i];
        }
    }

    out[j++] = '\xc0';

    com_printf(out, j);

	if (!WriteFile(handle, out, j, &num, NULL))
	{
        printf("com:%d send fail %d %d\n", (int)handle, errno, GetLastError());
		return -1;
	}

    printf("send data:%d %d %d\n", len, j, num);
    return len;
}

typedef int (*CALLBACK_PROC)(p_esp_loader_rsp rsp, int id);

int callback(p_esp_loader_rsp rsp, int id)
{
    printf("id:%d magic:%d op:%d data_len:%d value:%8x ret:%d\n", id, rsp->one, rsp->op, rsp->data_len, rsp->value,  rsp->data[(rsp->data_len == 2) ? 1 : 3]);
    return 0;
}

int com_recv(HANDLE handle, void *data, unsigned int max_len, CALLBACK_PROC proc)
{
    int len;

    printf("recv max:%d\n", max_len);

	if (!ReadFile(handle, data, max_len, &len, NULL))
	{
        printf("COM:%d recv fail %d %d\n", (int)handle, errno, GetLastError());
		return -1;
	}

    int j = 0;
    char *in  = (char*)data;
    char *out = (char*)data;

    com_printf(in, len);

    if ('\xc0' != in[0] || '\xc0' != in[len - 1])
    {
        printf("head end not 0xc0 len:%d\n", len);
        return -2;
    }

    for (int i = 0; i < len; i++)
    {
        if ('\xdb' == in[i] && '\xdc' == in[i + 1])
        {
            i++;
            out[j++] = '\xc0';
        }
        else if ('\xdb' == in[i] && '\xdd' == in[i + 1])
        {
            i++;
            out[j++] = '\xdb';
        }
        else
        {
            out[j++] = in[i];
        }
    }

    printf("recv data %d %d\n", len, j);

    int num = 0;
    p_esp_loader_rsp rsp;

    for (int i = 1; i < j; )
    {
        rsp = (p_esp_loader_rsp)(out + i);

        proc(rsp, num++);

        len = sizeof(t_esp_loader_rsp) + rsp->data_len;

        if (rsp->data[rsp->data_len] != 0xc0)
        {
            printf("end error\n");
            break;
        }

        i += len + 1; // 只加1,t_esp_loader_rsp中多了1位data
    }

    return num;
}

void com_reboot(HANDLE handle)
{
    EscapeCommFunction(handle, SETRTS);
    EscapeCommFunction(handle, CLRRTS);
}

/**
 * \brief   打印信息
 * \param   [in]  无
 * \return        无
 */
void printf_info()
{
    printf("esp_app.exe ota -size=0x1000 -o=output.bin\n");
    printf("esp_app.exe cvs -i=input.cvs -o=output.bin\n");
    printf("esp_app.exe bin -i=input.elf -o=output.bin -model=12F -version={1|3}\n");
    printf("esp_app.exe rom -i=input.bin -addr=0x8000 -com=4\n");
    printf("esp_app.exe ram -i=input.bin -addr=0x4000 -com=4\n");
}

/**
 * \brief   检测主函数外传入的参数
 * \param   [in]  int               argc            参数数量
 * \param   [in]  char**            argv            参数
 * \return        int                               0:成功,其它失败
 */
int check_args(int argc, char **argv, p_arg arg)
{
    const char *addr;
    const char *size;
    const char *model;
    const char *input;
    const char *output;
    const char *version;

    if (0 == strcmp(argv[1], "ota") && NULL != argv[2] && NULL != argv[3] && (size = strstr(argv[2], "-size=0x")) && (output = strstr(argv[3], "-o=")))
    {
        arg->type   = OTA;
        arg->size   = strtol(size + 8, NULL, 16);
        arg->output = output + 3;
    }
    else if (0 == strcmp(argv[1], "cvs") && NULL != argv[2] && NULL != argv[3] && (input = strstr(argv[2], "-i=")) && (output = strstr(argv[3], "-o=")))
    {
        arg->type   = CVS;
        arg->input  = input + 3;
        arg->output = output + 3;
    }
    else if (0 == strcmp(argv[1], "bin") && NULL != argv[2] && NULL != argv[3] && NULL != argv[4] && NULL != argv[5]  && (input = strstr(argv[2], "-i=")) && (output = strstr(argv[3], "-o="))&& (model = strstr(argv[4], "-model=12F")) && (version = strstr(argv[5], "-version=")))
    {
        arg->type    = BIN;
        arg->model   = F12;
        arg->version = atoi(version + 9);
        arg->input   = input + 3;
        arg->output  = output + 3;
    }
    else if (0 == strcmp(argv[1], "rom") && NULL != argv[2] && NULL != argv[3] && NULL != argv[3] && (input = strstr(argv[2], "-i=")) && (addr = strstr(argv[3], "-addr=0x")) && (addr = strstr(argv[4], "-com=")))
    {
        arg->type  = ROM;
        arg->com   = atoi(addr + 5);
        arg->addr  = strtol(addr + 8, NULL, 16);
        arg->input = input + 3;
    }
    else if (0 == strcmp(argv[1], "ram") && NULL != argv[2] && NULL != argv[3] && NULL != argv[3] && (input = strstr(argv[2], "-i=")) && (addr = strstr(argv[3], "-addr=0x")) && (addr = strstr(argv[4], "-com=")))
    {
        arg->type  = RAM;
        arg->com   = atoi(addr + 5);
        arg->addr  = strtol(addr + 8, NULL, 16);
        arg->input = input + 3;
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
    FILE *fp = NULL;

    if (0 != fopen_s(&fp, output, "wb+"))
    {
        printf("open %s error\n", output);
        return -1;
    }

    unsigned char *data = (unsigned char *)malloc(size);

    memset(data, 0xFF, size);

    fwrite(data, 1, size, fp);

    fclose(fp);

    free(data);

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
    char line_dat[96][1024];
    FILE *fp;

    if (0 != fopen_s(&fp, input, "rb"))
    {
        printf("open %s error\n", input);
        return -1;
    }

    for(line_num = 0; line_num < 96 && NULL != fgets(line_dat[line_num], sizeof(line_dat[0]) - 1, fp); line_num++);

    fclose(fp);

    char *id;
    char sub[64];
    char type[64];
    int len     = 0;
    int num     = 0;
    int end     = 0;
    int align[] = { 0x1000, 0x04 }; // app, data
    t_cvs *ptr  = NULL;
    t_cvs cvs[96];

    memset(&cvs, 0xFF, sizeof(cvs));

    for (int i = 0; i < line_num; i++)
    {
        if (1 == sscanf_s(line_dat[i], "%s", type, sizeof(type) - 1) && '#' == type[0]) continue;// 注释

        ptr = &(cvs[num++]);

        if (5 != sscanf_s(line_dat[i], "%[^,], %[^,], %[^,], 0x%x, 0x%x", ptr->name, sizeof(ptr->name) - 1, type, sizeof(type) - 1, sub, sizeof(sub) - 1, &(ptr->offset), &(ptr->size)))
        {
            printf("error line:%s\n", line_dat[i]);
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

    if (0 != fopen_s(&fp, output, "wb+"))
    {
        printf("open %s error\n", output);
        return -5;
    }

    fwrite(&cvs, 1, sizeof(cvs), fp);
    fclose(fp);
    return 0;
}

/**
 * \brief   elf->bin
 * \param   [in]  const char *      input           输入文件路径
 * \param   [in]  const char *      output          输出文件路径
 * \param   [in]  unsigned int      modle           ESP型号
 * \param   [in]  unsigned int      versio          BIN版本
 * \return        int                               0:成功,其它失败
 */
int process_bin(const char *input, const char *output, unsigned int model, unsigned int version)
{
#define BUFF_SIZE   16 * 1024 * 1024
    FILE           *fp;
    char           *in = (char*)malloc(BUFF_SIZE);
    char           *out = (char*)malloc(BUFF_SIZE);
    p_Elf32_Ehdr    elf = (p_Elf32_Ehdr)in;

    if (0 != fopen_s(&fp, input, "rb"))
    {
        free(in);
        free(out);
        printf("open %s error\n", input);
        return -1;
    }

    fseek(fp, 0, SEEK_SET);
    fread(in, 1, BUFF_SIZE, fp);
    fclose(fp);

    printf("e_entry:     \t0x%x\n", elf->e_entry);
    printf("e_shoff:     \t0x%x\n", elf->e_shoff);
    printf("e_shnum:     \t0x%x\n", elf->e_shnum);
    printf("e_shentsize: \t0x%x\n", elf->e_shentsize);
    printf("e_shstrndx:  \t0x%x\n", elf->e_shstrndx);

    unsigned int ro;
    unsigned int id;
    unsigned int len;
    unsigned int pad;
    unsigned int num[2] = { 0 };
    p_sec_data   ptr;
    t_sec_data   sec[2][512];
    p_Elf32_Shdr section = (p_Elf32_Shdr)(in + elf->e_shoff);
    const char  *name = in + section[elf->e_shstrndx].sh_offset;

    for (int i = 0; i < elf->e_shnum; i++, section++)
    {
        if (1 == section->sh_type  && 0 != section->sh_addr && 0 != section->sh_size)
        {
            id = (0 != strncmp(name + section->sh_name, ".flash.", 7));
            ro = ((0 == strcmp(name + section->sh_name, ".flash.rodata")) ? 8 : 0);

            ptr = &(sec[id][num[id]]);
            ptr->offset   = section->sh_offset + ro;
            ptr->sec.addr = section->sh_addr;
            ptr->sec.size = section->sh_size - ro;

            len = ptr->sec.size;
            pad = (0 == len % 4) ? 0 : (4 - len % 4); // 4字节对齐,需要补齐
            ptr->sec.size += pad;
            memset(in + ptr->offset + len, 0, pad);

            num[id]++;
            printf("%s\taddr:0x%x off:0x%05x size:0x%05x pad:%d\n", name + section->sh_name, section->sh_addr, section->sh_offset, section->sh_size, pad);
        }
    }

    if (3 == version)
    {
        // 排序
        for (int i = 0; i < 2; i++)
        {
            for (unsigned int j = 0; j < num[i] - 1; j++)
            {
                for (unsigned int k = 1; k < num[i]; k++)
                {
                    if (sec[i][k].sec.addr < sec[i][j].sec.addr)
                    {
                        t_sec_data t = sec[i][j];
                        sec[i][j] = sec[i][k];
                        sec[i][k] = t;
                    }
                }
            }
        }
    }

    // 文件头
    t_bin head = { 0xE9, num[0] + num[1], 0x02, (3 == version) ? 0x20 : 0x40, elf->e_entry};
    len = sizeof(head);
    memcpy(out, &head, len);

    // 计算xor校验码,只校验段数据部分,最后补齐16字节对齐,为了放校验码最少有1位,最多16位
    unsigned char checksum = 0xef;  // 校验码

    for (int i = 0; i < 2; i++)
    {
        for (unsigned int j = 0; j < num[i]; j++)
        {
            ptr = &(sec[i][j]);

            for (unsigned int k = 0; k < ptr->sec.size; k++)
            {
                checksum ^= in[ptr->offset + k];
            }

            memcpy(&out[len], &(ptr->sec), sizeof(ptr->sec));
            len += sizeof(ptr->sec);
            memcpy(&out[len], in + ptr->offset, ptr->sec.size);
            len += ptr->sec.size;

            printf("section checksum %2x\n", checksum);
        }
    }

    pad = (0 == len % 16) ? 16 : (16 - len % 16); // 16字节对齐
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

    if (0 != fopen_s(&fp, output, "wb+"))
    {
        printf("open %s error\n", output);
        return -2;
    }

    fwrite(out, 1, len, fp);
    fclose(fp);
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
    char        *in = (char*)malloc(BUFF_SIZE);
    unsigned int in_len;

    FILE *fp = NULL;

    if (0 != fopen_s(&fp, input, "rb"))
    {
        free(in);
        printf("open %s error\n", input);
        return -1;
    }

    in_len = fread(in, 1, BUFF_SIZE, fp);
    fclose(fp);

    //----------------------------------------------------------------
    // 进入串口下载模式

    g_com = com_open(com, 74880, 8, 0, 1, 1);

    if (g_com < 0)
    {
        return -2;
    }

    EscapeCommFunction(g_com, SETRTS);
    EscapeCommFunction(g_com, CLRRTS);
    EscapeCommFunction(g_com, SETDTR);
    Sleep(100);
    EscapeCommFunction(g_com, CLRDTR);
    EscapeCommFunction(g_com, CLRRTS);

    CloseHandle(g_com);

    //----------------------------------------------------------------

    unsigned char *buf = (unsigned char *)malloc(BUFF_SIZE);
    p_esp_loader_req req = (p_esp_loader_req)buf;

    // 0x08|同步|0x07,0x07,0x12,0x20,0x55*32
    req->head.zero       = 0;
    req->head.op         = 0x08;
    req->head.data_len   = 36;
    req->head.checksum   = 0;
    req->data[0]         = 0x07;
    req->data[1]         = 0x07;
    req->data[2]         = 0x12;
    req->data[3]         = 0x20;
    memset(req->data + 4, 0x55, 32);

    g_com = com_open(com, 115200, 8, 0, 1, 1);

    if (g_com < 0)
    {
        return -3;
    }

    int ret = com_send(g_com, req, sizeof(req->head) + req->head.data_len);

    if (ret <= 0)
    {
        CloseHandle(g_com);
        return -4;
    }

    ret = com_send(g_com, req, sizeof(req->head) + req->head.data_len); // 发送2个同步命令,收到8个应答

    if (ret <= 0)
    {
        CloseHandle(g_com);
        return -4;
    }

    ret = com_recv(g_com, buf, BUFF_SIZE, callback);

    if (ret <= 0)
    {
        CloseHandle(g_com);
        return -5;
    }

    CloseHandle(g_com);
    return 0;
}

/**
 * \brief   向内存写入数据
 * \param   [in]  const char *      input           输入文件路径
 * \param   [in]  unsigned int      addr            写入地址
 * \param   [in]  unsigned int      com             串口号
 * \return        int                               0:成功,其它失败
 */
int process_ram(const char *input, unsigned int addr, unsigned int com)
{
    char        *in = (char*)malloc(BUFF_SIZE);
    unsigned int in_len;

    FILE *fp = NULL;

    if (0 != fopen_s(&fp, input, "rb"))
    {
        free(in);
        printf("open %s error\n", input);
        return -1;
    }

    in_len = fread(in, 1, BUFF_SIZE, fp);
    fclose(fp);

    printf("input:%u\n", in_len);

    return 0;
}

/**
 * \brief   主函数
 * \param   [in]  int               argc            参数数量
 * \param   [in]  char**            argv            参数
 * \return        int                               0-成功，其它失败
 */
int main(int argc, char **argv)
{
    if (argc <= 1 || 0 != check_args(argc, argv, &g_arg))
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
        case CVS:
        {
            return process_csv(g_arg.input, g_arg.output);
        }
        case BIN:
        {
            return process_bin(g_arg.input, g_arg.output, g_arg.model, g_arg.version);
        }
        case ROM:
        {
            return process_rom(g_arg.input, g_arg.addr, g_arg.com);
        }
        case RAM:
        {
            return process_ram(g_arg.input, g_arg.addr, g_arg.com);
        }
        default:
        {
            printf("error %d\n", g_arg.type);
            break;
        }
    }

    return -2;
}