include_directories(
    ..
    framework
)

# Get test source files
file(GLOB TEST_SRCS *.cpp)

# Run through each test file
foreach(testSrc ${TEST_SRCS})
    # Get extension-less file name
    get_filename_component(testName ${testSrc} NAME_WE)
    # Add target
    add_executable(${testName} ${testSrc})
    # Link to relevant libraries
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
