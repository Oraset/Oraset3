# Oraset3 编程语言

Oraset3 是一种现代化的通用编程语言，结合了 Go 的简洁性和 C++ 的性能。

## 特性

- ✅ 简洁的语法，易于学习
- ✅ 高性能的编译型语言（编译为 C++）
- ✅ 跨平台支持（Windows/Linux）
- ✅ 支持函数、变量声明、控制流等

## 快速开始

### Windows
```bash
g++ -std=c++17 -Wall -Iinclude src/cpp/lexer/lexer.cpp src/cpp/parser/parser.cpp src/cpp/semantic/semantic.cpp src/cpp/codegen/codegen.cpp src/cpp/compiler.cpp -o oraset3.exe
```

### Linux/Mac
```bash
g++ -std=c++17 -Wall -Iinclude src/cpp/lexer/lexer.cpp src/cpp/parser/parser.cpp src/cpp/semantic/semantic.cpp src/cpp/codegen/codegen.cpp src/cpp/compiler.cpp -o oraset3
```

## 使用

### 直接运行（推荐）
```bash
# 直接编译并运行 Oraset3 代码
./oraset3 input.ora
```

### 编译到 C++ 文件
```bash
# 编译 Oraset3 源代码到 C++
./oraset3 input.ora output.cpp

# 编译生成的 C++ 代码
g++ -o output output.cpp

# 运行
./output
```

## 示例

### Hello World
```oraset
func main() {
    var message string = "Hello, Oraset3!"
    printString(message)
}

func printString(value string) {
    std::cout << value << std::endl;
}
```

### 阶乘与斐波那契
```oraset
func factorial(n int) int {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

func printFibonacci(n int) {
    var a int = 0;
    var b int = 1;
    var i int = 0;
    var temp int = 0;
    
    while (i < n) {
        printInt(a);
        temp = b;
        b = a + b;
        a = temp;
        i = i + 1;
    }
}
```

## 项目结构

```
Oraset3/
├── src/
│   ├── cpp/           # C++ 编译器核心
│   │   ├── lexer/     # 词法分析器
│   │   ├── parser/    # 语法分析器
│   │   ├── semantic/  # 语义分析器
│   │   └── codegen/   # C++ 代码生成器
│   ├── go/            # Go 前端（Windows）
│   └── clang/         # Clang 后端（Linux）
├── include/           # 头文件
├── tests/             # 测试用例
├── examples/          # 示例代码
└── docs/              # 文档
```

## 编译器架构

Oraset3 编译器分为几个阶段：

1. **词法分析（Lexer）** - 将源代码分解为 token
2. **语法分析（Parser）** - 根据语法规则构建抽象语法树（AST）
3. **语义分析（Semantic Analyzer）** - 检查类型和变量声明
4. **代码生成（Code Generator）** - 将 AST 转换为 C++ 代码

## 语言特性

- ✅ 变量声明 (`var x int = 42`)
- ✅ 函数定义和调用
- ✅ if/else 条件语句
- ✅ while 循环
- ✅ 基本表达式（+、-、*、/、<=、>= 等）
- ✅ 结构体和接口定义
- ✅ 嵌套调用
- ✅ 递归函数

## 语言规范

参见 `docs/LANGUAGE_SPEC.md`

## 许可证

MIT License