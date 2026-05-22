#include "oraset3.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <filesystem>
#include <sstream>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
#endif

namespace oraset3 {

// 全局变量：是否导入了 sysui
bool sysuiImported = false;

// 加载头文件
std::string loadHeader(const std::string& headerName) {
    std::vector<std::string> searchPaths = {
        "./include/",
        "./headers/",
        "./lib/",
        ""
    };

    std::string orhName = headerName;
    if (orhName.find(".orh") == std::string::npos) {
        orhName += ".orh";
    }

    for (const auto& path : searchPaths) {
        std::string fullPath = path + orhName;
        if (std::filesystem::exists(fullPath)) {
            std::ifstream ifs(fullPath);
            if (ifs) {
                std::string content((std::istreambuf_iterator<char>(ifs)),
                                   std::istreambuf_iterator<char>());
                return content;
            }
        }
    }

    throw std::runtime_error("Cannot find header file: " + headerName);
}

// 处理 $ 导入语法
std::string processHeaderIncludes(const std::string& source) {
    std::istringstream iss(source);
    std::string line;
    std::string result;
    bool inMultiLineComment = false;

    while (std::getline(iss, line)) {
        std::string trimmed = line;
        // 去除首尾空白
        while (!trimmed.empty() && (trimmed[0] == ' ' || trimmed[0] == '\t')) {
            trimmed = trimmed.substr(1);
        }
        while (!trimmed.empty() && (trimmed.back() == ' ' || trimmed.back() == '\t')) {
            trimmed.pop_back();
        }

        // 跳过单行注释
        if (trimmed.substr(0, 2) == "//") {
            continue;
        }

        // 处理多行注释开始/结束
        if (trimmed.find("/*") != std::string::npos) {
            inMultiLineComment = true;
        }
        if (trimmed.find("*/") != std::string::npos) {
            inMultiLineComment = false;
            continue;
        }
        if (inMultiLineComment) {
            continue;
        }

        // 处理 $ 头文件导入
        if (!trimmed.empty() && trimmed[0] == '$') {
            std::string headerName = trimmed.substr(1);
            // 去除可能的分号
            if (!headerName.empty() && headerName.back() == ';') {
                headerName.pop_back();
            }
            // 去除首尾空白
            while (!headerName.empty() && (headerName[0] == ' ' || headerName[0] == '\t')) {
                headerName = headerName.substr(1);
            }
            while (!headerName.empty() && (headerName.back() == ' ' || headerName.back() == '\t')) {
                headerName.pop_back();
            }

            // 特殊处理 $sysui - 内置UI库
            if (headerName == "sysui") {
                sysuiImported = true;
                result += "// UI Library (sysui) imported\n";
            } else {
                try {
                    std::string headerContent = loadHeader(headerName);
                    result += "// Header: " + headerName + "\n";
                    // 直接包含头文件内容（用Oraset3语言编写的函数实现）
                    result += headerContent + "\n";
                } catch (const std::exception& e) {
                    std::cerr << "Warning: " << e.what() << std::endl;
                }
            }
        } else {
            result += line + "\n";
        }
    }

    return result;
}

std::string Compiler::compile(const std::string& source) {
    try {
        // 重置 sysuiImported 标志
        sysuiImported = false;
        
        // 预处理：处理头文件导入
        std::string processedSource = processHeaderIncludes(source);

        // 词法分析
        Lexer lexer(processedSource);
        std::vector<std::unique_ptr<Token>> tokens;

        while (true) {
            auto token = lexer.nextToken();
            if (token->type == TokenType::END_OF_FILE) {
                break;
            }
            tokens.push_back(std::move(token));
        }

        // 语法分析
        Parser parser(std::move(tokens));
        auto program = parser.parse();

        // 语义分析
        SemanticAnalyzer analyzer(sysuiImported);
        analyzer.analyze(*program);

        if (analyzer.hasErrors()) {
            std::string errorMsg = "Semantic errors:\n";
            for (const auto& error : analyzer.getErrors()) {
                errorMsg += "  - " + error + "\n";
            }
            throw std::runtime_error(errorMsg);
        }

        // 代码生成
        CodeGenerator generator(sysuiImported);
        return generator.generate(*program);

    } catch (const std::exception& e) {
        throw std::runtime_error("Compilation failed: " + std::string(e.what()));
    }
}

} // namespace oraset3

void runProgram(const std::string& exePath) {
    std::string cmd = "\"" + exePath + "\"";
    int result = system(cmd.c_str());
    if (result != 0) {
        std::cerr << "Program exited with code: " << result << std::endl;
    }
}

std::string getTempFileName(const std::string& prefix, const std::string& suffix) {
    static int counter = 0;
    return prefix + "_" + std::to_string(counter++) + suffix;
}

// 运行程序并捕获输出
std::string runProgramAndCapture(const std::string& exePath) {
    #ifdef _WIN32
    // 使用临时文件来捕获输出，避免管道问题
    std::string outputFile = getTempFileName("oraset3_out", ".txt");

    // 确保路径中没有引号
    std::string cleanExePath = exePath;
    if (!cleanExePath.empty() && cleanExePath.front() == '"') {
        cleanExePath = cleanExePath.substr(1);
    }
    if (!cleanExePath.empty() && cleanExePath.back() == '"') {
        cleanExePath = cleanExePath.substr(0, cleanExePath.size() - 1);
    }

    std::string cmd = cleanExePath + " > " + outputFile + " 2>&1";
    system(cmd.c_str());

    std::string output;
    {
        std::ifstream ifs(outputFile);
        output = std::string((std::istreambuf_iterator<char>(ifs)),
                            std::istreambuf_iterator<char>());
    } // ifs 在此处自动关闭

    std::filesystem::remove(outputFile);
    return output;
    #else
    // Linux version
    std::string outputFile = getTempFileName("oraset3_out", ".txt");

    std::string cmd = exePath + " > " + outputFile + " 2>&1";
    system(cmd.c_str());

    std::string output;
    {
        std::ifstream ifs(outputFile);
        output = std::string((std::istreambuf_iterator<char>(ifs)),
                            std::istreambuf_iterator<char>());
    } // ifs 在此处自动关闭

    std::filesystem::remove(outputFile);
    return output;
    #endif
}

