# PAPP - PortableApp

## 功能

这是一个**轻量级**的 *Windows 便捷应用平台*及创建工具，使用 C++ & Python *(pocketpy)* 编写，具有多种应用环境。

1. 可以用于**隔离**不同应用的环境，避免应用之间的冲突。
2. 可以用于**应用双开**，例如：网易云音乐，QQ 音乐，等。
3. 可以用于创建自定义的应用启动**Python脚本**，例如：对运行环境复杂的应用可以创建一个启动脚本，用于启动应用。

## 特征

- 创建便携应用
- 支持自定义应用图标
- 支持自定义应用名称
- 支持自定义应用启动参数
- 支持自定义应用启动目录
- 支持自定义应用环境变量

## 本地使用

为了完整使用本项目构建便捷应用，您需要确保系统中安装了 [**CMake**](https://cmake.org/download/)、[**Python**](https://www.python.org/downloads/) (3.9+) 及编译环境 (如 MSVC、MinGW) 并添加到环境变量中。

在 tools 目录下有一个 create_app.py 脚本，您可以使用它来创建便捷应用。

## 开发

### 构建

```sh
mkdir build
cd build
cmake ..
cmake --build . --config Debug
```

## 第三方依赖

- *部分依赖已经内置*

[**pocketpy**: portable python 3.x interpreter](https://github.com/pocketpy/pocketpy)

[**Detours**: Microsoft Research Detours Package](https://github.com/microsoft/Detours)

[**icoextract**: extract icon from exe file](https://github.com/jlu5/icoextract)
