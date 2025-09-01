cmake_minimum_required(VERSION 3.25)
project(blufi)

set(COMMON_CXX_OPTIONS
    -Wall
    -O3
    -flto
    -fdata-sections
    -ffunction-sections
)
set(LINK_OPTIONS
    -Wl,--gc-sections
)

set(OPUS_X86_PRESUME_AVX2 on)
set(OPUS_X86_PRESUME_SSE4_1 on)

add_subdirectory(${CMAKE_SOURCE_DIR}/3rd/opus/)
# configure libopus
target_compile_options(opus PRIVATE -march=native)

#core
add_library(core src/core.cpp)
target_compile_features(core PUBLIC cxx_std_20)
target_include_directories(core PUBLIC 
    ${CMAKE_SOURCE_DIR}/include 
)
target_compile_options(core PRIVATE ${COMMON_CXX_OPTIONS})
target_link_libraries(core PRIVATE opus)

# cli
add_executable(decoder src/cli.cpp)
target_compile_features(decoder PRIVATE cxx_std_20)
target_compile_options(decoder PRIVATE ${COMMON_CXX_OPTIONS})

target_link_libraries(decoder PRIVATE core)
target_link_options(decoder PRIVATE
    ${LINK_OPTIONS}
    -static # 强制静态链接
)