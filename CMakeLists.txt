cmake_minimum_required(VERSION 3.16)
project(voxelEngine)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 23)

# Get packages
find_package(glfw3 CONFIG REQUIRED)

# Add source files
add_executable(app src/main.cpp)

# Link libraries
target_link_libraries(app glfw)