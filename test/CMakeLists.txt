include_directories("../include")
FILE(GLOB TEST_INCLUDE_FILES "*.hpp" "*.cpp")
add_library(ParserDataStructureTest STATIC ${TEST_INCLUDE_FILES})
