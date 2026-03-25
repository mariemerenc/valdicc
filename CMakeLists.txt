cmake_minimum_required(VERSION 3.16)

project(vcc VERSION 1.0 LANGUAGES CXX)

include(FetchContent)

# Download CLI11 automatically
FetchContent_Declare(
    CLI11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG v2.3.2   # or latest stable
)

FetchContent_MakeAvailable(CLI11)

add_executable(vcc
    src/main.cpp
    src/preprocess.cpp
)

target_compile_features(vcc PRIVATE cxx_std_17)

# Link CLI11
target_link_libraries(vcc PRIVATE CLI11::CLI11)