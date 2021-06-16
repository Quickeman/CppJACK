cmake_minimum_required(VERSION 3.12)

# Project name
project(CppJack)

# Generate compile_commands.json for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Request C++ 11 standard features
set(CMAKE_CXX_STANDARD 11)

# Add subdirectories
add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(lib)
