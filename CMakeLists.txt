cmake_minimum_required(VERSION 3.10)

# Project name
project(CppJack)

# Generate compile_commands.json for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Determine if unit tests should be compiled
set(CPPJACK_ENABLE_TESTS TRUE)
if(DEFINED DISABLE_TESTS)
    if(DISABLE_TESTS)
        set(CPPJACK_ENABLE_TESTS FALSE)
    endif()
endif()

if(CPPJACK_ENABLE_TESTS)
    enable_testing()
endif()

# Request C++11 standard features
set(CMAKE_CXX_STANDARD 11)

# Make sure the compiler looks here for source files
include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_subdirectory(include)
include_directories(include)
add_subdirectory(src)
add_subdirectory(demo)

# Set the header to public
set_target_properties(cppjack PROPERTIES PUBLIC_HEADER CppJack.h)

# Make PIC
set_target_properties(cppjack PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

# Tests
if(CPPJACK_ENABLE_TESTS)
    add_subdirectory(test)
endif()
