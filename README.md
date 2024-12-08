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

## some issue and solution

if you find some issue when you run the version of python like this:
```bash
libGL error: MESA-LOADER: failed to open swrast: /lib/x86_64-linux-gnu/libLLVM-12.so.1: undefined symbol: ffi_type_sint32, version LIBFFI_BASE_7.0 (search paths /usr/lib/x86_64-linux-gnu/dri:\$${ORIGIN}/dri:/usr/lib/dri, suffix _dri)
libGL error: failed to load driver: swrast
glfw error 65543: GLX: Failed to create context: GLXBadFBConfig
can not create window
```
you can solve this problem by:
```bash
sudo ln -s /usr/lib/x86_64-linux-gnu/dri/radeonsi_dri.so /usr/lib/dri/
sudo ln -s /usr/lib/x86_64-linux-gnu/dri/swrast_dri.so /usr/lib/dri/

```
or
```bash
conda install -c conda-forge gcc
```
And maybe you will find this problem:
```bash
libGL error: MESA-LOADER: failed to open swrast: /lib/x86_64-linux-gnu/libLLVM-12.so.1: undefined symbol: ffi_type_sint32, version LIBFFI_BASE_7.0 (search paths /usr/lib/x86_64-linux-gnu/dri:\$${ORIGIN}/dri:/usr/lib/dri, suffix _dri)
libGL error: failed to load driver: swrast
glfw error 65543: GLX: Failed to create context: GLXBadFBConfig
can not create window
```
a simple method is:
```bash
export LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libffi.so.7
```