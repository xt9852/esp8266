## 搭建Win7开发环境
> * 硬件: 使用nodeMCU模块，此模块使用的是esp12f芯片
> * 软件: 使用RTOS的SDK进行开发
> * 1.下载msys32工具链: https://dl.espressif.com/dl/esp32_win32_msys2_environment_and_toolchain-20181001.zip
> * 2.下载交叉编译工具: https://dl.espressif.com/dl/xtensa-lx106-elf-gcc8_4_0-esp-2020r3-win32.zip
> * 3.解压文件2个文件
> * 4.进入目录: esp32_win32_msys2_environment_and_toolchain-20181001\msys32\etc\profile.d
> * 6.编辑文件: esp32_toolchain.sh
> 文件中已经配置好了esp32的交叉编译环境变量,需增加esp8266和SDK
> 例:export PATH="$PATH:/opt/xtensa-lx106-elf/bin"
> 例:export IDF_PATH="/D:/4.backup/coding/ESP8266_RTOS_SDK-v3.4"
> * 4.执行命令: mingw32.exe，弹出的窗口为MINGW32环境

## 编译错误
> * 1.错误: Makefile:8: /make/project.mk: 没有那个文件或目录，解决方法:ESP8266_RTOS_SDK-v3.4目录下多了一层ESP8266_RTOS_SDK,将这一层去除即可
> * 2.错误: The following Python requirements are not satisfied: click>=5.0 pyelftools>=0.22，解决方法: python -m pip install --user -r $IDF_PATH/requirements.txt

python -m pip install --upgrade pip
pip config set global.index-url https://pypi.tuna.tsinghua.edu.cn/simple
python -m pip install -i https://pypi.tuna.tsinghua.edu.cn/simple --upgrade pip
python -m pip install -i https://pypi.tuna.tsinghua.edu.cn/simple --upgrade setuptools

ModuleNotFoundError: No module named '_sysconfigdata__mingw32_nt-6'           mingw32\lib\python2.7\_sysconfigdata.py


## 搭建Win10+Ubuntu20.04开发环境
> * 硬件: 使用nodeMCU模块，此模块使用的是esp12f芯片
> * 软件: 使用RTOS的SDK进行开发
> * 1.进入目录: cd ~
> * 2.创建目录: mkdir esp
> * 3.进入目录: cd esp
> * 4.下载交叉编译工具: wget https://dl.espressif.com/dl/xtensa-lx106-elf-gcc8_4_0-esp-2020r3-linux-amd64.tar.gz
> * 5.解压包: tar -xf xtensa-lx106-elf-gcc8_4_0-esp-2020r3-linux-amd64.tar.gz
> * 6.进入目录: cd xtensa-lx106-elf
> * 7.设置参数: export PATH=$PATH:~/esp/xtensa-lx106-elf/bin
> * 8.下载RTOS: git clone https://github.com/espressif/ESP8266_RTOS_SDK.git或wget http://github.com/espressif/ESP8266_RTOS_SDK/releases/download/v3.4/ESP8266_RTOS_SDK-v3.4.zip
> * 9.设置参数: export IDF_PATH=~/esp/ESP8266_RTOS_SDK

## 编译HelloWorld
> * 1.进入目录: cd ~/esp/ESP8266_RTOS_SDK/examples/get-started/hello_world
> * 2.执行命令: make menuconfig #修改串口号，速率
> * 3.执行命令: make
> * 4.执行命令: make flash
> * 5.执行命令: make monitor    #默认波特率74880，8数据位，1停止位

## 编译文件系统
> * 复制SDK目录examples\storage进行修改
> * partitions_example.csv做为分区使用的，如果不使用文件系统可以删除，如果使用需要执行make menuconfig中配置存储项目中将配置文件名添加上
> * 修改Make,CMakeLists,main\CMakeLists这几个文件中的项目名称
> * 修改main\spiffs_example_main.c文件同项目名
> * 进行编译，下载程序到模块，运行程序

## 编译错误
> * 1.错误 make: Command not found，解决方法: apt install make
> * 2.错误 cc: Command not found，解决方法: apt install gcc
> * 3.错误 flex: Command not found，解决方法: apt install flex
> * 4.错误 curses.h: No such file or directory，解决方法: apt install libncurses5-dev
> * 5.错误 bison: Command not found，解决方法: apt install bison
> * 6.错误 gperf: not found，解决方法: apt install gperf
> * 7.错误 python: No such file or directory，解决方法: 如果没安装python则 apt install python3，如果已经安装python则 ln -s /usr/bin/python3.8 /usr/bin/python(需要修改版本号)
> * 8.错误 The following Python requirements are not satisfied，解决方法: /usr/bin/python -m pip install --user -r /root/esp/ESP8266_RTOS_SDK/requirements.txt
> * 9.错误 /usr/bin/python: No module named pip，解决方法: apt install python3-pip 然后重新执行错误8的命令
> * 10.错误 GnuTLS recv error (-110): The TLS connection was non-properly terminated，解决方法: 重构git安装包，将gnutls替换为openssl

## Ubuntut重构git安装包，将gnutls替换为openssl
> * 1.安装软件包
** sudo apt-get -y install build-essential
** sudo apt-get -y install fakeroot
** sudo apt-get -y install dpkg-dev
** sudo apt-get -y install libcurl4-openssl-dev
** sudo apt-get -y build-dep git
> * 2.创建目录
**  mkdir git-openssl
** cd git-openssl
> * 3.获取 git 源码
** apt-get source git
** cd git-2.25.1 版本号可能不一致
> * 4.修改git安装包配置文件
** 将 libcurl4-gnutls-dev 替换为 libcurl4-openssl-dev
** sed -i -e 's/libcurl4-gnutls-dev/libcurl4-openssl-dev/g' ./debian/control
** 删除 TEST=test，缩短安装包构建时间
** sed -i -- '/TEST\s*=\s*test/d' ./debian/rules
> * 5.构建安装包
** sudo dpkg-buildpackage -rfakeroot -b
** 错误 dpkg-checkbuilddeps: error: Unmet build dependencies: libcurl4-openssl-dev，解决方法: sudo apt-get upgrade l libcurl4-openssl-dev
> * 6.安装git
** cd ../
** sudo dpkg -i git_2.17.1-1ubuntu0.8_amd64.deb 版本号可能不一致
> * 7.清理
** cd ../../
** sudo rm -rf git-openssl

## make menuconfig菜单
> * SDK tool configuration  --->
> * Bootloader config  --->
> * Serial flasher config  --->
> * Example Configuration  --->
> * Partition Table  --->
> * Compiler options  --->
> * Component config  --->
