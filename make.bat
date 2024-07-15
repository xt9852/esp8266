::-----------------------------------------------------
:: File:        make.bat
:: Author:      张海涛
:: Version:     2.0.0
:: Encode:      GB2312
:: Date:        2024-06-14
:: Description: 调用xtensa-lx106-elf编译ESP8266工程
::              参数配置sdkconfig.h
::-----------------------------------------------------

:: 不显示命令字符串
@echo off

:: UTF-8代码页,命令行标题栏右键,属性/字体,Lucida Console
@chcp 65001

:: 字体颜色
@color 0A

:: 设置缓冲区大小,设置窗口大小,命令行标题栏右键,属性/布局,窗口大小
@mode con COLS=100 LINES=1000

:: 延时变量扩展,如果不设置for的变量将不起作用
setLocal EnableDelayedExpansion

::----------------------------------------------------------------------------------------------------------

:: SDK目录
set SDK=D:\4.backup\coding\ESP8266_RTOS_SDK-v3.4\components

:: 交叉编译工具目录
set TOOL=D:\4.backup\coding\xtensa-lx106-elf

::----------------------------------------------------------------------------------------------------------

:: BOOTLOADER | APP
set TYPE=APP

:: 源文件路径,可多个路径
set APP_SRC=.\code\main ^
%SDK%\app_update ^
%SDK%\bootloader_support\src ^
%SDK%\log ^
%SDK%\esp8266\driver ^
%SDK%\esp8266\source ^
%SDK%\esp_common\src ^
%SDK%\esp_event ^
%SDK%\freertos\freertos ^
%SDK%\freertos\port\esp8266 ^
%SDK%\heap\src ^
%SDK%\http_parser\src ^
%SDK%\lwip\apps ^
%SDK%\lwip\lwip\src\api ^
%SDK%\lwip\lwip\src\core ^
%SDK%\lwip\lwip\src\netif ^
%SDK%\lwip\lwip\src\apps\sntp ^
%SDK%\lwip\lwip\src\apps\netbiosns ^
%SDK%\lwip\port ^
%SDK%\json\cJSON ^
%SDK%\mqtt\esp-mqtt ^
%SDK%\newlib\src ^
%SDK%\nvs_flash\src ^
%SDK%\spi_flash\src ^
%SDK%\tcpip_adapter ^
%SDK%\tcp_transport ^
%SDK%\wpa_supplicant\src

:: lwip\apps\sntp\sntp.c=>sntp_.c
:: wpa_supplicant\src\crypto\bignum.c=>bignum_.c

:: 排除的文件,目录
set APP_EXC=test\ ^
fuzzing\ ^
tests\ ^
apps\http\ ^
app_update\test\ ^
tls\ ^
test.c ^
ethernetif.c ^
esp_common.c ^
esp_scan.c ^
rrm.c ^
bignum_.c

set BOOT_SRC=^
%SDK%\bootloader\subproject\main ^
%SDK%\bootloader_support\src ^
%SDK%\spi_flash\src ^
%SDK%\spi_flash\port ^
%SDK%\esp8266\source ^
%SDK%\log

set BOOT_EXC=partition.c ^
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

:: 临时目录
set TMP=.\tmp

:: 输出目录
set OUT=.\out

:: 编译参数
set CF=-I.\conf

:: 链接参数 -lrtc -lclk -lpp -lsmartconfig -lssc -lespnow 
set APP_LF=-L%SDK%\esp8266\lib -lgcc -lcore -lcore_dbg -lnet80211 -lphy -lpp -lpp_dbg -Tesp8266.rom.ld ^
-L%TOOL%\xtensa-lx106-elf\lib -lc_nano -lm ^
-T.\code\test\esp8266.app.ld -o%TMP%\app.elf -Map=%TMP%\app.map -L%SDK%\esp8266\lib

set BOOT_LF=-L%SDK%\esp8266\lib -lcore ^
-L%SDK%\esp8266\ld -Tesp8266.rom.ld ^
-L%SDK%\bootloader\subproject\main -Tesp8266.bootloader.rom.ld -Tesp8266.bootloader.ld ^
-o%TMP%\bootloader.elf -Map=%TMP%\bootloader.map

