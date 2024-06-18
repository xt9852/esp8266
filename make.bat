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

:: xtensa-lx106-elf-ar cru libmain.a ./bootloader_start.o
:: xtensa-lx106-elf-gcc -std=gnu99 -Os -ggdb -Wno-frame-address -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -mlongcalls -nostdlib -Wall -Werror=all -Wno-error=unused-function -Wno-error=unused-but-set-variable -Wno-error=unused-variable -Wno-error=deprecated-declarations -Wextra -Wno-unused-parameter -Wno-sign-compare -Wno-old-style-declaration -I /root/esp/ESP8266_RTOS_SDK/components/esp8266/include -I /root/esp/ESP8266_RTOS_SDK/components/mbedtls/port/include/esp8266 -I /root/esp/ESP8266_RTOS_SDK/components/esp_common/include -D BOOTLOADER_BUILD=1 -I/root/esp/esp8266-test/code/build/bootloader/../include -DICACHE_FLASH -D__ESP_FILE__='"bootloader_start.c"' -DESP_PLATFORM -D IDF_VER=\"v3.4-dirty\" -MMD -MP -DESP_PLATFORM -D IDF_VER=\"v3.4-dirty\" -MMD -MP   -D_GNU_SOURCE -DGCC_NOT_5_2_0=1 -DMBEDTLS_CONFIG_FILE='"mbedtls/esp_config.h"' -DCONFIG_SSL_USING_MBEDTLS  -D_GNU_SOURCE -DGCC_NOT_5_2_0=1 -I /root/esp/ESP8266_RTOS_SDK/components/bootloader/subproject/main/include -I /root/esp/ESP8266_RTOS_SDK/components/bootloader_support/include -I /root/esp/ESP8266_RTOS_SDK/components/bootloader_support/include_priv -I /root/esp/ESP8266_RTOS_SDK/components/spi_flash/include -I /root/esp/ESP8266_RTOS_SDK/components/log/include -I /root/esp/ESP8266_RTOS_SDK/components/esp8266/include -I /root/esp/esp8266-test/code/build/bootloader/include  -I . -c /root/esp/ESP8266_RTOS_SDK/components/bootloader/subproject/main/bootloader_start.c -o bootloader_start.o
:: xtensa-lx106-elf-gcc -std=gnu99 -Os -ggdb -Wno-frame-address -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -mlongcalls -nostdlib -Wall -Werror=all -Wno-error=unused-function -Wno-error=unused-but-set-variable -Wno-error=unused-variable -Wno-error=deprecated-declarations -Wextra -Wno-unused-parameter -Wno-sign-compare -Wno-old-style-declaration -DICACHE_FLASH -D__ESP_FILE__='"esp_ota_ops.c"' -DAPP_OFFSET=0x10000 -DAPP_SIZE=0xf0000 -DESP_PLATFORM -D IDF_VER=\"v3.4-dirty\" -MMD -MP   -D_GNU_SOURCE -DGCC_NOT_5_2_0=1 -DMBEDTLS_CONFIG_FILE='"mbedtls/esp_config.h"' -DCONFIG_SSL_USING_MBEDTLS -I /root/esp/ESP8266_RTOS_SDK/components/app_update/include -I /root/esp/ESP8266_RTOS_SDK/components/bootloader_support/include -I /root/esp/ESP8266_RTOS_SDK/components/coap/include -I /root/esp/ESP8266_RTOS_SDK/components/console -I /root/esp/ESP8266_RTOS_SDK/components/esp-tls -I /root/esp/ESP8266_RTOS_SDK/components/esp-tls/private_include -I /root/esp/ESP8266_RTOS_SDK/components/esp-wolfssl/include -I /root/esp/ESP8266_RTOS_SDK/components/esp8266/include -I /root/esp/ESP8266_RTOS_SDK/components/esp8266/include -I /root/esp/ESP8266_RTOS_SDK/components/esp_common/include -I /root/esp/ESP8266_RTOS_SDK/components/esp_event/include -I /root/esp/ESP8266_RTOS_SDK/components/esp_gdbstub/include -I /root/esp/ESP8266_RTOS_SDK/components/esp_http_client/include -I /root/esp/ESP8266_RTOS_SDK/components/esp_http_server/include -I /root/esp/ESP8266_RTOS_SDK/components/esp_https_ota/include -I /root/esp/ESP8266_RTOS_SDK/components/esp_ringbuf/include -I /root/esp/ESP8266_RTOS_SDK/components/esp_ringbuf/include/freertos -I /root/esp/ESP8266_RTOS_SDK/components/fatfs/diskio -I /root/esp/ESP8266_RTOS_SDK/components/fatfs/vfs -I /root/esp/ESP8266_RTOS_SDK/components/fatfs/src -I /root/esp/ESP8266_RTOS_SDK/components/freemodbus/common/include -I /root/esp/ESP8266_RTOS_SDK/components/freertos/include -I /root/esp/ESP8266_RTOS_SDK/components/freertos/include -I /root/esp/ESP8266_RTOS_SDK/components/freertos/include/freertos -I /root/esp/ESP8266_RTOS_SDK/components/freertos/include/freertos/private -I /root/esp/ESP8266_RTOS_SDK/components/freertos/port/esp8266/include -I /root/esp/ESP8266_RTOS_SDK/components/freertos/port/esp8266/include/freertos -I /root/esp/ESP8266_RTOS_SDK/components/heap/include -I /root/esp/ESP8266_RTOS_SDK/components/heap/port/esp8266/include -I /root/esp/ESP8266_RTOS_SDK/components/http_parser/include -I /root/esp/ESP8266_RTOS_SDK/components/jsmn/include -I /root/esp/ESP8266_RTOS_SDK/components/json/cJSON -I /root/esp/ESP8266_RTOS_SDK/components/libsodium/libsodium/src/libsodium/include -I /root/esp/ESP8266_RTOS_SDK/components/libsodium/port_include -I /root/esp/ESP8266_RTOS_SDK/components/log/include -I /root/esp/ESP8266_RTOS_SDK/components/lwip/include/apps -I /root/esp/ESP8266_RTOS_SDK/components/lwip/include/apps/sntp -I /root/esp/ESP8266_RTOS_SDK/components/lwip/lwip/src/include -I /root/esp/ESP8266_RTOS_SDK/components/lwip/port/esp8266/include -I /root/esp/ESP8266_RTOS_SDK/components/lwip/port/esp8266/include/arch -I /root/esp/esp8266-test/code/main/include -I /root/esp/ESP8266_RTOS_SDK/components/mbedtls/port/include -I /root/esp/ESP8266_RTOS_SDK/components/mbedtls/mbedtls/include -I /root/esp/ESP8266_RTOS_SDK/components/mbedtls/port/include/esp8266 -I /root/esp/ESP8266_RTOS_SDK/components/mdns/include -I /root/esp/ESP8266_RTOS_SDK/components/mqtt/esp-mqtt/include -I /root/esp/ESP8266_RTOS_SDK/components/newlib/platform_include -I /root/esp/ESP8266_RTOS_SDK/components/nvs_flash/include -I /root/esp/ESP8266_RTOS_SDK/components/openssl/include -I /root/esp/ESP8266_RTOS_SDK/components/protobuf-c/protobuf-c -I /root/esp/ESP8266_RTOS_SDK/components/pthread/include -I /root/esp/ESP8266_RTOS_SDK/components/spi_flash/include -I /root/esp/ESP8266_RTOS_SDK/components/spi_ram/include -I /root/esp/ESP8266_RTOS_SDK/components/spiffs/include -I /root/esp/ESP8266_RTOS_SDK/components/tcp_transport/include -I /root/esp/ESP8266_RTOS_SDK/components/tcpip_adapter/include -I /root/esp/ESP8266_RTOS_SDK/components/tcpip_adapter/include -I /root/esp/ESP8266_RTOS_SDK/components/vfs/include -I /root/esp/ESP8266_RTOS_SDK/components/wear_levelling/include -I /root/esp/ESP8266_RTOS_SDK/components/wpa_supplicant/include -I /root/esp/ESP8266_RTOS_SDK/components/wpa_supplicant/port/include -I /root/esp/ESP8266_RTOS_SDK/components/wpa_supplicant/include/esp_supplicant -I /root/esp/esp8266-test/code/build/include  -I . -c /root/esp/ESP8266_RTOS_SDK/components/app_update/esp_ota_ops.c -o esp_ota_ops.o
:: xtensa-lx106-elf-c++ -std=gnu++11 -fno-rtti -Os -ggdb -Wno-frame-address -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -mlongcalls -nostdlib -Wall -Werror=all -Wno-error=unused-function -Wno-error=unused-but-set-variable -Wno-error=unused-variable -Wno-error=deprecated-declarations -Wextra -Wno-unused-parameter -Wno-sign-compare -fno-exceptions -D__ESP_FILE__='"nvs_types.cpp"' -DESP_PLATFORM -D IDF_VER=\"v3.4-dirty\" -MMD -MP   -D_GNU_SOURCE -DGCC_NOT_5_2_0=1 -DMBEDTLS_CONFIG_FILE='"mbedtls/esp_config.h"' -DCONFIG_SSL_USING_MBEDTLS -DNVS_CRC_HEADER_FILE=\"rom/crc.h\" -I /root/esp/ESP8266_RTOS_SDK/components/nvs_flash/include -I /root/esp/ESP8266_RTOS_SDK/components/app_update/include -I /root/esp/ESP8266_RTOS_SDK/components/bootloader_support/include -I /root/esp/ESP8266_RTOS_SDK/components/coap/include -I /root/esp/ESP8266_RTOS_SDK/components/console -I /root/esp/ESP8266_RTOS_SDK/components/esp-tls -I /root/esp/ESP8266_RTOS_SDK/components/esp-tls/private_include -I /root/esp/ESP8266_RTOS_SDK/components/esp-wolfssl/include -I /root/esp/ESP8266_RTOS_SDK/components/esp8266/include -I /root/esp/ESP8266_RTOS_SDK/components/esp8266/include -I /root/esp/ESP8266_RTOS_SDK/components/esp_common/include -I /root/esp/ESP8266_RTOS_SDK/components/esp_event/include -I /root/esp/ESP8266_RTOS_SDK/components/esp_gdbstub/include -I /root/esp/ESP8266_RTOS_SDK/components/esp_http_client/include -I /root/esp/ESP8266_RTOS_SDK/components/esp_http_server/include -I /root/esp/ESP8266_RTOS_SDK/components/esp_https_ota/include -I /root/esp/ESP8266_RTOS_SDK/components/esp_ringbuf/include -I /root/esp/ESP8266_RTOS_SDK/components/esp_ringbuf/include/freertos -I /root/esp/ESP8266_RTOS_SDK/components/fatfs/diskio -I /root/esp/ESP8266_RTOS_SDK/components/fatfs/vfs -I /root/esp/ESP8266_RTOS_SDK/components/fatfs/src -I /root/esp/ESP8266_RTOS_SDK/components/freemodbus/common/include -I /root/esp/ESP8266_RTOS_SDK/components/freertos/include -I /root/esp/ESP8266_RTOS_SDK/components/freertos/include -I /root/esp/ESP8266_RTOS_SDK/components/freertos/include/freertos -I /root/esp/ESP8266_RTOS_SDK/components/freertos/include/freertos/private -I /root/esp/ESP8266_RTOS_SDK/components/freertos/port/esp8266/include -I /root/esp/ESP8266_RTOS_SDK/components/freertos/port/esp8266/include/freertos -I /root/esp/ESP8266_RTOS_SDK/components/heap/include -I /root/esp/ESP8266_RTOS_SDK/components/heap/port/esp8266/include -I /root/esp/ESP8266_RTOS_SDK/components/http_parser/include -I /root/esp/ESP8266_RTOS_SDK/components/jsmn/include -I /root/esp/ESP8266_RTOS_SDK/components/json/cJSON -I /root/esp/ESP8266_RTOS_SDK/components/libsodium/libsodium/src/libsodium/include -I /root/esp/ESP8266_RTOS_SDK/components/libsodium/port_include -I /root/esp/ESP8266_RTOS_SDK/components/log/include -I /root/esp/ESP8266_RTOS_SDK/components/lwip/include/apps -I /root/esp/ESP8266_RTOS_SDK/components/lwip/include/apps/sntp -I /root/esp/ESP8266_RTOS_SDK/components/lwip/lwip/src/include -I /root/esp/ESP8266_RTOS_SDK/components/lwip/port/esp8266/include -I /root/esp/ESP8266_RTOS_SDK/components/lwip/port/esp8266/include/arch -I /root/esp/esp8266-test/code/main/include -I /root/esp/ESP8266_RTOS_SDK/components/mbedtls/port/include -I /root/esp/ESP8266_RTOS_SDK/components/mbedtls/mbedtls/include -I /root/esp/ESP8266_RTOS_SDK/components/mbedtls/port/include/esp8266 -I /root/esp/ESP8266_RTOS_SDK/components/mdns/include -I /root/esp/ESP8266_RTOS_SDK/components/mqtt/esp-mqtt/include -I /root/esp/ESP8266_RTOS_SDK/components/newlib/platform_include -I /root/esp/ESP8266_RTOS_SDK/components/openssl/include -I /root/esp/ESP8266_RTOS_SDK/components/protobuf-c/protobuf-c -I /root/esp/ESP8266_RTOS_SDK/components/pthread/include -I /root/esp/ESP8266_RTOS_SDK/components/spi_flash/include -I /root/esp/ESP8266_RTOS_SDK/components/spi_ram/include -I /root/esp/ESP8266_RTOS_SDK/components/spiffs/include -I /root/esp/ESP8266_RTOS_SDK/components/tcp_transport/include -I /root/esp/ESP8266_RTOS_SDK/components/tcpip_adapter/include -I /root/esp/ESP8266_RTOS_SDK/components/tcpip_adapter/include -I /root/esp/ESP8266_RTOS_SDK/components/vfs/include -I /root/esp/ESP8266_RTOS_SDK/components/wear_levelling/include -I /root/esp/ESP8266_RTOS_SDK/components/wpa_supplicant/include -I /root/esp/ESP8266_RTOS_SDK/components/wpa_supplicant/port/include -I /root/esp/ESP8266_RTOS_SDK/components/wpa_supplicant/include/esp_supplicant -I /root/esp/esp8266-test/code/build/include  -I src -c /root/esp/ESP8266_RTOS_SDK/components/nvs_flash/src/nvs_types.cpp -o src/nvs_types.o
:: xtensa-lx106-elf-gcc -nostdlib -u call_user_start_cpu0  -Wl,--gc-sections -Wl,-static -Wl,--start-group  -L/root/esp/esp8266-test/code/build/bootloader/main -lmain -L /root/esp/ESP8266_RTOS_SDK/components/esp8266/lib -lcore -L /root/esp/ESP8266_RTOS_SDK/components/bootloader/subproject/main -T esp8266.bootloader.ld -T /root/esp/ESP8266_RTOS_SDK/components/esp8266/ld/esp8266.rom.ld -T esp8266.bootloader.rom.ld -L/root/esp/esp8266-test/code/build/bootloader/bootloader_support -lbootloader_support -L/root/esp/esp8266-test/code/build/bootloader/spi_flash -lspi_flash -L/root/esp/esp8266-test/code/build/bootloader/log -llog -L/root/esp/esp8266-test/code/build/bootloader/esp8266 -lesp8266 -lgcc -lstdc++ -lgcov -Wl,--end-group -Wl,-EL -o /root/esp/esp8266-test/code/build/bootloader/bootloader.elf -Wl,-Map=/root/esp/esp8266-test/code/build/bootloader/bootloader.map
:: xtensa-lx106-elf-gcc -nostdlib -u call_user_start_cpu0  -Wl,--gc-sections -Wl,-static -Wl,--start-group  -L/root/esp/esp8266-test/code/build/app_update -lapp_update -u esp_app_desc -L/root/esp/esp8266-test/code/build/bootloader_support -lbootloader_support -L/root/esp/esp8266-test/code/build/coap -lcoap -L/root/esp/esp8266-test/code/build/console -lconsole -L/root/esp/esp8266-test/code/build/esp-tls -lesp-tls -L/root/esp/esp8266-test/code/build/esp-wolfssl -lesp-wolfssl -L/root/esp/esp8266-test/code/build/esp8266 -lesp8266 -L/root/esp/ESP8266_RTOS_SDK/components/esp8266/lib -lgcc -lhal -lcore -lnet80211 -lphy -lrtc -lclk -lpp -lsmartconfig -lssc -lespnow -L /root/esp/ESP8266_RTOS_SDK/components/esp8266/ld -T esp8266_out.ld -T /root/esp/esp8266-test/code/build/esp8266/esp8266.project.ld -Wl,--no-check-sections -u call_user_start -u g_esp_sys_info -T esp8266.rom.ld -T esp8266.peripherals.ld -L/root/esp/esp8266-test/code/build/esp_common -lesp_common -L/root/esp/esp8266-test/code/build/esp_event -lesp_event -L/root/esp/esp8266-test/code/build/esp_gdbstub -lesp_gdbstub -L/root/esp/esp8266-test/code/build/esp_http_client -lesp_http_client -L/root/esp/esp8266-test/code/build/esp_http_server -lesp_http_server -L/root/esp/esp8266-test/code/build/esp_https_ota -lesp_https_ota -L/root/esp/esp8266-test/code/build/esp_ringbuf -lesp_ringbuf -L/root/esp/esp8266-test/code/build/fatfs -lfatfs -L/root/esp/esp8266-test/code/build/freemodbus -lfreemodbus -L/root/esp/esp8266-test/code/build/freertos -lfreertos -L/root/esp/esp8266-test/code/build/heap -lheap -L/root/esp/esp8266-test/code/build/http_parser -lhttp_parser -L/root/esp/esp8266-test/code/build/jsmn -ljsmn -L/root/esp/esp8266-test/code/build/json -ljson -L/root/esp/esp8266-test/code/build/libsodium -llibsodium -L/root/esp/esp8266-test/code/build/log -llog -L/root/esp/esp8266-test/code/build/lwip -llwip -L/root/esp/esp8266-test/code/build/main -lmain -L/root/esp/esp8266-test/code/build/mbedtls -lmbedtls -L/root/esp/esp8266-test/code/build/mdns -lmdns -L/root/esp/esp8266-test/code/build/mqtt -lmqtt -L/root/esp/esp8266-test/code/build/newlib -lnewlib -lc_nano -lm -u __errno -L/root/esp/esp8266-test/code/build/nvs_flash -lnvs_flash -L/root/esp/esp8266-test/code/build/openssl -lopenssl -L/root/esp/esp8266-test/code/build/protobuf-c -lprotobuf-c -L/root/esp/esp8266-test/code/build/protocomm -lprotocomm -L/root/esp/esp8266-test/code/build/pthread -lpthread -u pthread_include_pthread_impl -u pthread_include_pthread_cond_impl -u pthread_include_pthread_local_storage_impl -L/root/esp/esp8266-test/code/build/spi_flash -lspi_flash -L/root/esp/esp8266-test/code/build/spi_ram -lspi_ram -L/root/esp/esp8266-test/code/build/spiffs -lspiffs -L/root/esp/esp8266-test/code/build/tcp_transport -ltcp_transport -L/root/esp/esp8266-test/code/build/tcpip_adapter -ltcpip_adapter -L/root/esp/esp8266-test/code/build/vfs -lvfs -L/root/esp/esp8266-test/code/build/wear_levelling -lwear_levelling -L/root/esp/esp8266-test/code/build/wifi_provisioning -lwifi_provisioning -L/root/esp/esp8266-test/code/build/wpa_supplicant -lwpa_supplicant -lgcc -lstdc++ -lgcov -Wl,--end-group -Wl,-EL -o /root/esp/esp8266-test/code/build/esp_test.elf -Wl,-Map=/root/esp/esp8266-test/code/build/esp_test.map


