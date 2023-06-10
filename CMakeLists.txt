cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(_PROJECT_NAME_ "Megumi C") # 设置project的名字
set(_EXE_NAME_ "mc") # 设置target的名字
set(_SRC_FILE_NAME_ "src") # 设置target源代码所在的位置

project(${_PROJECT_NAME_})

set(CPP_MODE ON)
if(CPP_MODE)
  set(FB_EXT ".cpp")
else()
  set(FB_EXT ".c")
endif()
message(STATUS "Flex/Bison generated source file extension: ${FB_EXT}")

# options about libraries and includes
set(LIB_DIR "$ENV{CDE_LIBRARY_PATH}/native" CACHE STRING "directory of libraries")
set(INC_DIR "$ENV{CDE_INCLUDE_PATH}" CACHE STRING "directory of includes")
message(STATUS "Library directory: ${LIB_DIR}")
message(STATUS "Include directory: ${INC_DIR}")
# find Flex/Bison
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

# generate lexer/parser
file(GLOB_RECURSE L_SOURCES "src/parser/*.l")
file(GLOB_RECURSE Y_SOURCES "src/parser/*.y")
if(NOT (L_SOURCES STREQUAL "" AND Y_SOURCES STREQUAL ""))
  string(REGEX REPLACE ".*/(.*)\\.l" "${CMAKE_CURRENT_BINARY_DIR}/\\1.lex${FB_EXT}" L_OUTPUTS "${L_SOURCES}")
  string(REGEX REPLACE ".*/(.*)\\.y" "${CMAKE_CURRENT_BINARY_DIR}/\\1.tab${FB_EXT}" Y_OUTPUTS "${Y_SOURCES}")
  flex_target(Lexer ${L_SOURCES} ${L_OUTPUTS})
  bison_target(Parser ${Y_SOURCES} ${Y_OUTPUTS})
  add_flex_bison_dependency(Lexer Parser)
endif()

# project link directories
link_directories(${LIB_DIR})
set(SOURCES 
            ${FLEX_Lexer_OUTPUTS} ${BISON_Parser_OUTPUT_SOURCE})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

add_executable(${_EXE_NAME_} ${SOURCES})
aux_source_directory(${_SRC_FILE_NAME_} _SOURCE_)
target_sources(${_EXE_NAME_} PUBLIC ${_SOURCE_})
target_include_directories(${_EXE_NAME_} PUBLIC ${_SRC_FILE_NAME_} ${INC_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(${_EXE_NAME_} PROPERTIES C_STANDARD 11 CXX_STANDARD 17)
target_link_libraries(${_EXE_NAME_} pthread dl)
# target_link_libraries(${_EXE_NAME_} koopa pthread dl)
add_subdirectory(${_SRC_FILE_NAME_})