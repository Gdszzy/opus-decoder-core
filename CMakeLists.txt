cmake_minimum_required(VERSION 3.25)
project(opus-decoder-core LANGUAGES C CXX)

set(COMMON_COMPILE_OPTIONS "")

if(NOT DEFINED ENV{DISABLE_MARCH_NATIVE})
    string(APPEND COMMON_COMPILE_OPTIONS "-march=native")
    message("Enable -march-native")
else()
    message("Disable -march-native")
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rd/opus/)
target_compile_options(opus PRIVATE ${COMMON_COMPILE_OPTIONS})

#core
add_library(core src/core.cpp)
target_compile_features(core PUBLIC cxx_std_20)
target_include_directories(core PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include 
    3rd/opus/include
)
target_compile_options(core PRIVATE ${COMMON_COMPILE_OPTIONS})
target_link_libraries(core PRIVATE opus)

# cli
add_executable(decoder src/cli.cpp)
target_compile_features(decoder PRIVATE cxx_std_20)
target_compile_options(decoder PRIVATE ${COMMON_COMPILE_OPTIONS})

target_link_libraries(decoder PRIVATE core)
target_link_options(decoder PRIVATE
    -static # 强制静态链接
)