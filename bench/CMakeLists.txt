cmake_minimum_required(VERSION 2.8)
project(pull-star-bench CXX)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
    CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
    if(COMPILER_SUPPORTS_CXX0X)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
    endif()
endif()
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -g")

find_package(Thrift REQUIRED)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

set(INCLUDE include ../thrift/include ../ds-lib/include ../ds-lib/bench/include ../ds-lib/external/cst_v_1_1)
file(GLOB_RECURSE SOURCE_FILES src/*.cc)
include_directories(${INCLUDE})
add_executable(rxbench ${SOURCE_FILES})
target_link_libraries(rxbench pullstar)
target_link_libraries(rxbench rxclient)
target_link_libraries(rxbench ${THRIFT_LIBRARIES})