::'"spi_flash.c"'          -DPARTITION_QUEUE_HEADER=\"sys/queue.h\"
::'"spi_flash_raw.c"'      -DPARTITION_QUEUE_HEADER=\"sys/queue.h\"
::'"port.c"'               -DPARTITION_QUEUE_HEADER=\"sys/queue.h\"
::
::/root/esp/ESP8266_RTOS_SDK/components/bootloader/subproject/main/bootloader_start.c
::xtensa-lx106-elf-ar cru .\tmp\libmain.a .\tmp\bootloader_start.o
::
::/root/esp/ESP8266_RTOS_SDK/components/bootloader_support/src/bootloader_flash.c
::/root/esp/ESP8266_RTOS_SDK/components/bootloader_support/src/flash_qio_mode.c
::/root/esp/ESP8266_RTOS_SDK/components/bootloader_support/src/efuse.c
::/root/esp/ESP8266_RTOS_SDK/components/bootloader_support/src/bootloader_init.c
::/root/esp/ESP8266_RTOS_SDK/components/bootloader_support/src/bootloader_common.c
::/root/esp/ESP8266_RTOS_SDK/components/bootloader_support/src/secure_boot.c
::/root/esp/ESP8266_RTOS_SDK/components/bootloader_support/src/bootloader_clock.c
::/root/esp/ESP8266_RTOS_SDK/components/bootloader_support/src/bootloader_random.c
::/root/esp/ESP8266_RTOS_SDK/components/bootloader_support/src/secure_boot_signatures.c
::/root/esp/ESP8266_RTOS_SDK/components/bootloader_support/src/flash_partitions.c
::/root/esp/ESP8266_RTOS_SDK/components/bootloader_support/src/esp_image_format.c
::/root/esp/ESP8266_RTOS_SDK/components/bootloader_support/src/bootloader_sha.c
::/root/esp/ESP8266_RTOS_SDK/components/bootloader_support/src/flash_encrypt.c
::/root/esp/ESP8266_RTOS_SDK/components/bootloader_support/src/bootloader_utility.c
::xtensa-lx106-elf-ar cru .\tmp\libbootloader_support.a .\tmp\bootloader_flash.o .\tmp\flash_qio_mode.o .\tmp\efuse.o .\tmp\bootloader_init.o .\tmp\bootloader_common.o .\tmp\secure_boot.o .\tmp\bootloader_clock.o .\tmp\bootloader_random.o .\tmp\secure_boot_signatures.o .\tmp\flash_partitions.o .\tmp\esp_image_format.o .\tmp\bootloader_sha.o .\tmp\flash_encrypt.o .\tmp\bootloader_utility.o
::
::/root/esp/ESP8266_RTOS_SDK/components/spi_flash/src/spi_flash.c
::/root/esp/ESP8266_RTOS_SDK/components/spi_flash/src/spi_flash_raw.c
::/root/esp/ESP8266_RTOS_SDK/components/spi_flash/port/port.c
::xtensa-lx106-elf-ar cru .\tmp\libspi_flash.a .\tmp\spi_flash.o .\tmp\spi_flash_raw.o .\tmp\port.o
::
::/root/esp/ESP8266_RTOS_SDK/components/log/log.c -o log.o
::xtensa-lx106-elf-ar cru .\tmp\liblog.a .\tmp\log.o
::
::/root/esp/ESP8266_RTOS_SDK/components/esp8266/source/ets_printf.c
::/root/esp/ESP8266_RTOS_SDK/components/esp8266/source/crc.c
::/root/esp/ESP8266_RTOS_SDK/components/esp8266/source/esp_fast_boot.c
::xtensa-lx106-elf-ar cru .\tmp\libesp8266.a .\tmp\ets_printf.o .\tmp\crc.o
::
::xtensa-lx106-elf-gcc
:: -nostdlib -u call_user_start_cpu0  -Wl,--gc-sections -Wl,-static -Wl,--start-group
::-L.\tmp -lmain -lcore -lbootloader_support -lspi_flash -llog -lesp8266
::-lgcc -lstdc++ -lgcov -Wl,--end-group -Wl,-EL
::-o .\tmp\bootloader.elf -Wl,-Map=.\tmp\bootloader.map
::-L D:\4.backup\coding\ESP8266_RTOS_SDK-v3.4\components\bootloader\subproject\main
::-L D:\4.backup\coding\ESP8266_RTOS_SDK-v3.4\components\esp8266\ld
::-T esp8266.bootloader.ld -T esp8266.bootloader.rom.ld -T esp8266.rom.ld -LD:\4.backup\coding\ESP8266_RTOS_SDK-v3.4\components\esp8266\lib