// 解释器模式运行
void interpretProgram(const std::string& source, bool noOutputHeader) {
    try {
        oraset3::Compiler compiler;
        std::string cppCode = compiler.compile(source);

        // 使用临时文件
        std::string outputFile = getTempFileName("oraset3", ".cpp");
        std::string exeFile = outputFile;
        size_t dotPos = exeFile.find_last_of('.');
        if (dotPos != std::string::npos) {
            exeFile = exeFile.substr(0, dotPos);
        }
        #ifdef _WIN32
        exeFile += ".exe";
        #endif

        // 写入C++文件
        std::ofstream ofs(outputFile);
        ofs << cppCode;
        ofs.close();

        // 编译
        std::string compileCmd;
        #ifdef _WIN32
        compileCmd = "g++ -std=c++20 -mconsole -finput-charset=utf-8 -fexec-charset=utf-8 -o \"" + exeFile + "\" \"" + outputFile + "\"";
        #else
        compileCmd = "g++ -std=c++20 -finput-charset=utf-8 -fexec-charset=utf-8 -o " + exeFile + " " + outputFile;
        #endif

        if (!noOutputHeader) {
            std::cout << "Compiling... " << std::endl;
        }
        
        // 记录编译开始时间
        auto compileStart = std::chrono::high_resolution_clock::now();
        
        int compileResult = system(compileCmd.c_str());
        
        // 记录编译结束时间
        auto compileEnd = std::chrono::high_resolution_clock::now();
        auto compileDuration = std::chrono::duration_cast<std::chrono::milliseconds>(compileEnd - compileStart).count();
        
        if (compileResult != 0) {
            std::cerr << "C++ compilation failed" << std::endl;
            std::filesystem::remove(outputFile);
            return;
        }

        if (!noOutputHeader) {
            // 运行程序并捕获输出
            std::cout << "Running..." << std::endl;
            
            // 记录运行开始时间
            auto runStart = std::chrono::high_resolution_clock::now();
            
            std::string programOutput = runProgramAndCapture(exeFile);
            
            // 记录运行结束时间
            auto runEnd = std::chrono::high_resolution_clock::now();
            auto runDuration = std::chrono::duration_cast<std::chrono::milliseconds>(runEnd - runStart).count();

            std::cout << "------------------------" << std::endl;
            std::cout << programOutput;
            std::cout << "------------------------" << std::endl;

            // 显示编译和运行时间
            std::cout << "编译耗时: " << compileDuration << " ms" << std::endl;
            std::cout << "运行耗时: " << runDuration << " ms" << std::endl;
            std::cout << "------------------------" << std::endl;
            std::cout << "Program finished" << std::endl;
        } else {
            // 仅输出程序结果 - 直接运行避免管道带来的额外空行
            std::string runCmd;
            #ifdef _WIN32
            runCmd = "\"" + exeFile + "\"";
            #else
            runCmd = exeFile;
            #endif
            system(runCmd.c_str());
        }

        // 清理临时文件
        std::filesystem::remove(outputFile);
        std::filesystem::remove(exeFile);

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    // 解析命令行参数
    std::string inputFile;
    std::string outputFile;
    bool runDirectly = true;
    bool noOutputHeader = false;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--noo") {
            noOutputHeader = true;
        } else if (inputFile.empty()) {
            inputFile = arg;
        } else if (outputFile.empty()) {
            outputFile = arg;
            runDirectly = false;
        }
    }

    if (inputFile.empty()) {
        std::cerr << "Usage:" << std::endl;
        std::cerr << "  oraset3 <input_file>              - Compile and run with full output" << std::endl;
        std::cerr << "  oraset3 <input_file> <output_file> - Compile to C++ file" << std::endl;
        std::cerr << "  oraset3 <input_file> --noo        - Run with minimal output (only program output)" << std::endl;
        return 1;
    }

    // 如果使用 --noo 参数，直接运行
    if (noOutputHeader) {
        try {
            std::ifstream ifs(inputFile);
            if (!ifs) {
                std::cerr << "Cannot open input file: " << inputFile << std::endl;
                return 1;
            }

            std::string source((std::istreambuf_iterator<char>(ifs)),
                               std::istreambuf_iterator<char>());

            interpretProgram(source, noOutputHeader);

        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            return 1;
        }
        return 0;
    }

    // 传统模式 - 编译并运行
    try {
        // 读取源文件
        std::ifstream ifs(inputFile);
        if (!ifs) {
            std::cerr << "Cannot open input file: " << inputFile << std::endl;
            return 1;
        }

        std::string source((std::istreambuf_iterator<char>(ifs)),
                           std::istreambuf_iterator<char>());

        if (runDirectly && outputFile.empty()) {
            // 没有指定输出文件，直接编译并运行（带完整输出）
            interpretProgram(source, false);
        } else {
            // 编译到指定文件
            oraset3::Compiler compiler;
            std::string output = compiler.compile(source);

            // 写入输出文件
            std::ofstream ofs(outputFile);
            if (!ofs) {
                std::cerr << "Cannot open output file: " << outputFile << std::endl;
                return 1;
            }

            ofs << output;
            ofs.close();

            std::cout << "Compilation successful! Output written to: " << outputFile << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}