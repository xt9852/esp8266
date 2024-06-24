::-----------------------------------------------------
:: Copyright:   XT Tech. Co., Ltd.
:: File:        make.bat
:: Author:      张海涛
:: Version:     2.0.0
:: Encode:      GB2312
:: Date:        2024-06-14
:: Description: 调用xtensa-lx106-elf编译ESP8266工程
::              参数配置sdkconfig.h
::-----------------------------------------------------

:: 不显示命令字符串,设置屏幕大小,字体颜色
@echo off && mode con cols=100 lines=1000 && color 0A

::切换到UTF-8代码页,在命令行标题栏上点击右键，选择"属性"->"字体"，将字体修改为True Type字体"Lucida Console"
::chcp 65001

:: 延时变量扩展,如果不设置for的变量将不起作用
setLocal EnableDelayedExpansion

::-----------------------------------------------------

:: SDK目录
set SDK=D:\4.backup\coding\ESP8266_RTOS_SDK-v3.4\components

:: BOOTLOADER OR APP
set TYPE=APP

:: 源文件路径,可多个路径
set SRC_APP=.\code\test

set SRC_BOOT=^
%SDK%\bootloader\subproject\main ^
%SDK%\bootloader_support\src ^
%SDK%\spi_flash\src ^
%SDK%\spi_flash\port ^
%SDK%\esp8266\source ^
%SDK%\log

:: 排除的文件,目录
set EXC_APP=

set EXC_BOOT=partition.c ^
test_log_set_tag.c ^
backtrace.c ^
chip_boot.c ^
esp_fsleep.c ^
esp_sleep.c ^
esp_timer.c ^
esp_wifi.c ^
esp_wifi_os_adapter.c ^
hw_random.c ^
phy_init.c ^
reset_reason.c ^
rom.c ^
smartconfig.c ^
smartconfig_ack.c ^
startup.c ^
system_api.c ^
task_wdt.c

:: 临时文件路径
set TMP=.\tmp

:: 编译参数
set CF=-I.\conf

:: 链接参数
set LF_APP=-T.\code\test\esp8266.app.ld -o%TMP%\app.elf -Map=%TMP%\app.map -L%SDK%\esp8266\lib

set LF_BOOT=-L%SDK%\esp8266\lib -lcore -L%SDK%\bootloader\subproject\main -L%SDK%\esp8266\ld ^
-T esp8266.bootloader.ld -T esp8266.bootloader.rom.ld -T esp8266.rom.ld ^
-o .\tmp\bootloader.elf -Wl,-Map=.\tmp\bootloader.map

::-----------------------------------------------------
::执行命令

if "%1" == "clean" (rd /q/s "%TMP%" 2>nul & echo rd /q/s "%TMP%" & exit /b 0) else if not exist "%TMP%" (mkdir "%TMP%")

::-----------------------------------------------------
:: 设置编译参数

:: 编译参数
:: -c                               只编译不链接
:: -D                               创建宏
:: -E                               执行预处理
:: -I                               指定头文件目录
:: -fdata-sections                  将每个数据,放在独立的section
:: -ffunction-sections              将每个函段,放在独立的section
:: -fPIC                            产生与位置无关代码,用于动态库
:: -g                               生成调试信息,用于GDB调试
:: -ggdb                            为GDB产生调试信息
:: -ggdb[level]                     设定产生何种等级的调试信息， level 为 1-3，1 最少，3 最多
:: -l                               要链接的库
:: -L                               链接库的目录
:: -mlong-calls                     允许同一个源文件中的函数调用点和被调函数的距离可以超过256K之远
:: -MMD                             忽略由#include包含的头文件所造成的文件之间的依赖关系, 信息输出导入到.d的文件里面
:: -MP                              生成带有头文件依赖关系的.d文件
:: -o                               指定输出文件名
:: -O0                              禁止优化
:: -O1,-O                           尝试优化
:: -O2                              尽可能优化,但拒绝空间换时间。
:: -O3                              尽可能优化,接受空间换时间。
:: -Os                              为代码大小进行优化,生成尽量短小的机器码
:: -Og                              启用全局优化,为了能够生成更好的调试信息
:: -Ofast                           使用最高级别的优化选项来编译代码,可以无视严格的语言标准以提高程序的执行速度
:: -shared                          创建动态链接库
:: -static                          创建静态链接库
:: -std=gnu99                       编译语言的标准:C89,C94,C99,C11,C++11,C++14,C++20,例:gnu++14
:: -U                               取消宏
:: -w                               关闭编译时的警告
:: -W                               显示编译器认为会出现错误的警告
:: -Wall                            显示所有警告
:: -Werror=all                      所有警告都视为错误
:: -Wextra                          启用额外的警告,包括:未使用变量和未使用的函数,潜在的缓冲区溢出和内存泄漏,可能会导致未定义行为的语句等
:: -Wno-error=                      关闭特定类型的警告
:: -Wno-frame-address               不显示没有main函数的警告
:: -Wno-sign-compare                不显示有符号与无符号变量比较警告
:: -Wno-unused-variable             不显示未使用的变量警告
:: -Wno-unused-parameter            不显示未使用的参数警告
:: -Wno-unused-function             不显示未使用的函数警告
:: -Wno-unused-but-set-variable     不显示已赋值但未使用的变量警告
:: -Wno-unused-private-field        不显示未使用的类私有成员警告
:: -Wno-unused-label                不显示未使用的跳转标记警告
:: -Wno-deprecated-declarations     不显示过时的不推荐使用的函数或者特性时

