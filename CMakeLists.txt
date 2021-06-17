cmake_minimum_required(VERSION 3.12)

# Project name
project(CppJack)

# Generate compile_commands.json for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Request C++ 11 standard features
set(CMAKE_CXX_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR}/**)

# Create CppJack library
add_library(cppjack STATIC CppJack.cpp)
set_target_properties(cppjack PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# Link the Jack library
target_link_libraries(cppjack jack)

# Make PIC
set_target_properties(cppjack PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
# Set the header to public
set_target_properties(cppjack PROPERTIES PUBLIC_HEADER CppJack.h)


### Unit Testing

# Include test files
file(GLOB TEST_SRCS ${PROJECT_SOURCE_DIR}/test/*.cpp)
# Run through each test file
foreach(testSrc ${TEST_SRCS})
    # Get extension-less file name
    get_filename_component(testName ${testSrc} NAME_WE)
    # Add target
    add_executable(${testName} ${testSrc})
    # Link to Boost, source code and other external libraries
    target_link_libraries(${testName} cppjack)
    # Put test executables in their own directory
    set_target_properties(${testName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/test/bin)

    # Add test to execution
    add_test(
        NAME ${testName}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test/bin
        COMMAND ${testName}
    )
endforeach(testSrc)

# Add definition for the project root directly for file loading
add_definitions(-DCPPJACK_ROOT_DIR="${PROJECT_SOURCE_DIR}/")