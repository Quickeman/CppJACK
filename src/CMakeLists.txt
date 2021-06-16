# Create CppJack library
add_library(cppjack STATIC CppJack.cpp)

# Link the Jack library
target_link_libraries(cppjack jack)

# Make PIC
set_target_properties(cppjack PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
# Set the header to public
set_target_properties(cppjack PROPERTIES PUBLIC_HEADER CppJack.h)
