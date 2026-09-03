cmake_minimum_required(VERSION 3.10)
project(ByteVM C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compile options for warnings and debugging
if(MSVC)
    add_compile_options(/W4 /WX-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -g)
endif()

# Source files (current flat structure)
set(SOURCES
    src/main.c
    src/syntax/lexer.c
    src/syntax/parser.c
    src/syntax/ast.c
    src/compiler/symbol_table.c
    src/compiler/codegen.c
    src/runtime/value.c
    src/runtime/vm.c
    src/runtime/chunk.c
)

# Add the executable target
add_executable(byteVM ${SOURCES})

# Include headers from the current directory
target_include_directories(byteVM PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
