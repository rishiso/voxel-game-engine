cmake_minimum_required(VERSION 3.16)
project(voxelEngine)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 23)

include(FetchContent)
FetchContent_Declare(
    FastNoise2
    GIT_REPOSITORY https://github.com/Auburn/FastNoise2.git
    GIT_TAG master
)
FetchContent_MakeAvailable(FastNoise2)

# Get packages
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# Add source files
add_executable(app src/main.cpp src/shader.cpp src/camera.cpp src/chunk.cpp)

# Link libraries
target_link_libraries(app PRIVATE glfw)
target_link_libraries(app PRIVATE glad::glad)
target_link_libraries(app PRIVATE glm::glm)
target_link_libraries(app PRIVATE FastNoise2) # Not support via vcpkg