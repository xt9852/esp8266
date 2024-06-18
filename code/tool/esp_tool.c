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

enum
{
    APP = 0,
    OTA,
    CVS,
    BIN,
    FLASH,
    NODEMCU,
};

typedef struct
{
    unsigned int    type;       // 类型:OTA,CVS,BIN,FLASH
    unsigned int    com;        // 串口号
    unsigned int    size;       // 数据的长度
    unsigned int    addr;       // 数据的地址
    unsigned int    module;     // 模块:NODEMCU
    unsigned int    version;    // 版本:1,3
    const char     *input;      // 输入文件
    const char     *output;     // 输出文件

} t_arg, *p_arg;

typedef struct
{
    unsigned short flag;        // 0x50AA
    unsigned char  type;        // app-0,data-1
    unsigned char  type_sub;    // factory-0x00,test-0x20,ota-0x00, phy-0x01, nvs-0x02, coredump-0x03, nvs_keys-0x04, efuse-0x05, esphttpd-0x80, fat-0x81, spiffs-0x82
    unsigned int   offset;      // 位置
    unsigned int   size;        // 大小
    char           name[20];    // 名称

} t_cvs, *p_cvs;

typedef struct
{
    unsigned char   flag;       // 固定0xE9
    unsigned char   sec_num;    // 段数量
    unsigned char   flash_mod;  // FLASH模式          QIO,QOUT,DIO,DOUT,FAST_READ,SLOW_READ
    unsigned char   flash_sf;   // FLASH大小和频率    大小:1MB,2MB,4MB,8MB,16MB 频率40M,26M,20M,80M
    unsigned int    entry;      // 入口地址

} t_bin, *p_bin;

typedef struct
{
    unsigned int    addr;
    unsigned int    size;

} t_sec, *p_sec;

