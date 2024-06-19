#### ESP12-F
- 载有Tensilica公司xtensa系列L106超低功耗32位微型MCU

参数|数值
-|-
物理内存|**96KB(0x18000)** 的数据内存, **64KB(0x10000)** 的指令内存
数据内存地址|**0x3FFE8000 ~ 0x3FFFFFFF**
指令内存地址|**0x40100000 ~ 0x4010FFFF**
Flash|**4MB**
Flash模式|**QIO**
晶振|**26MHz**

#### 缓存
- 缓存(iROM): 因为内存有限, 所以就不能把所有的数据都载入内存, 只能是先载入必要的数据，其他数据保留在Flash中，在需要的时候读取
- ESP8266_RTOS_SDK函数默认存放在缓存(iROM)中，代码中无需再添加 ICACHE_FLASH_ATTR 宏,中断处理函数也可以在iROM中,一些频繁调用的函数要想放在iRAM中, 在函数前添加 IRAM_ATTR 宏即可

#### 启动模式
ESP8266 Reset 后, 通过判断如下管脚的状态来决定启动模式:

GPIO15|GPIO0|GPIO2|模式|说明
-|-|-|-|-
L|L|H|UART|串口刷机
L|H|H|Flash|SPI Flash 正常启动
H|x|x|SDIO|SD-card 启动

启动时串口(波特率74880)输出的boot mode:(x, y),x的低3位对应{GPIO15, 0, 2}

``` python
ets Jan  8 2013,rst cause:2, boot mode:(3,6)
load 0x40100000, len 5976, room 16
tail 8
chksum 0x96
load 0x3ffe8408, len 24, room 0
tail 8
chksum 0x7f
load 0x3ffe8420, len 3268, room 0
tail 4
chksum 0xf8
csum 0xf8
```

#### ESP8266启动过程
> **片内 Bootloader**
在没有外置 SPI Flash 的情形下，ESP8266 只需要几个外围支持元件（晶振、电容、电阻），也能启动，其片内有 ROM，带一个很小的 XTOS（xtensa 提供的最小系统，类似PC机的BIOS）

- 芯片上电后首先在0x40000080处运行片内的ROM(片内Bootloader), 完成必要初始化
- 片内Bootloader读取Flash的0x00000处的数据(booloader.bin头部), 并读取出 .text,.data,.rodata三个段在内存中的位置, 并将这3部分加载到内存中
- 加载完毕后跳到入口地址entry_addr处, 执行片外Bootloader
- .text加载到iRAM(0x40100000)
- .data和.rodata加载到dRAM(0x3FFE8000)
- 在dRAM上还有bbs, stack, heap, 要注意使用量

#### 段说明
- .text 程序代码段, 标记RO只读
- .data 已初始化的全局变量的一块内存区域, 属于静态内存分配, 标记RW可读写
- .bss 未初始化的全局变量的一块内存区域, 属于静态内存分配, 标记ZI, 这个段不占用ROM空间, 只有在程序运行的时候在RAM被初始化为0
- .rodata 用来存放程序中常量的一块内存区域, 属于静态内存分配, 标记RO只读
- COMMON	未初始化的全局变量

#### BIN文件
- bin文件由elf文件转化而来

位置|大小(字节)|说明
-|-|-
0x00|1|固定E9
0x01|1|段数量
0x02|1|FLASH模式:QIO,QOUT,DIO,DOUT,FAST_READ,SLOW_READ
0x03|1|FLASH大小和频率<br>大小<br>V1: 512K, 256K, 1M, 2M, 4M, 2M-c1, 4M-c1, 8M-8, 16M-9<br>V3: 1MB, 2MB, 4MB, 8MB, 16MB<br>频率<br>40M, 26M, 20M, 80M
0x04|4|入口地址
0x05|4|段1地址
0x06|4|段1长度(包括4字节对齐数据长)
0x07|段长度|段1数据
||0~3|4字节对齐,填充0x00
...|...|段n
||1~16|16字节对齐,填充0x00,最后一位放段数据的校验码(1字节)
||32|上面全部数据SHA256校验码