:: -Wno-error=unused-function -Wno-error=unused-variable -Wno-error=unused-but-set-variable -Wno-error=deprecated-declarations
:: -Wno-frame-address -Wno-old-style-declaration

set CF=%CF% -std=gnu99 -Os -MMD -MP -mlongcalls ^
-fdata-sections -ffunction-sections -fstrict-volatile-bitfields ^
-Wall -Werror=all -Wextra ^
-Wno-sign-compare -Wno-unused-parameter ^
-D_GNU_SOURCE -DGCC_NOT_5_2_0=1 ^
-DESP_PLATFORM -DICACHE_FLASH -DIDF_VER=\"v3.4-dirty\" ^
-DMBEDTLS_CONFIG_FILE='"mbedtls/esp_config.h"' -DCONFIG_SSL_USING_MBEDTLS

:: BOOTLOADER OR APP
if %TYPE% == BOOTLOADER (
    set CF=%CF% -DBOOTLOADER_BUILD=1
    set LF=%LF_BOOT%
    set SRC=%SRC_BOOT%
    set EXC=%EXC_BOOT%
) else (
    set CF=%CF% -DAPP_OFFSET=0x10000 -DAPP_SIZE=0xf0000
    set LF=%LF_APP%
    set SRC=%SRC_APP%
    set EXC=%EXC_APP%
)

:: 头文件路径
set CF=%CF% ^
-I%SDK%\bootloader_support\include_priv ^
-I%SDK%\freertos\port\esp8266\include ^
-I%SDK%\freertos\port\esp8266\include\freertos ^
-I%SDK%\freertos\include\freertos\private ^
-I%SDK%\heap\port\esp8266\include ^
-I%SDK%\mqtt\esp-mqtt\include ^
-I%SDK%\json\cJSON ^
-I%SDK%\lwip\include\apps ^
-I%SDK%\lwip\include\apps\sntp ^
-I%SDK%\lwip\lwip\src\include ^
-I%SDK%\lwip\port\esp8266\include ^
-I%SDK%\newlib\platform_include

:: SDK内所有组件路径
for /d %%D in (%SDK%\*) do (
   set INCLUDE=!INCLUDE! -I%%D\include
)

::-----------------------------------------------------
:: 设置连接参数
:: EL                       小端架构
:: gc-sections              去掉不用的section, 编译需加-fdata-sections,-ffunction-sections
:: start-group,end-group    让包含在这两者间的静态库顺序可以随意
:: -u                       取消符号定义
::
:: -u call_user_start_cpu0 --start-group -lgcc -lstdc++ -lgcov --end-group

set LF=-EL -nostdlib --gc-sections %LF%

::-----------------------------------------------------
:: 编译工具

set AR=xtensa-lx106-elf-ar.exe
set CC=xtensa-lx106-elf-gcc.exe
set XX=xtensa-lx106-elf-c++.exe
set LD=xtensa-lx106-elf-ld.exe
set TOOL=D:\4.backup\coding\xtensa-lx106-elf\bin

::-----------------------------------------------------
:: 编译文件

set PATH=%PATH%;%TOOL%

set OBJ=

:: 编译文件,多个源目录
for %%D in (%SRC%) do (
    for /f %%F in ('dir /s/b %%D') do (
        set PROC=0
        if %%~xF == .c     (set PROC=1)
        if %%~xF == .cpp   (set PROC=1)

        :: 是否需要排除,RELATIVE相对路径
        if !PROC! == 1 (
            set FULLNAME=%%F
            set RELATIVE=!FULLNAME:%%D\=!
            for /f "delims=\" %%E in ('echo !RELATIVE!') do (set DIR=%%E)
            echo %EXC% | findstr !DIR! > nul && (set PROC=0)
            echo %EXC% | findstr %%~nxF > nul && (set PROC=0)
        )

        if !PROC! == 1 (
            set OBJ=!OBJ! %TMP%\%%~nF.o
            if %%~xF == .c (
                set EXE=%CC%  %CF% %INCLUDE% -D__ESP_FILE__='"%%~nF%%~xF"' -c %%F -o %TMP%\%%~nF.o
            ) else (
                set EXE=%CXX% %CF% %INCLUDE% -D__ESP_FILE__='"%%~nF%%~xF"' -c %%F -o %TMP%\%%~nF.o
            )

            echo %CC%  %CF% -D__ESP_FILE__='"%%~nF%%~xF"' -c %%F -o %TMP%\%%~nF.o

            !EXE! > %TMP%\error.txt 2>&1

            for %%I in (%TMP%\error.txt) do (set SIZE=%%~zI)

            if !SIZE! == 0 (
                echo %%~nF%%~xF
            ) else (
                type %TMP%\error.txt
                exit /b -1
            )
       )
    )
)

echo %LD% %OBJ% %LF%

%LD% %OBJ% %LF% > %TMP%\error.txt 2>&1

for %%I in (%TMP%\error.txt) do (set SIZE=%%~zI)

if !SIZE! == 0 (
    echo success
) else (
    type %TMP%\error.txt
    exit /b -1
)

.\tmp\esp_tool.exe bin -o=tmp\app.bin -i=tmp\app.elf -version=1
.\tmp\esp_tool.exe rom -i=tmp\app.bin -addr=0x0 -com=5