typedef struct
{
    t_sec           sec;
    unsigned int    offset;

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

#define ESP_GET_BE32(a) ((((unsigned int) (a)[0]) << 24) | (((unsigned int) (a)[1]) << 16) | (((unsigned int) (a)[2]) << 8) | ((unsigned int) (a)[3]))

#define ESP_PUT_BE32(a, val)                                                \
    do {                                                                    \
        (a)[0] = (unsigned char) ((((unsigned int) (val)) >> 24) & 0xff);   \
        (a)[1] = (unsigned char) ((((unsigned int) (val)) >> 16) & 0xff);   \
        (a)[2] = (unsigned char) ((((unsigned int) (val)) >> 8)  & 0xff);   \
        (a)[3] = (unsigned char) ( ((unsigned int) (val))        & 0xff);   \
    } while (0)

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

/**
 * \brief   打印参数信息
 * \param   [in]  无
 * \return        无
 */
void printf_args_info()
{
    printf("cvs   -i=input.cvs -o=out.bin\n");
    printf("ota   -size=0x2000 -o=out.bin\n");
    printf("bin   -module=NodeMCU -v={1|3} -i=input.elf -o=output.bin\n");
    printf("flash -com=4 -addr=0x8000 -i=app.bin\n");
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
    const char *input;
    const char *output;
    const char *module;
    const char *version;

    if (0 == strcmp(argv[1], "cvs") && NULL != argv[2] && NULL != argv[3] && (input = strstr(argv[2], "-i=")) && (output = strstr(argv[3], "-o=")))
    {
        arg->type   = CVS;
        arg->input  = input + 3;
        arg->output = output + 3;
    }
    else if (0 == strcmp(argv[1], "ota") && NULL != argv[2] && NULL != argv[3] && (size = strstr(argv[2], "-size=0x")) && (output = strstr(argv[3], "-o=")))
    {
        arg->type   = OTA;
        arg->size   = strtol(size + 8, NULL, 16);
        arg->output = output + 3;
    }
    else if (0 == strcmp(argv[1], "bin") && NULL != argv[2] && NULL != argv[3] && NULL != argv[4] && NULL != argv[5] &&
            (module = strstr(argv[2], "-module=NodeMCU")) && (version = strstr(argv[3], "-v=")) && (input = strstr(argv[4], "-i=")) && (output = strstr(argv[5], "-o=")))
    {
        arg->type    = BIN;
        arg->module  = NODEMCU;
        arg->version = atoi(version + 3);
        arg->input   = input + 3;
        arg->output  = output + 3;
    }
    else if (0 == strcmp(argv[1], "flash") && NULL != argv[2] && NULL != argv[3] && NULL != argv[3] && (addr = strstr(argv[2], "-com=")) && (addr = strstr(argv[3], "-addr=0x")) && (input = strstr(argv[4], "-i=")))
    {
        arg->type  = FLASH;
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
 * \brief   生成分区表数据
 *
 *          分区表文件:
 *          # Name,   Type, SubType, Offset,   Size, Flags
 *          # Note: if you change the phy_init or app partition offset, make sure to change the offset in Kconfig.projbuild
 *          nvs,      data, nvs,     0x9000,  0x4000
 *          otadata,  data, ota,     0xd000,  0x2000
 *          phy_init, data, phy,     0xf000,  0x1000
 *          ota_0,    0,    ota_0,   0x10000, 0xF0000
 *          ota_1,    0,    ota_1,   0x110000,0xF0000
 *
 *          Type    :
 *                    app-0
 *                    data-1
 *          SubType :
 *                    app  : factory-0x00, test-0x20
 *                    data : ota-0x00, phy-0x01, nvs-0x02, coredump-0x03, nvs_keys-0x04, efuse-0x05,
 *                           ota_0-0x10, ota_1-0x11, ota_2-0x12, ota_3-0x13, ota_4-0x14, ota_5-0x15, ota_6-0x16, ota_7-0x17,
 *                           ota_8-0x18, ota_9-0x19, ota_10-0x1a, ota_11-0x1b, ota_12-0x1c, ota_13-0x1d, ota_14-0x1e, ota_15-0x1f,
 *                           esphttpd-0x80, fat-0x81, spiffs-0x82
 *
 *          每条信息32字字=AA 50(2字节) + Type(1字节) + SubType(1字节) + Offset(4字节) + Length(4字节) + Usage(20字节)
 *          最多96条3KB
 *          ota_**最多16条
 *
 * \param   [in]  const char *      input           输入文件路径
 * \param   [in]  const char *      output          输出文件路径
 * \return        int                               0:成功,其它失败
 */
int process_csv(const char *input, const char *output)
{
    FILE *fp_in  = NULL;
    FILE *fp_out = NULL;

    if (0 != fopen_s(&fp_in, input, "rb"))
    {
        printf("open %s error\n", input);
        return -1;
    }

    if (0 != fopen_s(&fp_out, output, "wb+"))
    {
        fclose(fp_in);
        printf("open %s error\n", output);
        return -2;
    }

    int count = 0;
    char sub[64];
    char type[64];
    char line[256];
    const char *ptr;
    unsigned int end     = 0;
    unsigned int align[] = { 0x1000, 0x04 };
    t_cvs        cvs     = { 0x50AA };

    while(NULL != fgets(line, sizeof(line) - 1, fp_in))
    {
        sscanf_s(line, "%s", type, sizeof(type) - 1);

        if ('#' == type[0]) // 注释
        {
            continue;
        }

        if (5 != sscanf_s(line, "%[^,], %[^,], %[^,], 0x%x, 0x%x", cvs.name, sizeof(cvs.name) - 1, type, sizeof(type) - 1, sub, sizeof(sub) - 1, &cvs.offset, &cvs.size))
        {
            printf("error line:%s\n", line);
            fclose(fp_in);
            fclose(fp_out);
            return -3;
        }

        int len = strlen(cvs.name);
        memset(cvs.name + len, 0, sizeof(cvs.name) - len);

        cvs.type = (0 == strcmp(type, "data"));

        ptr = strstr("factory-0,test-20,ota-00,phy-01,nvs-02,coredump-03,nvs_keys-04,efuse-05,"
                     "ota_0-10,ota_1-11,ota_2-12,ota_3-13,ota_4-14,ota_5-15,ota_6-16,ota_7-17,"
                     "ota_8-18,ota_9-19,ota_10-1a,ota_11-1b,ota_12-1c,ota_13-1d,ota_14-1e,ota_15-1f,"
                     "esphttpd-80,fat-81,spiffs-82", sub);

        cvs.type_sub = (NULL == ptr) ? 0 : (unsigned char)strtol(ptr + strlen(sub) + 1, NULL, 16);

        if (cvs.offset % align[cvs.type] || cvs.size % align[cvs.type])
        {
            printf("name:%s (offset,size) no align:0x%x\n", cvs.name, align[cvs.type]);
            fclose(fp_in);
            fclose(fp_out);
            return -4;
        }

        if (cvs.offset <= end)
        {
            printf("name:%s offset:0x%x <= last end:0x%x\n", cvs.name, cvs.offset, end);
            fclose(fp_in);
            fclose(fp_out);
            return -5;
        }

        count++;
        end = cvs.offset + cvs.size - 1;
        fwrite(&cvs, 1, sizeof(cvs), fp_out);

        printf("name: %19s type: %x sub: %2x offset: %8x size: %x\n", cvs.name, cvs.type, cvs.type_sub, cvs.offset, cvs.size);
    }

    printf("%s count:%d ok\n", output, count);

    memset(&cvs, 0xFF, sizeof(cvs));

    for (int i = count; i < 96; i++) // 填充成3KB
    {
        fwrite(&cvs, 1, sizeof(cvs), fp_out);
    }

    fclose(fp_in);
    fclose(fp_out);
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
 * \brief   elf->bin
 * \param   [in]  const char *      input           输入文件路径
 * \param   [in]  const char *      output          输出文件路径
 * \return        int                               0:成功,其它失败
 */
int process_bin(unsigned int module, unsigned int version, const char *input, const char *output)
{
    FILE           *fp_in;
    FILE           *fp_out;
    unsigned int    len;
    char           *buff = (char*)malloc(16 * 1024 * 1024);
    p_Elf32_Ehdr    head = (p_Elf32_Ehdr)buff;

    if (0 != fopen_s(&fp_in, input, "rb"))
    {
        printf("open %s error\n", input);
        return -1;
    }

    fseek(fp_in, 0, SEEK_END);
    len = ftell(fp_in);

    fseek(fp_in, 0, SEEK_SET);
    fread(buff, 1, len, fp_in);
    fclose(fp_in);

    printf("e_entry:     \t 0x%x\n", head->e_entry);
    printf("e_shoff:     \t 0x%x\n", head->e_shoff);
    printf("e_shnum:     \t 0x%x\n", head->e_shnum);
    printf("e_shentsize: \t 0x%x\n", head->e_shentsize);
    printf("e_shstrndx:  \t 0x%x\n", head->e_shstrndx);

    unsigned int id;
    unsigned int pad;
    unsigned int num[2] = { 0 };
    t_sec_data   sec[2][512];
    p_Elf32_Shdr section = (p_Elf32_Shdr)(buff + head->e_shoff);
    const char  *name = buff + section[head->e_shstrndx].sh_offset;

    for (int i = 0; i < head->e_shnum; i++, section++)
    {
        if (1 == section->sh_type  && 0 != section->sh_addr && 0 != section->sh_size)
        {
            id = (0 != strncmp(name + section->sh_name, ".flash.", 7));

            sec[id][num[id]].offset   = section->sh_offset + ((0 == strcmp(name + section->sh_name, ".flash.rodata")) ? 8 : 0);
            sec[id][num[id]].sec.addr = section->sh_addr;
            sec[id][num[id]].sec.size = section->sh_size - ((0 == strcmp(name + section->sh_name, ".flash.rodata")) ? 8 : 0);

            len = sec[id][num[id]].sec.size;
            pad = (0 == len % 4) ? 0 : (4 - len % 4); // 4字节对齐,需要补齐
            sec[id][num[id]].sec.size += pad;
            memset(buff + sec[id][num[id]].offset + len, 0, pad);

            num[id]++;
            printf("%s\t addr:0x%x off:0x%05x size:0x%05x pad:%d\n", name + section->sh_name, section->sh_addr, section->sh_offset, section->sh_size, pad);
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

    unsigned char data;
    unsigned char checksum = 0xef;
    esp_sha256_t sha256;

    if (0 != fopen_s(&fp_out, output, "wb+"))
    {
        free(buff);
        printf("open %s error\n", output);
        return -2;
    }

    t_bin bin = { 0xE9, num[0] + num[1], 0x02, (3 == version) ? 0x20 : 0x40, head->e_entry};
    fwrite(&bin, 1, sizeof(bin), fp_out);

    if (3 == version)
    {
        esp_sha256_init(&sha256);
        esp_sha256_update(&sha256, &bin, sizeof(bin));
    }

    for (int i = 0; i < 2; i++)
    {
        for (unsigned int j = 0; j < num[i]; j++)
        {
            fwrite(&(sec[i][j].sec), 1, sizeof(sec[i][j].sec), fp_out);
            fwrite(buff + sec[i][j].offset, 1, sec[i][j].sec.size, fp_out);

            if (3 == version)
            {
                esp_sha256_update(&sha256, &(sec[i][j].sec), sizeof(sec[i][j].sec));
                esp_sha256_update(&sha256, buff + sec[i][j].offset, sec[i][j].sec.size);
            }

            for (unsigned int k = 0; k < sec[i][j].sec.size; k++)
            {
                data = buff[sec[i][j].offset + k];
                checksum ^= data;
            }

            printf("checksum:%x\n", checksum);
        }
    }

    len = ftell(fp_out);
    pad = (0 == len % 16) ? 0 : (16 - len % 16); // 16字节对齐
    buff[pad - 1] = checksum;
    memset(buff, 0, pad - 1);

    fwrite(buff, 1, pad, fp_out);

    printf("checksum:%x pad:%d\n", checksum, pad);

    if (3 == version)
    {
        esp_sha256_update(&sha256, buff, pad);
        esp_sha256_finish(&sha256);

        for (int i = 0; i < 8; i++)
        {
            id = sha256.state[i];
            sha256.state[i] = ((id & 0xFF000000) >> 24) | ((id & 0x00FF0000) >> 8) | ((id & 0x0000FF00)) << 8 | ((id & 0x000000FF) << 24);
        }

        fwrite(&(sha256.state), 1, sizeof(sha256.state), fp_out);

        printf("sha256:%08x%08x%08x%08x%08x%08x%08x%08x\n",
                sha256.state[0], sha256.state[1], sha256.state[2], sha256.state[3],
                sha256.state[4], sha256.state[5], sha256.state[6], sha256.state[7]);
    }

    free(buff);
    fclose(fp_out);
    return 0;
}

/**
 * \brief   刷新固件
 * \param   [in]  unsigned int      com             串口号
 * \param   [in]  unsigned int      addr            写入地址
 * \param   [in]  const char *      input           输入文件路径
 * \return        int                               0:成功,其它失败
 */
int process_flash(unsigned int com, unsigned int addr, const char *input)
{
    printf("com:%d\n",    com);
    printf("addr:0x%x\n", addr);
    printf("input:%s\n",  input);

    FILE *fp = NULL;
    //fopen_s(output, "wb+");

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
        printf_args_info();
        return -1;
    }

    switch (g_arg.type)
    {
        case CVS:
        {
            return process_csv(g_arg.input, g_arg.output);
        }
        case OTA:
        {
            return process_ota(g_arg.size, g_arg.output);
        }
        case BIN:
        {
            return process_bin(g_arg.module, g_arg.version, g_arg.input, g_arg.output);
        }
        case FLASH:
        {
            return process_flash(g_arg.com, g_arg.addr, g_arg.input);
        }
    }

    return -2;
}