:: -L/root/esp/esp8266-test/code/build/bootloader/main -lmain
:: -L/root/esp/ESP8266_RTOS_SDK/components/esp8266/lib -lcore
:: -L/root/esp/ESP8266_RTOS_SDK/components/bootloader/subproject/main -T esp8266.bootloader.ld -T /root/esp/ESP8266_RTOS_SDK/components/esp8266/ld/esp8266.rom.ld -T esp8266.bootloader.rom.ld
:: -L/root/esp/esp8266-test/code/build/bootloader/bootloader_support -lbootloader_support
:: -L/root/esp/esp8266-test/code/build/bootloader/spi_flash -lspi_flash
:: -L/root/esp/esp8266-test/code/build/bootloader/log -llog
:: -L/root/esp/esp8266-test/code/build/bootloader/esp8266 -lesp8266
:: -lgcc -lstdc++ -lgcov -Wl,--end-group -Wl,-EL
:: -o /root/esp/esp8266-test/code/build/bootloader/bootloader.elf -Wl,-Map=/root/esp/esp8266-test/code/build/bootloader/bootloader.map

:: -nostdlib -u call_user_start_cpu0  -Wl,--gc-sections -Wl,-static -Wl,--start-group
:: -L/root/esp/esp8266-test/code/build/app_update -lapp_update -u esp_app_desc
:: -L/root/esp/esp8266-test/code/build/bootloader_support -lbootloader_support
:: -L/root/esp/esp8266-test/code/build/coap -lcoap
:: -L/root/esp/esp8266-test/code/build/console -lconsole
:: -L/root/esp/esp8266-test/code/build/esp-tls -lesp-tls
:: -L/root/esp/esp8266-test/code/build/esp-wolfssl -lesp-wolfssl
:: -L/root/esp/esp8266-test/code/build/esp8266 -lesp8266
:: -L/root/esp/ESP8266_RTOS_SDK/components/esp8266/lib -lgcc -lhal -lcore -lnet80211 -lphy -lrtc -lclk -lpp -lsmartconfig -lssc -lespnow
:: -L/root/esp/ESP8266_RTOS_SDK/components/esp8266/ld -T esp8266_out.ld
:: -T /root/esp/esp8266-test/code/build/esp8266/esp8266.project.ld
:: -Wl,--no-check-sections -u call_user_start -u g_esp_sys_info -T esp8266.rom.ld -T esp8266.peripherals.ld
:: -L/root/esp/esp8266-test/code/build/esp_common -lesp_common
:: -L/root/esp/esp8266-test/code/build/esp_event -lesp_event
:: -L/root/esp/esp8266-test/code/build/esp_gdbstub -lesp_gdbstub
:: -L/root/esp/esp8266-test/code/build/esp_http_client -lesp_http_client
:: -L/root/esp/esp8266-test/code/build/esp_http_server -lesp_http_server
:: -L/root/esp/esp8266-test/code/build/esp_https_ota -lesp_https_ota
:: -L/root/esp/esp8266-test/code/build/esp_ringbuf -lesp_ringbuf
:: -L/root/esp/esp8266-test/code/build/fatfs -lfatfs
:: -L/root/esp/esp8266-test/code/build/freemodbus -lfreemodbus
:: -L/root/esp/esp8266-test/code/build/freertos -lfreertos
:: -L/root/esp/esp8266-test/code/build/heap -lheap
:: -L/root/esp/esp8266-test/code/build/http_parser -lhttp_parser
:: -L/root/esp/esp8266-test/code/build/jsmn -ljsmn
:: -L/root/esp/esp8266-test/code/build/json -ljson
:: -L/root/esp/esp8266-test/code/build/libsodium -llibsodium
:: -L/root/esp/esp8266-test/code/build/log -llog
:: -L/root/esp/esp8266-test/code/build/lwip -llwip
:: -L/root/esp/esp8266-test/code/build/main -lmain
:: -L/root/esp/esp8266-test/code/build/mbedtls -lmbedtls
:: -L/root/esp/esp8266-test/code/build/mdns -lmdns
:: -L/root/esp/esp8266-test/code/build/mqtt -lmqtt
:: -L/root/esp/esp8266-test/code/build/newlib -lnewlib -lc_nano -lm -u __errno
:: -L/root/esp/esp8266-test/code/build/nvs_flash -lnvs_flash
:: -L/root/esp/esp8266-test/code/build/openssl -lopenssl
:: -L/root/esp/esp8266-test/code/build/protobuf-c -lprotobuf-c
:: -L/root/esp/esp8266-test/code/build/protocomm -lprotocomm
:: -L/root/esp/esp8266-test/code/build/pthread -lpthread -u pthread_include_pthread_impl -u pthread_include_pthread_cond_impl -u pthread_include_pthread_local_storage_impl
:: -L/root/esp/esp8266-test/code/build/spi_flash -lspi_flash
:: -L/root/esp/esp8266-test/code/build/spi_ram -lspi_ram
:: -L/root/esp/esp8266-test/code/build/spiffs -lspiffs
:: -L/root/esp/esp8266-test/code/build/tcp_transport -ltcp_transport
:: -L/root/esp/esp8266-test/code/build/tcpip_adapter -ltcpip_adapter
:: -L/root/esp/esp8266-test/code/build/vfs -lvfs
:: -L/root/esp/esp8266-test/code/build/wear_levelling -lwear_levelling
:: -L/root/esp/esp8266-test/code/build/wifi_provisioning -lwifi_provisioning
:: -L/root/esp/esp8266-test/code/build/wpa_supplicant -lwpa_supplicant
:: -lgcc -lstdc++ -lgcov -Wl,--end-group -Wl,-EL
:: -o /root/esp/esp8266-test/code/build/esp_test.elf -Wl,-Map=/root/esp/esp8266-test/code/build/esp_test.map


