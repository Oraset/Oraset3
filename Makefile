CC = g++
CFLAGS = -std=c++17 -Wall -Iinclude
LDFLAGS = 

SRCS = src/cpp/lexer/lexer.cpp \
       src/cpp/parser/parser.cpp \
       src/cpp/semantic/semantic.cpp \
       src/cpp/codegen/codegen.cpp \
       src/cpp/compiler.cpp

OBJS = $(SRCS:.cpp=.o)
TARGET = oraset3

.PHONY: all clean test run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
	rm -f tests/*.out
	rm -f examples/*.out

test: all
	./$(TARGET) tests/test1.ora tests/test1.cpp
	g++ -o tests/test1.out tests/test1.cpp
	./tests/test1.out

run: all
	./$(TARGET) examples/hello.ora examples/hello.cpp
	g++ -o examples/hello.out examples/hello.cpp
	./examples/hello.out

# Linux build with clang
clang:
	clang++ -std=c++17 -Wall -Iinclude -o $(TARGET) $(SRCS)

# Windows build
win:
	x86_64-w64-mingw32-g++ -std=c++17 -Wall -Iinclude -o $(TARGET).exe $(SRCS)