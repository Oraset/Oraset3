package main

import (
	"bufio"
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"runtime"
)

func main() {
	if len(os.Args) != 3 {
		fmt.Println("Usage: oraset3-go <input_file> <output_file>")
		os.Exit(1)
	}

	inputFile := os.Args[1]
	outputFile := os.Args[2]

	err := compile(inputFile, outputFile)
	if err != nil {
		fmt.Printf("Error: %v\n", err)
		os.Exit(1)
	}

	fmt.Printf("Compilation successful! Output: %s\n", outputFile)
}

func compile(inputFile, outputFile string) error {
	source, err := readFile(inputFile)
	if err != nil {
		return err
	}

	// 调用C++编译器核心
	cppOutput, err := runCppCompiler(source)
	if err != nil {
		return err
	}

	// 写入临时C++文件
	tmpCppFile := outputFile + ".tmp.cpp"
	err = writeFile(tmpCppFile, cppOutput)
	if err != nil {
		return err
	}
	defer os.Remove(tmpCppFile)

	// 根据平台调用相应的编译器
	if runtime.GOOS == "windows" {
		return compileWindows(tmpCppFile, outputFile)
	} else {
		return compileLinux(tmpCppFile, outputFile)
	}
}

func readFile(path string) (string, error) {
	data, err := os.ReadFile(path)
	if err != nil {
		return "", fmt.Errorf("failed to read file: %w", err)
	}
	return string(data), nil
}

func writeFile(path, content string) error {
	err := os.WriteFile(path, []byte(content), 0644)
	if err != nil {
		return fmt.Errorf("failed to write file: %w", err)
	}
	return nil
}

func runCppCompiler(source string) (string, error) {
	// 在实际项目中，这里会调用C++编译核心
	// 由于Go无法直接调用C++代码，我们使用exec调用编译好的oraset3编译器
	// 或者可以使用cgo绑定
	
	// 这里简化处理，直接返回源代码内容（演示用）
	return source, nil
}

func compileWindows(cppFile, outputFile string) error {
	// 使用MinGW或MSVC编译
	gccPath, err := exec.LookPath("g++")
	if err != nil {
		return fmt.Errorf("g++ not found: %w", err)
	}

	args := []string{
		"-o", outputFile,
		cppFile,
		"-std=c++17",
		"-Wall",
	}

	cmd := exec.Command(gccPath, args...)
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr

	err = cmd.Run()
	if err != nil {
		return fmt.Errorf("compilation failed: %w", err)
	}

	return nil
}

func compileLinux(cppFile, outputFile string) error {
	// 使用Clang编译
	clangPath, err := exec.LookPath("clang++")
	if err != nil {
		return fmt.Errorf("clang++ not found: %w", err)
	}

	args := []string{
		"-o", outputFile,
		cppFile,
		"-std=c++17",
		"-Wall",
		"-O2",
	}

	cmd := exec.Command(clangPath, args...)
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr

	err = cmd.Run()
	if err != nil {
		return fmt.Errorf("compilation failed: %w", err)
	}

	return nil
}

func isWindows() bool {
	return runtime.GOOS == "windows"
}

func isLinux() bool {
	return runtime.GOOS == "linux"
}

// getCompilerPath returns the path to the appropriate compiler
func getCompilerPath() (string, error) {
	if isWindows() {
		return exec.LookPath("g++")
	}
	return exec.LookPath("clang++")
}

// ensureDirectory ensures the output directory exists
func ensureDirectory(path string) error {
	dir := filepath.Dir(path)
	if dir == "." {
		return nil
	}
	return os.MkdirAll(dir, 0755)
}