# TongDaXin Trade Server

## 概述

本程序实现了一个服务器，用来接收交易请求，并将请求转发给trade.dll，使其可以通过rest api 被其它程序调用。

```
注意，注意，注意: 本程序不提供trade.dll文件，仅仅调用该dll库进行交易，并将其封装成rest api使用，
我们也不会提供任何trade.dll文件给使用者（关于trade.dll，请自行百度\谷歌）
如需技术支持，可以想办法联系我。
```


结构图

```
+--------------------------------------------------------------+
|              Your Quant or Other System                      |
|                                                              |
|                                                              |
|  +----------------------+ +----------------------------------+
|  |                      | |                                 ||
|  |  Python client Api   | |  Other Language Apis            ||
|  |  pytdx               | |                                 ||
|  |                      | |                                 ||
|  |                      | |                                 ||
|  +----------------------+ +----------------------------------+
|                                                              |
|                                                              |
+----------------------------+---^-----------------------------+
                             |   |
+----------------------------v---+-----------------------------+
|                                                              |
|          TongDaXin Trade SerVer Listening : 10092 port       |
|                               +                              |
|                               |      +---------------------+ |
|                               |      |                     | |
|                               +----->+      Trade.dll      | |
|                                      |                     | |
|                                      |                     | |
|                            +         +---------------------+ |
+--------------------------------^-----------------------------+
                             |   |
+----------------------------v---+-----------------------------+
|                                                              |
|                  Tong Da Xin Trade Servers                   |
|                                                              |
|                                                              |
|                                                              |
+--------------------------------------------------------------+

```

## 构建

本程序使用QT5.9.1开发，使用`restbed`作为web server实现，使用`jsonformoderncpp`作为json序列化和反序列化工具，使用`conan`作为c++依赖库的管理。
请使用兼容`c++11`的编译器编译（这里我使用的vs2015的编译器，理论上mingw应该也可以）

### 构建过程

1. 安装python
2. 安装Conan
3. 安装Qt 并配置VS2015 C++ Compiler
4. 配置git
5. 安装cmake
6. 配置git,mingz,cmake命令在系统path中可以调用
7. 安装msys2 (openssl编译需要）
8. 在msys2中安装perl, cmake
```bash
pacman -S perl
pacman -S make
```
9. 配置conan.conf
```
[general]
bash_path="c:\msys32\bin\bash"
..
[settings_defaults]
arch=x86
compiler=Visual Studio
compiler.version=14
compiler.runtime=MTd
build_type=Debug
os=Windows
```
10. 在项目目录下执行
```
conan install --build zlib --buildOpenSSL --build asio --build restbed
```

然后打开QT Creator编译即可。

## 配置

我们可以通过配置文件来配置系统，配置文件可以在如下两个地方被放置

1. [程序运行目录]/TdxTradeServer.ini
2. ~/TdxTradeServer/TdxTradeServer.ini   ， ~ 代表当前用户的主目录

配置文件Demo

```
bind=10.11.5.175    ; 绑定的ip地址，默认是127.0.0.1
port=10092          ; 绑定的端口
trade_dll_path=D:\\trade_rainx.dll   ;一份可以使用的trade.dll文件
transport_enc_key=4f1cf3fec4c84c84   ; 可选， aes加密秘钥
transport_enc_iv=0c78abc083b011e7    ; 可选， aes加密iv
```
注意，后面的加密选项为可选，如果`transport_enc_key` 或者 `transport_enc_iv` 不提供的话，将使用明文和`client api`通讯，请注意和`client api`保持一致。
如果您跨机器调用接口，建议使用加密功能，并且，请生成随即密钥和iv, 注意，我们这里`key`和`iv`必须是`16`个字节