::python /root/esp/ESP8266_RTOS_SDK/tools/ldgen/ldgen.py
::--input /root/esp/ESP8266_RTOS_SDK/components/esp8266/ld/esp8266.project.ld.in
::--config        /root/esp/esp8266-test/code/sdkconfig
::--fragments            /root/esp/ESP8266_RTOS_SDK/components/esp8266/ld/esp8266_fragments.lf
::/root/esp/ESP8266_RTOS_SDK/components/esp8266/ld/esp8266_bss_fragments.lf
::/root/esp/ESP8266_RTOS_SDK/components/esp8266/linker.lf
::/root/esp/ESP8266_RTOS_SDK/components/freertos/linker.lf
::/root/esp/ESP8266_RTOS_SDK/components/log/linker.lf
::/root/esp/ESP8266_RTOS_SDK/components/lwip/linker.lf
::/root/esp/ESP8266_RTOS_SDK/components/spi_flash/linker.lf
::--libraries-file /root/esp/esp8266-test/code/build/ldgen_libraries
::--output        /root/esp/esp8266-test/code/build/esp8266/esp8266.project.ld
::--kconfig       /root/esp/ESP8266_RTOS_SDK/Kconfig
::--env           "COMPONENT_KCONFIGS=/root/esp/ESP8266_RTOS_SDK/components/app_update/Kconfig
::/root/esp/ESP8266_RTOS_SDK/components/coap/Kconfig
::/root/esp/ESP8266_RTOS_SDK/components/esp-tls/Kconfig
::/root/esp/ESP8266_RTOS_SDK/components/esp-wolfssl/Kconfig
::/root/esp/ESP8266_RTOS_SDK/components/esp8266/Kconfig
::/root/esp/ESP8266_RTOS_SDK/components/esp_common/Kconfig
::/root/esp/ESP8266_RTOS_SDK/components/esp_event/Kconfig
::/root/esp/ESP8266_RTOS_SDK/components/esp_gdbstub/Kconfig
::/root/esp/ESP8266_RTOS_SDK/components/esp_http_client/Kconfig
::/root/esp/ESP8266_RTOS_SDK/components/esp_http_server/Kconfig
::/root/esp/ESP8266_RTOS_SDK/components/esp_https_ota/Kconfig
::/root/esp/ESP8266_RTOS_SDK/components/fatfs/Kconfig
::/root/esp/ESP8266_RTOS_SDK/components/freemodbus/Kconfig
::/root/esp/ESP8266_RTOS_SDK/components/freertos/Kconfig
::/root/esp/ESP8266_RTOS_SDK/components/heap/Kconfig
::/root/esp/ESP8266_RTOS_SDK/components/libsodium/Kconfig
::/root/esp/ESP8266_RTOS_SDK/components/log/Kconfig
::/root/esp/ESP8266_RTOS_SDK/components/lwip/Kconfig
::/root/esp/ESP8266_RTOS_SDK/components/mbedtls/Kconfig
::/root/esp/ESP8266_RTOS_SDK/components/mdns/Kconfig
::/root/esp/ESP8266_RTOS_SDK/components/mqtt/Kconfig
::/root/esp/ESP8266_RTOS_SDK/components/newlib/Kconfig
::/root/esp/ESP8266_RTOS_SDK/components/openssl/Kconfig
::/root/esp/ESP8266_RTOS_SDK/components/pthread/Kconfig
::/root/esp/ESP8266_RTOS_SDK/components/spiffs/Kconfig
::/root/esp/ESP8266_RTOS_SDK/components/tcpip_adapter/Kconfig
::/root/esp/ESP8266_RTOS_SDK/components/vfs/Kconfig
::/root/esp/ESP8266_RTOS_SDK/components/wear_levelling/Kconfig
::/root/esp/ESP8266_RTOS_SDK/components/wifi_provisioning/Kconfig
::/root/esp/ESP8266_RTOS_SDK/components/wpa_supplicant/Kconfig"
::--env           "COMPONENT_KCONFIGS_PROJBUILD= /root/esp/ESP8266_RTOS_SDK/components/bootloader/Kconfig.projbuild
::/root/esp/ESP8266_RTOS_SDK/components/esptool_py/Kconfig.projbuild
::/root/esp/ESP8266_RTOS_SDK/components/partition_table/Kconfig.projbuild            "
::
::--env           "IDF_CMAKE=n"
::--objdump           xtensa-lx106-elf-objdump