::----------------------------------------------------------------------------------------------------------
::执行命令

echo ARG=%*

if "%1" == "app" (
    set TYPE=APP
) else if "%1" == "bootloader" (
    set TYPE=BOOTLOADER
) else if "%1" == "clean" (
    rd /q/s "%TMP%" 2>nul
    exit /b
) else (
    echo "Don't have {app|bootloader|clean}"
    pause
    exit /b
)

if not exist "%TMP%" (mkdir "%TMP%")
if not exist "%OUT%" (mkdir "%OUT%")

::----------------------------------------------------------------------------------------------------------
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
:: -Wno-implicit-fallthrough        不显示switch的case里没有break
:: -Wno-error=unused-function -Wno-error=unused-variable -Wno-error=unused-but-set-variable -Wno-error=deprecated-declarations
:: -Wno-frame-address -Wno-old-style-declaration

set CF=%CF% -Os -MMD -MP -mlongcalls ^
-fdata-sections -ffunction-sections -fstrict-volatile-bitfields ^
-Wall -Werror=all -Wextra ^
-Wno-sign-compare -Wno-unused-parameter -Wno-implicit-fallthrough -Wno-cast-function-type -Wno-unused-variable ^
-DGCC_NOT_5_2_0=1 -D_GNU_SOURCE ^
-DESP_PLATFORM -DIDF_VER=\"v3.4-dirty\" ^
-DPROJECT_VER=\"96823e3-dirty\" -DPROJECT_NAME=\"esp_app\" ^
-DMBEDTLS_CONFIG_FILE=\"mbedtls/esp_config.h\" -DCONFIG_SSL_USING_MBEDTLS ^
-DICACHE_FLASH ^
-D__ets__ ^
-DESP_SUPPLICANT ^
-DIEEE8021X_EAPOL ^
-DEAP_PEER_METHOD ^
-DEAP_MSCHAPv2 ^
-DEAP_TTLS ^
-DEAP_TLS ^
-DEAP_PEAP ^
-DUSE_WPA2_TASK ^
-DCONFIG_WPS2 ^
-DCONFIG_WPS_PIN ^
-DUSE_WPS_TASK ^
-DESPRESSIF_USE ^
-DESP8266_WORKAROUND ^
-DCONFIG_ECC ^
-DCONFIG_IEEE80211W ^
-DCONFIG_WPA3_SAE ^
-DCONFIG_DPP ^
-DCONFIG_WNM

:: BOOTLOADER OR APP
if %TYPE% == BOOTLOADER (
    set CF=%CF% -DBOOTLOADER_BUILD=1
    set LF=%BOOT_LF%
    set SRC=%BOOT_SRC%
    set EXC=%BOOT_EXC%
) else (
    set CF=%CF%
    set LF=%APP_LF%
    set SRC=%APP_SRC%
    set EXC=%APP_EXC%
)

:: 头文件路径
set INCLUDE=^
-I%SDK%\bootloader_support\include_priv ^
-I%SDK%\esp8266\include\driver ^
-I%SDK%\esp_event\private_include ^
-I%SDK%\esp-tls ^
-I%SDK%\esp_ringbuf\include\freertos ^
-I%SDK%\freertos\port\esp8266\include ^
-I%SDK%\freertos\port\esp8266\include\freertos ^
-I%SDK%\freertos\include\freertos ^
-I%SDK%\freertos\include\freertos\private ^
-I%SDK%\heap\port\esp8266\include ^
-I%SDK%\json\cJSON ^
-I%SDK%\lwip\include\apps ^
-I%SDK%\lwip\include\apps\sntp ^
-I%SDK%\lwip\lwip\src\include ^
-I%SDK%\lwip\port\esp8266\include ^
-I%SDK%\mbedtls\mbedtls\include ^
-I%SDK%\mbedtls\port\include ^
-I%SDK%\mbedtls\port\include\esp8266 ^
-I%SDK%\mqtt\esp-mqtt\include ^
-I%SDK%\mqtt\esp-mqtt\lib\include ^
-I%SDK%\newlib\platform_include ^
-I%SDK%\tcp_transport\private_include ^
-I%SDK%\wpa_supplicant\src ^
-I%SDK%\wpa_supplicant\port\include ^
-I%SDK%\wpa_supplicant\include\esp_supplicant

