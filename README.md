# Voxel Game Engine

## Prerequisites
This projects runs using C++, CMake, and vcpkg. 

Using vcpkg, the following libraries were installed:
- glfw3
- glad

## Setup

### CMake Presets
1. Create a `CMakeUserPresets.json` using https://learn.microsoft.com/en-us/vcpkg/get_started/get-started
2. Set the generator in `CMakePresets.json` based on local environment

### Build
```
cmake --preset=default
cmake --build build
```