:: 不显示命令字符串,设置屏幕大小,字体颜色
@echo off && mode con cols=100 lines=1000 && color 0A

::-----------------------------------------------------

:: 外部参数
::echo ARG=%*

::if "%1" == "" (echo "Don't have {app|bootloader|clean}" && pause && exit)

::-----------------------------------------------------

set SDK=D:\4.backup\coding\ESP8266_RTOS_SDK-v3.4\components

:: BOOTLOADER OR APP
set TYPE=BOOTLOADER

:: 是否调试:y,n
set DEBUG=n

:: 源文件路径,可多个路径
set SRC_APP=.\code\main
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
set LF_APP=
set LF_BOOT=-L%TMP% -L%SDK%\esp8266\lib -lcore -L%SDK%\bootloader\subproject\main -L%SDK%\esp8266\ld ^
-T esp8266.bootloader.ld -T esp8266.bootloader.rom.ld -T esp8266.rom.ld ^
-o .\tmp\bootloader.elf -Wl,-Map=.\tmp\bootloader.map

::-----------------------------------------------------
:: 编译工具

set AR=xtensa-lx106-elf-ar.exe
set CC=xtensa-lx106-elf-gcc.exe
set CXX=xtensa-lx106-elf-c++
set TOOL=D:\4.backup\coding\xtensa-lx106-elf\bin

