cmake_minimum_required(VERSION 3.16)

project(vcc VERSION 1.0 LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
    CLI11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG v2.3.2
)

FetchContent_MakeAvailable(CLI11)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

set(CMAKE_CXX_FLAGS "-Wall -pedantic")

add_executable(vcc ${SOURCES})

target_compile_features(vcc PRIVATE cxx_std_20)

target_link_libraries(vcc PRIVATE CLI11::CLI11)