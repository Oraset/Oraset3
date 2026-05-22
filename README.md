# Oraset3 编程语言

[![GPL-3.0 License](https://img.shields.io/badge/License-GPL--3.0-blue.svg)](https://opensource.org/licenses/GPL-3.0)

Oraset3 是一种现代化的通用编程语言，结合了 Go 的简洁性和 C++ 的性能。编译为 C++ 代码，支持直接执行，语法简洁如 Python，性能接近 C++。

## 特性

### 核心特性
- ✅ **语法简洁** - 比 Python 更简洁，比 Go 更灵活
- ✅ **高性能编译** - 编译为优化的 C++ 代码
- ✅ **跨平台支持** - Windows/Linux/macOS
- ✅ **无需主函数** - 顶层代码直接执行

### 操作系统级特性
- ✅ **指针操作** - `&` 取地址，`*` 解引用
- ✅ **位操作符** - `&`, `|`, `^`, `~`, `<<`, `>>`
- ✅ **联合体(union)** - 内存重叠的数据结构
- ✅ **枚举(enum)** - 类型安全的常量组
- ✅ **类型转换** - C 风格类型转换 `(type)value`

### 语法示例
```oraset
// 变量声明（简洁语法）
name := "Oraset3"
age := 1

// 位操作
a := 10  // 0b1010
b := 12  // 0b1100
println(a & b)  // 8
println(a | b)  // 14
println(a ^ b)  // 6

// 枚举
enum Color { RED, GREEN, BLUE = 10 }

// 联合体
union Data { i: int; f: float }

// 指针操作
x := 100
ptr := &x
*ptr = 200  // 通过指针修改值

// 类型转换
num := 3.14
intNum := (int)num  // 3
```

## 快速开始

### 编译 Oraset3 编译器

```bash
# Windows (MinGW)
g++ -std=c++20 -Iinclude src/cpp/*.cpp src/cpp/lexer/*.cpp src/cpp/parser/*.cpp src/cpp/semantic/*.cpp src/cpp/codegen/*.cpp -o oraset3.exe -mconsole

# Linux/macOS
g++ -std=c++20 -Iinclude src/cpp/*.cpp src/cpp/lexer/*.cpp src/cpp/parser/*.cpp src/cpp/semantic/*.cpp src/cpp/codegen/*.cpp -o oraset3
```

### 运行 Oraset3 代码

```bash
# 直接运行（推荐）
./oraset3 input.ora

# 编译到 C++ 文件
./oraset3 input.ora output.cpp

# 编译生成的 C++ 代码
g++ -std=c++20 -o output output.cpp
./output
```

## 项目结构

```
Oraset3/
├── src/
│   ├── cpp/               # C++ 编译器核心
│   │   ├── lexer/         # 词法分析器
│   │   ├── parser/        # 语法分析器
│   │   ├── semantic/      # 语义分析器
│   │   └── codegen/       # C++ 代码生成器
│   └── go/                # Go 前端
├── include/               # 头文件和内置库
│   └── fdan.orh           # 内置函数库
├── tests/                 # 测试用例
│   ├── test_os_features.ora   # 操作系统特性测试
│   ├── test_bitops.ora        # 位操作测试
│   ├── test_enum.ora          # 枚举测试
│   └── test_union.ora         # 联合体测试
└── docs/                  # 文档
```

## 内置函数

| 函数 | 说明 |
|------|------|
| `println(x)` | 打印并换行 |
| `print(x)` | 打印不换行 |
| `input()` | 读取一行输入 |
| `int(x)` | 转换为整数 |
| `float(x)` | 转换为浮点数 |
| `string(x)` | 转换为字符串 |
| `len(x)` | 获取长度 |
| `sys(cmd)` | 执行系统命令 |

## 许可

本项目采用 [GPL-3.0](LICENSE) 许可证。

- 编译器源码（词法分析器、语法分析器、语义分析器、代码生成器）受 GPL-3.0 约束
- Oraset3 语言语法、运行时库、以及编译生成的代码**不受此许可证约束**
- 您可以自由使用 Oraset3 编写任何程序，包括私有和商业软件

## 示例代码

### Hello World
```oraset
println("Hello, Oraset3!");
```

### 阶乘函数
```oraset
func factorial(n int) int {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

println(factorial(5));  // 120
```

### 结构体
```oraset
struct Point { x: int; y: int; }

var p Point
p.x = 10
p.y = 20
println(p.x + p.y)
```