::-----------------------------------------------------
:: 设置编译参数

:: 延时变量扩展,如果不设置for的变量将不起作用
setLocal EnableDelayedExpansion

:: 编译参数
set CF=%CF% -std=gnu99 -Os -ggdb -Wno-frame-address -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -mlongcalls -nostdlib ^
-Wall -Werror=all -Wno-error=unused-function -Wno-error=unused-but-set-variable -Wno-error=unused-variable -Wno-error=deprecated-declarations ^
-Wextra -Wno-unused-parameter -Wno-sign-compare -Wno-old-style-declaration ^
-DICACHE_FLASH -DESP_PLATFORM -D_GNU_SOURCE -DGCC_NOT_5_2_0=1 ^
-DIDF_VER=\"v3.4-dirty\" -DMBEDTLS_CONFIG_FILE='"mbedtls/esp_config.h"' -DCONFIG_SSL_USING_MBEDTLS ^
-MMD -MP

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
   set CF=!CF! -I%%D\include
)

::-----------------------------------------------------
:: 设置连接参数

set LF=%LF% -nostdlib -u call_user_start_cpu0  -Wl,--gc-sections -Wl,-static -Wl,--start-group -lgcc -lstdc++ -lgcov -Wl,--end-group -Wl,-EL

::-----------------------------------------------------
::执行命令

