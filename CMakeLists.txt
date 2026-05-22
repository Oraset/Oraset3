cmake_minimum_required(VERSION 3.16)
project(Oraset3Compiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(SOURCES
    src/cpp/lexer/lexer.cpp
    src/cpp/parser/parser.cpp
    src/cpp/semantic/semantic.cpp
    src/cpp/codegen/codegen.cpp
    src/cpp/compiler.cpp
)

add_executable(oraset3 ${SOURCES})

if(WIN32)
    target_compile_options(oraset3 PRIVATE /W3)
else()
    target_compile_options(oraset3 PRIVATE -Wall -Wextra)
endif()

install(TARGETS oraset3 DESTINATION bin)