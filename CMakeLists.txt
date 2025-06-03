cmake_minimum_required(VERSION 3.16)
project(voxelEngine)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 23)

# Get packages
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# Add source files
add_executable(app src/main.cpp src/shader.cpp src/camera.cpp)

# Link libraries
target_link_libraries(app glfw)
target_link_libraries(app glad::glad)
target_link_libraries(app glm::glm)