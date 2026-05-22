@echo off
echo Building Oraset3 Compiler...

rem Create build directory
if not exist build mkdir build
cd build

rem Run CMake
cmake .. -G "Visual Studio 17 2022" -A x64
if %errorlevel% neq 0 (
    echo CMake failed
    exit /b 1
)

rem Build the project
cmake --build . --config Release
if %errorlevel% neq 0 (
    echo Build failed
    exit /b 1
)

echo Build successful!
cd ..