if "%1" == "clean" (rd /q/s "%TMP%" 2>nul & echo rd /q/s "%TMP%" & exit /b 0) else if not exist "%TMP%" (mkdir "%TMP%")

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
                set EXE=%CC% %CF% -D__ESP_FILE__='"%%~nF%%~xF"' -c %%F -o %TMP%\%%~nF.o
            ) else (
                set EXE=%CXX% %CF% -D__ESP_FILE__='"%%~nF%%~xF"' -c %%F -o %TMP%\%%~nF.o
            )

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

%CC% %OBJ% %LF% > %TMP%\error.txt 2>&1

for %%I in (%TMP%\error.txt) do (set SIZE=%%~zI)

if !SIZE! == 0 (
    echo success
) else (
    type %TMP%\error.txt
    exit /b -1
)

::xtensa-lx106-elf-ar cru .\tmp\libmain.a .\tmp\bootloader_start.o
::xtensa-lx106-elf-ar cru .\tmp\libbootloader_support.a .\tmp\bootloader_flash.o .\tmp\flash_qio_mode.o .\tmp\efuse.o .\tmp\bootloader_init.o .\tmp\bootloader_common.o .\tmp\secure_boot.o .\tmp\bootloader_clock.o .\tmp\bootloader_random.o .\tmp\secure_boot_signatures.o .\tmp\flash_partitions.o .\tmp\esp_image_format.o .\tmp\bootloader_sha.o .\tmp\flash_encrypt.o .\tmp\bootloader_utility.o
::xtensa-lx106-elf-ar cru .\tmp\libspi_flash.a .\tmp\spi_flash.o .\tmp\spi_flash_raw.o .\tmp\port.o
::xtensa-lx106-elf-ar cru .\tmp\liblog.a .\tmp\log.o
::xtensa-lx106-elf-ar cru .\tmp\libesp8266.a .\tmp\ets_printf.o .\tmp\crc.o
::xtensa-lx106-elf-gcc -nostdlib -u call_user_start_cpu0  -Wl,--gc-sections -Wl,-static -Wl,--start-group
::-L D:\4.backup\coding\ESP8266_RTOS_SDK-v3.4\components\esp8266\lib -lcore
::-L.\tmp -lmain -lbootloader_support -lspi_flash -llog -lesp8266
::-lgcc -lstdc++ -lgcov -Wl,--end-group -Wl,-EL
::-o .\tmp\bootloader.elf -Wl,-Map=.\tmp\bootloader.map
::-L D:\4.backup\coding\ESP8266_RTOS_SDK-v3.4\components\bootloader\subproject\main
::-L D:\4.backup\coding\ESP8266_RTOS_SDK-v3.4\components\esp8266\ld -T esp8266.bootloader.ld -T esp8266.bootloader.rom.ld -T esp8266.rom.ld

