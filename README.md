# LA32R-Simulator
本项目是基于龙芯架构32位精简版（以下简称`LA32R`）指令集的C++命令行模拟器，其主要功能包括：

* 模拟执行符合`LA32R`非浮点指令集约定的二进制程序，并提供了双线程外设模拟，支持指令踪迹
* 将执行的程序指令流反汇编为`LA32R`汇编代码（无需依赖`LLVM API`）
* 为基于`Verilator`的处理器仿真调试系统提供`Difftest`



## 项目构建方法

### 构建环境要求

当前已知该项目能够成功运行的环境如下：

* 操作系统：**Ubuntu22.04 LTS**
* g++版本：**11.4.0**

### 依赖项目安装

在`Ubuntu22.04`系统下，请使用如下命令安装本项目依赖：

```bash
$ sudo apt install gcc g++ gdb
$ sudo apt install make
$ sudo apt install libsdl2-dev
```

### 编译为可执行模拟器程序

在本项目根目录下，运行如下命令：

```bash
make
```

即可在`build`文件夹下生成可执行文件`simulator`

如果使用如下命令：

```bash
make run
```

则将会在编译完成后，自动运行模拟器程序。模拟器执行的默认程序为内嵌于模拟器内部的四条`LA32R`指令：

```assembly
1c000000: 1c00000c      pcaddu12i    $r12, 0
1c000004: 29804180      st.w         $r0 , $r12, 16
1c000008: 28804184      ld.w         $r4 , $r12, 16
1c00000c: 002a0000      break        0
```

如果需要运行自定义的程序，请确保程序满足如下条件：

* 程序所在文件为`Linux`二进制可执行文件
* 程序中所有指令满足`LA32R`非浮点指令集手册`（v1.0.3）`中的约定
* 程序入口地址为`0x1c000000`

### 编译为`Difftest`调试参考库文件

在本项目根目录下，运行如下命令：
```bash
make SHARE=1
```

即可在`build`文件夹下生成库文件`simulator-so`



## 项目结构概述

本项目的代码结构基本如下：
```tex
.
├── Makefile
├── README.md
├── include				
│   ├── arch.h				// 约定了LA32R架构的处理器状态、程序起始地址、内存大小等信息
│   ├── command.h			// 调试命令接口
│   ├── common.h			// 通用库，提供了基本的C++库文件支持，以及位运算操作、取变量指定位操作的宏定义
│   ├── config.h			// 项目配置信息
│   ├── cpu.h				// 处理器状态接口
│   ├── decode.h			// LA32R指令译码执行接口
│   ├── device.h			// 各类外设访问接口
│   ├── difftest.h			// difftest 对外接口
│   ├── disasm.h			// 反汇编程序接口
│   ├── mmio.h				// MMIO访存接口
│   ├── monitor.h			// 程序监控函数接口
│   ├── paddr.h				// 处理器访存接口
│   └── utils.h				// 实用调试工具宏定义
├── script
│   └── build.mk
└── src
    ├── cpu
    │   ├── cpu-exec.cc		// 模拟器指令执行驱动函数、调试信息输出
    │   └── ref.cc			// 为Difftest提供操控模拟运行的API
    ├── device
    │   ├── device.cc		// 模拟外设更新逻辑
    │   ├── keyboard.cc		// 模拟键盘外设
    │   ├── mmio.cc			// 提供MMIO访存支持
    │   ├── rtc.cc			// 模拟实时时钟外设	
    │   ├── serial.cc		// 模拟串口外设	
    │   └── vga.cc			// 模拟显示器外设
    ├── loongarch
    │   ├── decode.cc		// LA32R指令译码执行
    │   └── reg.cc			// LA32R架构寄存器标准
    ├── memory
    │   └── paddr.cc		// 模拟内存，提供给处理器访存支持
    ├── monitor
    │   ├── command
    │   │   └── command.cc	// 模拟器调试命令
    │   └── monitor.cc		// 模拟器监控运行系统
    ├── sim-main.cc			// 主函数
    └── utils
        └── disasm.cc		// LA32R二进制指令码反汇编逻辑
```



## 模拟器程序标准

### 模拟内存

* 内存起始地址为`0x1c00000`，该地址被映射到模拟内存（`uint8_t`型数组）的起始位置
* 内存大小为`0x8000000`，超出`0x1c000000-0x24000000`范围的访存请求会被认为**不在物理内存中**

### 模拟外设

* 当前实现的外设及其地址映射对应表如下：

  | 外设名称                         | 映射地址范围            |
  | -------------------------------- | ----------------------- |
  | 串口（serial-port）              | 0xa00003f8 ~ 0xa00003ff |
  | 实时时钟（rtc）                  | 0xa0000040 ~ 0xa000005f |
  | 键盘数据（keyboard）             | 0xa0000060 ~ 0xa0000063 |
  | 显示器控制信息（vgactl）         | 0xa0000100 ~ 0xa0000107 |
  | 显示器显示存储（vmem）           | 0xa1000000 ~ 0xa10752ff |
  | 显示器快速显示配置信息（ffb_mem) | 0xa2000000 ~ 0xa200001b |
  | 显示器快速显示使能（ffb_fraw）   | 0xa200001c ~ 0xa200001f |

* **串口**

  向`0xa00003f8`地址写入一个长度为1的`ACSII`码，即可在调试终端上显示该字符

* **实时时钟**