:: 查找SDK内所有组件路径
for /d %%D in (%SDK%\*) do (
   set INCLUDE=!INCLUDE! -I%%D\include
)

::----------------------------------------------------------------------------------------------------------
:: 设置连接参数
:: EL                       小端架构
:: gc-sections              去掉不用的section, 编译需加-fdata-sections,-ffunction-sections
:: start-group,end-group    让包含在这两者间的静态库顺序可以随意
:: -u                       取消符号定义-D定义的宏
:: -u call_user_start_cpu0 --start-group -lgcc -lstdc++ -lgcov --end-group

set LF=-EL -nostdlib --gc-sections %LF%

::----------------------------------------------------------------------------------------------------------
:: 编译工具

set AR=xtensa-lx106-elf-ar.exe
set CC=xtensa-lx106-elf-gcc.exe
set XX=xtensa-lx106-elf-c++.exe
set LD=xtensa-lx106-elf-ld.exe

::----------------------------------------------------------------------------------------------------------
:: 编译文件

set PATH=%PATH%;%TOOL%\bin

set OBJ=

:: 编译文件,多个源目录
:: %%~f 文件全路径名
:: %%~nx 文件名+文件扩展名
for %%D in (%SRC%) do (
    for /f %%F in ('dir /s/b %%D') do (
        set PROC=0

        :: 比较扩展名
        if "%%~xF" == ".c"   (set PROC=1)
        if "%%~xF" == ".cpp" (set PROC=1)
        if "%%~xF" == ".S"   (set PROC=1)

        if !PROC! == 1 (
            set FULLNAME=%%F

            :: 相对路径名,%%~fD全路径名
            set RELATIVE=!FULLNAME:%%~fD\=!

            :: 去掉文件名,%%~nxF文件名
            set MIDDLE=!RELATIVE:%%~nxF=!

            for %%i in (%EXC%) do (
                :: 排除文件
                if %%i == %%~nxF (set PROC=0)

                :: 排除目录
                echo !MIDDLE! | findstr /b %%i > nul && (set PROC=0)
            )
        )

        if !PROC! == 1 (
            set OBJ=!OBJ! %TMP%\%%~nF.o

            if exist "%TMP%\%%~nF.o" (
                echo %%~nxF
            ) else (
                set CF=%CF% -D__ESP_FILE__=\"%%~nxF\" -c %%F -o %TMP%\%%~nF.o

                if "%%~xF" == ".c" (set EXE=%CC% -std=gnu99 -Wno-old-style-declaration !CF!) else (set EXE=%XX% -std=gnu++11 !CF!)

                :: 编译器错误时errorlevel也为0
                !EXE! %INCLUDE% > %TMP%\error.txt 2>&1

                :: 通过文件大小判断是否成功
                for %%I in (%TMP%\error.txt) do (if %%~zI == 0 (echo %%~nxF) else (type %TMP%\error.txt && exit /b))
            )
        )
    )
)

::----------------------------------------------------------------------------------------------------------
:: 链接文件

::echo %LD% %OBJ% %LF%

%LD% %OBJ% %LF% > %TMP%\error.txt 2>&1

:: 通过文件大小判断是否成功type %TMP%\error.txt &&
for %%I in (%TMP%\error.txt) do (if %%~zI == 0 (echo seccess) else (exit /b))

::----------------------------------------------------------------------------------------------------------
:: 烧录文件

if %TYPE% == BOOTLOADER (
    %OUT%\esp_tool.exe bin -o=%OUT%\1.bootloader.bin -i=%TMP%\bootloader.elf -version=1
    if "%2" == "rom" (%OUT%\esp_tool.exe rom -i=%OUT%\1.bootloader.bin -addr=0x0 -com=5)
) else (
    %OUT%\esp_tool.exe bin -o=%OUT%\4.app.bin -i=%TMP%\app.elf -version=3
    if "%2" == "rom" (%OUT%\esp_tool.exe rom -i=%OUT%\4.app.bin -addr=0x8000 -com=5)
)
