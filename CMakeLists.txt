project(ParserDataStructures)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Werror -Wextra -g")
set(INCLUDE_DIR include)
include_directories (${INCLUDE_DIR})
#include_directories (include)
aux_source_directory(. SRC_LIST)
#add_executable(${PROJECT_NAME} ${SRC_LIST})

aux_source_directory(./test Test)

file(GLOB TEST_FILES "test/*.cpp")
add_executable(${PROJECT_NAME} ${TEST_FILES})

file(GLOB HEADER_FILES_LIB "include/*.hpp")
add_subdirectory(test)

FILE(GLOB_RECURSE LibFiles "include/*.hpp")
add_custom_target(headers SOURCES ${LibFiles})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#set_target_properties(test PROPERTIES LINKER_LANGUAGE CXX)
#target_link_libraries(${PROJECT_NAME} ParserDataStructureTest)
