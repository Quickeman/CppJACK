cmake_minimum_required(VERSION 3.12)

# Project name
project(CppJack)

# Generate compile_commands.json for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Request C++ 11 standard features
set(CMAKE_CXX_STANDARD 11)

include_directories(lib/**)
include_directories(lib/jack/common/jack/)

# Create CppJack library
add_library(cppjack STATIC CppJack.cpp)
set_target_properties(cppjack PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# Link the Jack library
target_link_libraries(cppjack jack)

# Make PIC
set_target_properties(cppjack PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
# Set the header to public
set_target_properties(cppjack PROPERTIES PUBLIC_HEADER CppJack.h)
