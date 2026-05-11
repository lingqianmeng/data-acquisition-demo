# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(AdasDataAcquisitionDemo VERSION 1.0 LANGUAGES CXX)

# Require C++17 standard (Crucial for our Lock-Free Buffer and std::optional)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add standard compiler warnings to catch bugs early
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra)
endif()

# Find the Threading library (required for std::thread / pthread)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Define the executable and where its source code lives
add_executable(adas_demo src/main.cpp)

# Link the threading library to our executable
target_link_libraries(adas_demo PRIVATE Threads::Threads)