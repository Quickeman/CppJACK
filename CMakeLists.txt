cmake_minimum_required(VERSION 3.12)

# Project name
project(CppJack)

# Generate compile_commands.json for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Request C++ 11 standard features
set(CMAKE_CXX_STANDARD 11)

# Make sure the compiler looks here for source files
include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_subdirectory(include)
include_directories(include)
add_subdirectory(src)
add_subdirectory(demo)

set_target_properties(cppjack PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# Make PIC
set_target_properties(cppjack PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
# Set the header to public
set_target_properties(cppjack PROPERTIES PUBLIC_HEADER include/CppJack.h)

add_subdirectory(test)

# Add definition for the project root directly for file loading
add_definitions(-DCPPJACK_ROOT_DIR="${PROJECT_SOURCE_DIR}/")
