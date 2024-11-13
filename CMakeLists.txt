cmake_minimum_required(VERSION 3.22)

project(voxelvis)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON )
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

if(MSVC)
    add_compile_options("/source-charset:utf-8")
    add_compile_options("/execution-charset:gbk")
endif(MSVC)

# opengl 460
find_package(OpenGL REQUIRED)
# python3.8 python1.24.4
set(Python_ROOT_DIR "${CMAKE_SOURCE_DIR}/.venv")
find_package(Python COMPONENTS Interpreter Development NumPy)
if(Python_FOUND)
message(STATUS "python execture path: ${Python_EXECUTABLE}")
message(STATUS "python include path : ${Python_INCLUDE_DIRS}")
message(STATUS "python library path : ${Python_LIBRARIES}")
include_directories(${Python_INCLUDE_DIRS})
endif(Python_FOUND)

if(Python_NumPy_FOUND)
message(STATUS "numpy include path : ${Python_NumPy_INCLUDE_DIRS}")
message(STATUS "numpy version : ${Python_NumPy_VERSION}")
include_directories(${Python_NumPy_INCLUDE_DIRS})
endif(Python_NumPy_FOUND)


set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLM_ENABLE_CXX_17 ON CACHE BOOL "" FORCE)
# glfw
add_subdirectory(3rdparty/glfw)
# glad
add_subdirectory(3rdparty/glad)
# glm
add_subdirectory(3rdparty/glm)
#imgui
set(LINKED_LIBS OpenGL::GL glfw)
include_directories(3rdparty/imgui 3rdparty/imgui/backends 3rdparty/glfw/include 3rdparty/glad/include)
set(IMGUI_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui)
set(IMGUI_SOURCE_FILES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp

    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)
# static lib
add_library(imgui STATIC ${IMGUI_SOURCE_FILES})
# pybind11
find_package(pybind11 REQUIRED PATHS "${Python_ROOT_DIR}/Lib/site-packages/pybind11")

# set libpath and name
link_directories(${Python_LIBRARY_DIRS})
set(VOXELVIS_Libs OpenGL::GL glfw glad glm::glm ${Python_LIBRARIES} imgui)
########################################################################

include_directories(src)
# vis app
add_executable(my_app main.cpp src/voxelvis.cpp src/instance.cpp src/common.cpp src/camera.cpp src/auxgrid.cpp)

# 添加动态库
# add_library(example_module SHARED src/example.cpp)
# target_link_libraries(example_module pybind11::module)
# cmake方式打包pybind11
pybind11_add_module(example_module example.cpp src/voxelvis.cpp src/instance.cpp src/common.cpp src/camera.cpp src/auxgrid.cpp)

# TODO 彻底搞清public private
target_link_libraries(my_app PRIVATE ${VOXELVIS_Libs})
# TODO target_link_libraries的用处为什么会联系上include
target_link_libraries(example_module PRIVATE ${VOXELVIS_Libs})



# Python 包打包指令，调用 setup.py
# add_custom_target(python_package ALL
#     COMMAND ${CMAKE_COMMAND} -E env
#     PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}
#     ${Python_EXECUTABLE} setup.py build_ext --inplace
#     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#     COMMENT "Building Python package"
# )
# # 依赖关系
# add_dependencies(python_package example_module)