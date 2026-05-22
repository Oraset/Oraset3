# Oraset3 编程语言规范

## 1. 概述

Oraset3 是一种现代化的通用编程语言，结合了 Go 的简洁性和 C++ 的性能。

### 1.1 设计目标
- 简洁的语法，易于学习
- 高性能的编译型语言
- 跨平台支持（Windows/Linux）
- 内存安全

## 2. 词法结构

### 2.1 关键字
```
var, let, const, func, struct, interface, if, else, for, while, return, break, continue, import, export, true, false, nil
```

### 2.2 标识符
字母开头，可包含字母、数字、下划线

### 2.3 字面量
- 整数: `42`, `-100`, `0x1A`
- 浮点数: `3.14`, `-0.5`, `1e10`
- 字符串: `"hello"`, `'world'`
- 布尔值: `true`, `false`
- 空值: `nil`

### 2.4 运算符
- 算术: `+`, `-`, `*`, `/`, `%`
- 比较: `==`, `!=`, `<`, `>`, `<=`, `>=`
- 逻辑: `&&`, `||`, `!`
- 赋值: `=`, `:=`, `+=`, `-=`, `*=`, `/=`

## 3. 语法结构

### 3.1 变量声明（简化语法）

**推荐写法 - 使用类型推断:**
```oraset
x := 42           // 自动推断为 int
name := "Alice"   // 自动推断为 string
flag := true      // 自动推断为 bool
pi := 3.14        // 自动推断为 float
```

**传统写法:**
```oraset
var x int = 42
let name string = "Alice"
const PI float = 3.14
```

### 3.2 函数定义

**简洁写法 - 省略返回类型（void）:**
```oraset
func greet(name string) {
    println("Hello, " + name)
}
```

**带返回值:**
```oraset
func add(a int, b int) int {
    return a + b
}
```

**无参数:**
```oraset
func main() {
    println("Hello, Oraset3!")
}
```

### 3.3 条件语句
```oraset
if x > 0 {
    println("Positive")
} else if x < 0 {
    println("Negative")
} else {
    println("Zero")
}
```

### 3.4 循环
```oraset
// while 循环
while i < 10 {
    println(i)
    i := i + 1
}

// for 循环
for i := 0; i < 10; i := i + 1 {
    println(i)
}
```

### 3.5 结构体
```oraset
struct Point {
    x int
    y int
}
```

### 3.6 接口
```oraset
interface Shape {
    area() float
}
```

## 4. 内置函数

Oraset3 提供以下内置函数，无需声明即可使用：

| 函数 | 说明 |
|------|------|
| `print(value)` | 输出值到控制台（不换行） |
| `println(value)` | 输出值到控制台（换行） |

**支持的类型:**
- `int` - 整数
- `float` - 浮点数
- `string` - 字符串
- `bool` - 布尔值

**示例:**
```oraset
func main() {
    println(42)                    // 输出整数
    println(3.14)                  // 输出浮点数
    println("Hello, World!")       // 输出字符串
    println(true)                  // 输出布尔值
    
    print("Hello")                 // 不换行输出
    println(" World!")             // 换行输出
}
```

## 5. 类型系统

### 5.1 基本类型
- `int` - 整数（64位）
- `float` - 浮点数（64位）
- `bool` - 布尔值
- `string` - 字符串
- `nil` - 空值

### 5.2 复合类型
- 数组: `[5]int`
- 切片: `[]int`
- 字典: `map[string]int`
- 函数: `func(int) int`

## 6. 内存管理

Oraset3 使用自动垃圾回收机制，无需手动管理内存。

## 7. 模块系统

```oraset
import "math"
export func main() {
    // 入口函数
}
```

## 8. 代码风格建议

### 8.1 分号
分号在 Oraset3 中是可选的，推荐省略分号以保持代码简洁：

**推荐:**
```oraset
x := 42
println(x)
```

**也支持:**
```oraset
x := 42;
println(x);
```

### 8.2 缩进
使用 4 个空格进行缩进。

### 8.3 命名规范
- 变量和函数名使用蛇形命名: `my_variable`, `my_function`
- 类型名使用 Pascal 命名: `MyStruct`, `MyInterface`
