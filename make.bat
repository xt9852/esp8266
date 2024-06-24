::-----------------------------------------------------
:: Copyright:   XT Tech. Co., Ltd.
:: File:        make.bat
:: Author:      �ź���
:: Version:     2.0.0
:: Encode:      GB2312
:: Date:        2024-06-14
:: Description: ����xtensa-lx106-elf����ESP8266����
::              ��������sdkconfig.h
::-----------------------------------------------------

:: ����ʾ�����ַ���,������Ļ��С,������ɫ
@echo off && mode con cols=100 lines=1000 && color 0A

::�л���UTF-8����ҳ,�������б������ϵ���Ҽ���ѡ��"����"->"����"���������޸�ΪTrue Type����"Lucida Console"
::chcp 65001

:: ��ʱ������չ,���������for�ı�������������
setLocal EnableDelayedExpansion

::-----------------------------------------------------

:: SDKĿ¼
set SDK=D:\4.backup\coding\ESP8266_RTOS_SDK-v3.4\components

:: BOOTLOADER OR APP
set TYPE=APP

:: Դ�ļ�·��,�ɶ��·��
set SRC_APP=.\code\test

set SRC_BOOT=^
%SDK%\bootloader\subproject\main ^
%SDK%\bootloader_support\src ^
%SDK%\spi_flash\src ^
%SDK%\spi_flash\port ^
%SDK%\esp8266\source ^
%SDK%\log

:: �ų����ļ�,Ŀ¼
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

:: ��ʱ�ļ�·��
set TMP=.\tmp

:: �������
set CF=-I.\conf

:: ���Ӳ���
set LF_APP=-T.\code\test\esp8266.app.ld -o%TMP%\app.elf -Map=%TMP%\app.map -L%SDK%\esp8266\lib

set LF_BOOT=-L%SDK%\esp8266\lib -lcore -L%SDK%\bootloader\subproject\main -L%SDK%\esp8266\ld ^
-T esp8266.bootloader.ld -T esp8266.bootloader.rom.ld -T esp8266.rom.ld ^
-o .\tmp\bootloader.elf -Wl,-Map=.\tmp\bootloader.map

::-----------------------------------------------------
::ִ������

if "%1" == "clean" (rd /q/s "%TMP%" 2>nul & echo rd /q/s "%TMP%" & exit /b 0) else if not exist "%TMP%" (mkdir "%TMP%")

::-----------------------------------------------------
:: ���ñ������

:: �������
:: -c                               ֻ���벻����
:: -D                               ������
:: -E                               ִ��Ԥ����
:: -I                               ָ��ͷ�ļ�Ŀ¼
:: -fdata-sections                  ��ÿ������,���ڶ�����section
:: -ffunction-sections              ��ÿ������,���ڶ�����section
:: -fPIC                            ������λ���޹ش���,���ڶ�̬��
:: -g                               ���ɵ�����Ϣ,����GDB����
:: -ggdb                            ΪGDB����������Ϣ
:: -ggdb[level]                     �趨�������ֵȼ��ĵ�����Ϣ�� level Ϊ 1-3��1 ���٣�3 ���
:: -l                               Ҫ���ӵĿ�
:: -L                               ���ӿ��Ŀ¼
:: -mlong-calls                     ����ͬһ��Դ�ļ��еĺ������õ�ͱ��������ľ�����Գ���256K֮Զ
:: -MMD                             ������#include������ͷ�ļ�����ɵ��ļ�֮���������ϵ, ��Ϣ������뵽.d���ļ�����
:: -MP                              ���ɴ���ͷ�ļ�������ϵ��.d�ļ�
:: -o                               ָ������ļ���
:: -O0                              ��ֹ�Ż�
:: -O1,-O                           �����Ż�
:: -O2                              �������Ż�,���ܾ��ռ任ʱ�䡣
:: -O3                              �������Ż�,���ܿռ任ʱ�䡣
:: -Os                              Ϊ�����С�����Ż�,���ɾ�����С�Ļ�����
:: -Og                              ����ȫ���Ż�,Ϊ���ܹ����ɸ��õĵ�����Ϣ
:: -Ofast                           ʹ����߼�����Ż�ѡ�����������,���������ϸ�����Ա�׼����߳����ִ���ٶ�
:: -shared                          ������̬���ӿ�
:: -static                          ������̬���ӿ�
:: -std=gnu99                       �������Եı�׼:C89,C94,C99,C11,C++11,C++14,C++20,��:gnu++14
:: -U                               ȡ����
:: -w                               �رձ���ʱ�ľ���
:: -W                               ��ʾ��������Ϊ����ִ���ľ���
:: -Wall                            ��ʾ���о���
:: -Werror=all                      ���о��涼��Ϊ����
:: -Wextra                          ���ö���ľ���,����:δʹ�ñ�����δʹ�õĺ���,Ǳ�ڵĻ�����������ڴ�й©,���ܻᵼ��δ������Ϊ������
:: -Wno-error=                      �ر��ض����͵ľ���
:: -Wno-frame-address               ����ʾû��main�����ľ���
:: -Wno-sign-compare                ����ʾ�з������޷��ű����ȽϾ���
:: -Wno-unused-variable             ����ʾδʹ�õı�������
:: -Wno-unused-parameter            ����ʾδʹ�õĲ�������
:: -Wno-unused-function             ����ʾδʹ�õĺ�������
:: -Wno-unused-but-set-variable     ����ʾ�Ѹ�ֵ��δʹ�õı�������
:: -Wno-unused-private-field        ����ʾδʹ�õ���˽�г�Ա����
:: -Wno-unused-label                ����ʾδʹ�õ���ת��Ǿ���
:: -Wno-deprecated-declarations     ����ʾ��ʱ�Ĳ��Ƽ�ʹ�õĺ�����������ʱ

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

:: ͷ�ļ�·��
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

:: SDK���������·��
for /d %%D in (%SDK%\*) do (
   set INCLUDE=!INCLUDE! -I%%D\include
)

::-----------------------------------------------------
:: �������Ӳ���
:: EL                       С�˼ܹ�
:: gc-sections              ȥ�����õ�section, �������-fdata-sections,-ffunction-sections
:: start-group,end-group    �ð����������߼�ľ�̬��˳���������
:: -u                       ȡ�����Ŷ���
::
:: -u call_user_start_cpu0 --start-group -lgcc -lstdc++ -lgcov --end-group

set LF=-EL -nostdlib --gc-sections %LF%

::-----------------------------------------------------
:: ���빤��

set AR=xtensa-lx106-elf-ar.exe
set CC=xtensa-lx106-elf-gcc.exe
set XX=xtensa-lx106-elf-c++.exe
set LD=xtensa-lx106-elf-ld.exe
set TOOL=D:\4.backup\coding\xtensa-lx106-elf\bin

::-----------------------------------------------------
:: �����ļ�

set PATH=%PATH%;%TOOL%

set OBJ=

:: �����ļ�,���ԴĿ¼
for %%D in (%SRC%) do (
    for /f %%F in ('dir /s/b %%D') do (
        set PROC=0
        if %%~xF == .c     (set PROC=1)
        if %%~xF == .cpp   (set PROC=1)

        :: �Ƿ���Ҫ�ų�,RELATIVE���·��
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
