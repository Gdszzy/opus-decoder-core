cmake_minimum_required(VERSION 3.13)
project(blufi)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--gc-sections")
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
set(CMAKE_STRIP "strip")

set(FLAGS_COMMON "-Wall -Os -flto -fdata-sections -ffunction-sections -s")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS_COMMON}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAGS_COMMON}")

add_subdirectory(${CMAKE_SOURCE_DIR}/3rd/opus/)

#core
add_library(core src/core.cpp)
target_include_directories(core PUBLIC 
    ${CMAKE_SOURCE_DIR}/include 
)
target_link_libraries(core PRIVATE opus)

# test
# enable_testing()
add_executable(testMain test/testMain.cpp)
target_link_libraries(testMain PRIVATE core)
target_compile_definitions(testMain PRIVATE DEBUG)
# add_test(NAME testMain COMMAND testMain)

# cli
add_executable(decoder src/cli.cpp)
target_link_libraries(decoder PRIVATE core)
# target_link_options(decoder PRIVATE -Os -flto)