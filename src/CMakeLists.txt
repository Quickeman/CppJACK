include_directories(..)

file(GLOB SRC_FILES *.cpp)

# Create CppJack library
add_library(cppjack STATIC ${SRC_FILES})

set_target_properties(cppjack PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# Link the Jack library
target_link_libraries(cppjack jack)