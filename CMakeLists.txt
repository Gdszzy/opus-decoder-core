cmake_minimum_required(VERSION 3.25)
project(opus-decoder-core LANGUAGES C CXX)

set(COMMON_COMPILE_OPTIONS "")

if(CMAKE_CROSSCOMPILING)
    message("Disable -march-native")
else()
    string(APPEND COMMON_COMPILE_OPTIONS "-march=native")
    message("Enable -march-native")
endif()

# prepare dependencies
include(FetchContent)
FetchContent_Declare(
    opus
    GIT_REPOSITORY https://github.com/xiph/opus.git
    GIT_TAG v1.5.2
)
message("Fetching opus...")
FetchContent_MakeAvailable(opus)
target_compile_options(opus PRIVATE ${COMMON_COMPILE_OPTIONS})

FetchContent_Declare(
    CLI11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG v2.6.1
)
message("Fetching CLI11...")
FetchContent_MakeAvailable(CLI11)

#core
add_library(core src/core.cpp)
target_compile_features(core PUBLIC cxx_std_20)
target_include_directories(core PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_options(core PRIVATE ${COMMON_COMPILE_OPTIONS})
target_link_libraries(core PRIVATE opus)

# cli
add_executable(decoder src/cli.cpp)
target_compile_features(decoder PRIVATE cxx_std_20)
target_compile_options(decoder PRIVATE ${COMMON_COMPILE_OPTIONS})

target_link_libraries(decoder PRIVATE core CLI11::CLI11)
target_link_options(decoder PRIVATE
    -static # 强制静态链接
)