#### 分区表文件
- 分区说明文件: ESP8266_RTOS_SDK-v3.4/components/partition_table/partitions_two_ota.csv
``` python
# Name,   Type, SubType, Offset,   Size, Flags
# Note: if you change the phy_init or app partition offset, make sure to change the offset in Kconfig.projbuild
nvs,      data, nvs,     0x9000,  0x4000
otadata,  data, ota,     0xd000,  0x2000
phy_init, data, phy,     0xf000,  0x1000
ota_0,    0,    ota_0,   0x10000, 0xF0000
ota_1,    0,    ota_1,   0x110000,0xF0000
```
- 有OTA的FLASH布局:
``` python
BootLoader     0x000000 0x08000    32K
PartitionTable 0x008000 0x01000     4K
WiFi data nvs  0x009000 0x04000    16K
OTA data       0x00d000 0x02000     8K
RF data        0x00f000 0x01000     4K
App            0x010000 0xf0000   960K
App            0x110000 0xf0000   960K
```
- 分区数据文件: partitions_two_ota.bin
> 每条信息32字节=AA 50(2字节) + Type(1字节) + SubType(1字节) + Offset(4字节) + Length(4字节) + Usage(20字节)
最多96条3KB,其余空间全为FF,ota_**最多16条
``` python
Offset      0  1  2  3  4  5  6  7   8  9  A  B  C  D  E  F
00000000   AA 50 01 02 00 90 00 00  00 40 00 00 6E 76 73 00   狿.......@..nvs.
00000010   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ................
00000020   AA 50 01 00 00 D0 00 00  00 20 00 00 6F 74 61 64   狿...?.. ..otad
00000030   61 74 61 00 00 00 00 00  00 00 00 00 00 00 00 00   ata.............
00000040   AA 50 01 01 00 F0 00 00  00 10 00 00 70 68 79 5F   狿...?.....phy_
00000050   69 6E 69 74 00 00 00 00  00 00 00 00 00 00 00 00   init............
00000060   AA 50 00 10 00 00 01 00  00 00 0F 00 6F 74 61 5F   狿..........ota_
00000070   30 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   0...............
00000080   AA 50 00 11 00 00 11 00  00 00 0F 00 6F 74 61 5F   狿..........ota_
00000090   31 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   1...............
```

Type|0x值
-|-
app|0
data|1

Type|SubType|0x值
-|-|-
app|factory|00
app|test|20
data|ota|00
data|phy|01
data|nvs|02
data|coredump|03
data|nvs_keys|04
data|efuse|05
data|ota_0|10
data|ota_1|11
data|ota_2|12
data|ota_3|13
data|ota_4|14
data|ota_5|15
data|ota_6|16
data|ota_7|17
data|ota_8|18
data|ota_9|19
data|ota_10|1a
data|ota_11|1b
data|ota_12|1c
data|ota_13|1d
data|ota_14|1e
data|ota_15|1f
data|esphttpd|80
data|fat|81
data|spiffs|82

#### OTA数据
ota_data_initial.bin,大小8192(8KB,由分区表决定其大小),全为FF

#### bootloader代码
``` python
bootloader_start.c : call_start_cpu
    bootloader_init.c : bootloader_init
        // 清栈,_bss_end,_bss_start gcc连接时生成
        memset(&_bss_start, 0, (&_bss_end - &_bss_start) * sizeof(_bss_start))
        bootloader_main
            uart_console_configure
            typedef struct {
                uint8_t magic;        // 0xE9
                uint8_t segment_count;
                uint8_t spi_mode;     // QIO,QOUT,DIO,DOUT,FAST_READ,SLOW_READ
                uint8_t spi_speed: 4; // 40M,26M,20M,80(F)
                uint8_t spi_size: 4;  // 512K,256K,1M,2M,4M,2M_C1,4M_C1,8M(8),16,MAX
                uint32_t entry_addr;
            } __attribute__((packed))  esp_image_header_t;

            bootloader_flash.c : bootloader_flash_read(0x0000, esp_image_header_t)
                bootloader_flash_read_no_decrypt
                    SPIRead(src_addr, dest, size);
            // 打印编译信息
            core.a:phy_reg_default
            update_flash_config
                spi_flash.c : esp_spi_flash_init
                Cache_Read_Disable
            print_flash_info()
                // 打印Flash信息
    select_image
        bootloader_utility.c : bootloader_utility_load_partition_table
            bootloader_mmap
            esp_partition_table_basic_verify
            // 打印分区表信息
            bootloader_munmap
        selected_boot_partition
            bootloader_utility.c : bootloader_utility_get_selected_boot_partition
                bootloader_mmap
                bootloader_munmap
                ESP_LOGI(TAG, "Defaulting to factory image");
                ESP_LOGI(TAG, "No factory image, trying OTA 0");
        bootloader_utility.c : bootloader_utility_load_boot_image
            try_load_partition
                esp_image_load
                    bootloader_flash_read
                    verify_image_header
                    for process_segment 打印段信息
                    bootloader_sha256_finish
    bootloader_utility.c : bootloader_utility_load_image
        bootloader_utility_start_image(uint32_t entry_addr)
            void (*user_start)(size_t start_addr);
            bootloader_mmap(image_start, image_size);
            user_start = (void *)entry_addr;
            user_start(image_start);    // 跳转!!!!!!!!!!!!!!bootloader完成工作
```