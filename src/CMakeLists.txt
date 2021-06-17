include_directories(..)

file(GLOB SRC_FILES *.cpp)

# Create CppJack library
add_library(cppjack STATIC ${SRC_FILES})

# Link the Jack library
target_link_libraries(cppjack jack)