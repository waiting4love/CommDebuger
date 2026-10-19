# 串口调试器 (Serial Port Debuger)

基于 WTL (Windows Template Library) 的轻量级串口调试工具，支持串口数据的收发、显示和文件操作。

## 功能特性

- **串口配置**：支持选择端口、波特率、数据位、停止位、校验位、流控方式
- **数据收发**：支持文本/HEX 格式的发送和接收显示
- **自动发送**：可设定间隔时间自动循环发送
- **文件操作**：支持从文件加载发送数据、接收数据保存到文件、自动保存
- **信号控制**：DTR / RTS / BREAK 信号控制，CTS / DSR / RING / RLSD 信号状态实时显示
- **错误检测**：实时显示通信错误（帧错误、奇偶校验错误、缓冲区溢出等）
- **高 DPI 支持**：Per-Monitor V2 DPI 感知，在不同缩放比例的显示器上正确显示
- **系统字体**：界面使用系统默认 UI 字体，收发文本框使用系统等宽字体 (Consolas)
- **32/64 位**：同时支持 x86 和 x64 架构构建

## 截图

程序主界面包含：
- 左侧：串口配置区（端口、波特率等）、收发计数、信号状态
- 右侧上方：发送面板（文本输入、HEX 发送、自动发送、文件加载）
- 右侧下方：接收面板（文本/HEX 显示、暂停、清空、保存）

## 构建方法

### 环境要求

- **编译器**：Visual Studio 2022 17.x 或 Visual Studio 2026 (v18) 及以上
- **Windows SDK**：10.0.19041.0 及以上
- **CMake**：3.20 及以上（推荐使用 Visual Studio 自带的 CMake）

### 使用 CMake 构建（推荐）

```bash
# 32 位构建
cmake -S . -B build-x86 -A Win32
cmake --build build-x86 --config Release

# 64 位构建
cmake -S . -B build-x64 -A x64
cmake --build build-x64 --config Release
```

构建产物位于：
- 32 位：`build-x86/Win32/Release/WTLCommDebuger.exe`
- 64 位：`build-x64/x64/Release/WTLCommDebuger.exe`

### 使用 Visual Studio 构建

直接打开 `WTLCommDebuger.sln`，选择配置（Debug/Release）和平台（Win32），然后生成解决方案即可。

## 下载

前往 [Releases 页面](../../releases) 下载预编译版本：
- `WTLCommDebuger-Release-Win32.zip` — 32 位版本
- `WTLCommDebuger-Release-x64.zip` — 64 位版本

## CI/CD

项目使用 GitHub Actions 自动构建并发布 Release。推送 `v*` 标签即可触发：

```bash
git tag v1.0.0
git push origin v1.0.0
```

也可在 GitHub 仓库的 Actions 页面手动触发构建。

## 技术栈

- **WTL 10.0**：Windows Template Library（已包含在 `WTLCommDebuger/Include/` 目录中）
- **ATL**：Active Template Library（随 Visual Studio 安装）
- **C++17**：使用 C++17 标准
- **静态链接 CRT**：使用 `/MT` 编译，无需额外运行时依赖

## 项目结构

```
CommDebuger/
├── CMakeLists.txt              # CMake 构建脚本
├── WTLCommDebuger.sln          # Visual Studio 解决方案文件
├── WTLCommDebuger/
│   ├── WTLCommDebuger.cpp      # 程序入口
│   ├── MainDlg.cpp/h           # 主对话框
│   ├── SendPanel.h             # 发送面板
│   ├── RecvPanel.h             # 接收面板
│   ├── Option.h                # 选项对话框
│   ├── Comm.h                  # 串口通信封装
│   ├── tmycommAPI.h            # 串口通信底层 API（线程读写策略）
│   ├── SerialPortInfo.cpp/h   # 串口枚举（SetupAPI）
│   ├── ColorStatic.h           # 自定义颜色控件
│   ├── SystemFonts.h           # 系统字体辅助
│   ├── stdafx.h/cpp            # 预编译头
│   ├── WTLCOMM.rc              # 资源文件（对话框、菜单、字符串表）
│   ├── WTLCommDebuger.manifest # 应用清单（DPI 感知、Common Controls v6）
│   ├── resource.h              # 资源头文件
│   └── Include/                # WTL 10.0 头文件
└── .github/workflows/
    └── build-release.yml       # GitHub Actions 构建发布工作流
```

## 许可证

[MIT License](LICENSE.txt)
