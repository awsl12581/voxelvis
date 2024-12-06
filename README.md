# voxel async vis by opengl

this module is used to display OCC voxel information on autodrive

## 3rdparty

- opengl==4.6.0
- glm==1.0.1
- glfw3==3.4.0
- glad==2.0.8
- imgui==1.91.5
  
All 3rd party libraries should be located in ./3rdparty

For glad, please set CMakeLists.txt in its source directory

```cmake
cmake_minimum_required(VERSION 3.22)
project(glad)

add_library(glad STATIC include/glad/glad.h src/glad.c)
target_include_directories(glad PUBLIC include)

```

## venv

- python >=3.8
- numpy >=1.26.1
- pybind11 >=2.13.6
- pybind11_stubgen

virtualvenv should be located in ./.venv
