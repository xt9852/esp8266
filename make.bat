::-----------------------------------------------------
:: File:        make.bat
:: Author:      �ź���
:: Version:     2.0.0
:: Encode:      GB2312
:: Date:        2024-06-14
:: Description: ����xtensa-lx106-elf����ESP8266����
::              ��������sdkconfig.h
::-----------------------------------------------------

:: ����ʾ�����ַ���
@echo off

:: UTF-8����ҳ,�����б������Ҽ�,����/����,Lucida Console
@chcp 65001

:: ������ɫ
@color 0A

:: ���û�������С,���ô��ڴ�С,�����б������Ҽ�,����/����,���ڴ�С
@mode con COLS=100 LINES=1000

:: ��ʱ������չ,���������for�ı�������������
setLocal EnableDelayedExpansion

::----------------------------------------------------------------------------------------------------------

:: SDKĿ¼
set SDK=D:\4.backup\coding\ESP8266_RTOS_SDK-v3.4\components

:: ������빤��Ŀ¼
set TOOL=D:\4.backup\coding\xtensa-lx106-elf

::----------------------------------------------------------------------------------------------------------

:: BOOTLOADER | APP
set TYPE=APP

:: Դ�ļ�·��,�ɶ��·��
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

:: �ų����ļ�,Ŀ¼
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

:: ��ʱĿ¼
set TMP=.\tmp

:: ���Ŀ¼
set OUT=.\out

:: �������
set CF=-I.\conf

:: ���Ӳ��� -lrtc -lclk -lpp -lsmartconfig -lssc -lespnow 
set APP_LF=-L%SDK%\esp8266\lib -lgcc -lcore -lcore_dbg -lnet80211 -lphy -lpp -lpp_dbg -Tesp8266.rom.ld ^
-L%TOOL%\xtensa-lx106-elf\lib -lc_nano -lm ^
-T.\code\test\esp8266.app.ld -o%TMP%\app.elf -Map=%TMP%\app.map -L%SDK%\esp8266\lib

set BOOT_LF=-L%SDK%\esp8266\lib -lcore ^
-L%SDK%\esp8266\ld -Tesp8266.rom.ld ^
-L%SDK%\bootloader\subproject\main -Tesp8266.bootloader.rom.ld -Tesp8266.bootloader.ld ^
-o%TMP%\bootloader.elf -Map=%TMP%\bootloader.map

::----------------------------------------------------------------------------------------------------------
::ִ������

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
:: -Wno-implicit-fallthrough        ����ʾswitch��case��û��break
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

:: ͷ�ļ�·��
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

:: ����SDK���������·��
for /d %%D in (%SDK%\*) do (
   set INCLUDE=!INCLUDE! -I%%D\include
)

::----------------------------------------------------------------------------------------------------------
:: �������Ӳ���
:: EL                       С�˼ܹ�
:: gc-sections              ȥ�����õ�section, �������-fdata-sections,-ffunction-sections
:: start-group,end-group    �ð����������߼�ľ�̬��˳���������
:: -u                       ȡ�����Ŷ���-D����ĺ�
:: -u call_user_start_cpu0 --start-group -lgcc -lstdc++ -lgcov --end-group

set LF=-EL -nostdlib --gc-sections %LF%

::----------------------------------------------------------------------------------------------------------
:: ���빤��

set AR=xtensa-lx106-elf-ar.exe
set CC=xtensa-lx106-elf-gcc.exe
set XX=xtensa-lx106-elf-c++.exe
set LD=xtensa-lx106-elf-ld.exe

::----------------------------------------------------------------------------------------------------------
:: �����ļ�

set PATH=%PATH%;%TOOL%\bin

set OBJ=

:: �����ļ�,���ԴĿ¼
:: %%~f �ļ�ȫ·����
:: %%~nx �ļ���+�ļ���չ��
for %%D in (%SRC%) do (
    for /f %%F in ('dir /s/b %%D') do (
        set PROC=0

        :: �Ƚ���չ��
        if "%%~xF" == ".c"   (set PROC=1)
        if "%%~xF" == ".cpp" (set PROC=1)
        if "%%~xF" == ".S"   (set PROC=1)

        if !PROC! == 1 (
            set FULLNAME=%%F

            :: ���·����,%%~fDȫ·����
            set RELATIVE=!FULLNAME:%%~fD\=!

            :: ȥ���ļ���,%%~nxF�ļ���
            set MIDDLE=!RELATIVE:%%~nxF=!

            for %%i in (%EXC%) do (
                :: �ų��ļ�
                if %%i == %%~nxF (set PROC=0)

                :: �ų�Ŀ¼
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

                :: ����������ʱerrorlevelҲΪ0
                !EXE! %INCLUDE% > %TMP%\error.txt 2>&1

                :: ͨ���ļ���С�ж��Ƿ�ɹ�
                for %%I in (%TMP%\error.txt) do (if %%~zI == 0 (echo %%~nxF) else (type %TMP%\error.txt && exit /b))
            )
        )
    )
)

::----------------------------------------------------------------------------------------------------------
:: �����ļ�

::echo %LD% %OBJ% %LF%

%LD% %OBJ% %LF% > %TMP%\error.txt 2>&1

:: ͨ���ļ���С�ж��Ƿ�ɹ�type %TMP%\error.txt &&
for %%I in (%TMP%\error.txt) do (if %%~zI == 0 (echo seccess) else (exit /b))

::----------------------------------------------------------------------------------------------------------
:: ��¼�ļ�

if %TYPE% == BOOTLOADER (
    %OUT%\esp_tool.exe bin -o=%OUT%\1.bootloader.bin -i=%TMP%\bootloader.elf -version=1
    if "%2" == "rom" (%OUT%\esp_tool.exe rom -i=%OUT%\1.bootloader.bin -addr=0x0 -com=5)
) else (
    %OUT%\esp_tool.exe bin -o=%OUT%\4.app.bin -i=%TMP%\app.elf -version=3
    if "%2" == "rom" (%OUT%\esp_tool.exe rom -i=%OUT%\4.app.bin -addr=0x8000 -com=5)
)
