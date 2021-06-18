include_directories(..)

# Get demo source files
file(GLOB DEMO_SRCS *.cpp)

# Run through each demo file
foreach(demoSrc ${DEMO_SRCS})
    # Get file name w/o extension
    get_filename_component(demoName ${demoSrc} NAME_WE)
    # Add target executable
    add_executable(${demoName} ${demoSrc})
    # Link to relevant libraries
    target_link_libraries(${demoName} cppjack)
    # Place in executables subdirectory
    set_target_properties(${demoName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY bin)
endforeach(demoSrc ${DEMO_SRCS})
