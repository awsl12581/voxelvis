cmake_minimum_required(VERSION 3.22)

project(voxelvis)

enable_testing()

if(LINUX)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif(LINUX)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON )
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
# opengl 460
find_package(OpenGL REQUIRED)
# python3.8 python1.24.4
option(Python_ROOT_DIR "Path to the Python root directory" "${CMAKE_SOURCE_DIR}/.venv")
find_package(Python COMPONENTS Interpreter Development NumPy)
if(Python_FOUND)
message(STATUS "python execture path: ${Python_EXECUTABLE}")
message(STATUS "python include path : ${Python_INCLUDE_DIRS}")
message(STATUS "python library path : ${Python_LIBRARIES}")
message(STATUS "Python version: ${Python_VERSION}")
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
# test
# add_subdirectory(tests)
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
if(MSVC)
    find_package(pybind11 REQUIRED PATHS "${Python_ROOT_DIR}/Lib/site-packages/pybind11")
elseif(LINUX)
    # 使用字符串操作截取前两个版本号
    string(REGEX MATCH "^([0-9]+\\.[0-9]+)" PYTHON_MAJOR_MINOR "${Python_VERSION}")

    # 输出截取后的 Python 版本
    message(STATUS "Python major and minor version: ${PYTHON_MAJOR_MINOR}")
    find_package(pybind11 REQUIRED PATHS "${Python_ROOT_DIR}/lib/python${PYTHON_MAJOR_MINOR}/site-packages/pybind11")
else()
# win
    find_package(pybind11 REQUIRED PATHS "${Python_ROOT_DIR}/Lib/site-packages/pybind11")
endif()


# set libpath and name
link_directories(${Python_LIBRARY_DIRS})
set(VOXELVIS_Libs OpenGL::GL glfw glad glm::glm ${Python_LIBRARIES} imgui)
########################################################################

include_directories(src)

# 不要在库编译前设置，除非必须，直接在项目编译前加即可
if(MSVC)
    add_compile_options("/source-charset:utf-8")
    add_compile_options("/execution-charset:gbk")
endif(MSVC)

if(LINUX)
find_package(Threads REQUIRED)
endif(LINUX)


set(Vis_Source_Files
    src/capture.cpp
    src/voxelvis.cpp
    src/instance.cpp
    src/common.cpp
    src/camera.cpp
    src/auxgrid.cpp
)
# vis app
add_executable(my_app main.cpp ${Vis_Source_Files})
# TODO 彻底搞清public private
target_link_libraries(my_app PRIVATE ${VOXELVIS_Libs})
if(LINUX)
target_link_libraries(my_app PRIVATE Threads::Threads)
endif(LINUX)

# find_package(X11 REQUIRED)
# x11

# if(LINUX)
# add_executable(my_app_x11 src/imgui_impl_x11_main.cpp src/imgui_impl_x11.cpp)
# target_link_libraries(my_app_x11 PRIVATE ${VOXELVIS_Libs})
# target_link_libraries(my_app_x11 PRIVATE Threads::Threads X11)
# endif(LINUX)

# 添加动态库
# add_library(teavoxelui SHARED src/example.cpp)
# target_link_libraries(teavoxelui pybind11::module)
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()
# cmake方式打包pybind11
pybind11_add_module(teavoxelui example.cpp ${Vis_Source_Files})
# TODO target_link_libraries的用处为什么会联系上include
target_link_libraries(teavoxelui PRIVATE ${VOXELVIS_Libs})
if(LINUX)
target_link_libraries(teavoxelui PRIVATE Threads::Threads X11)
endif(LINUX)



add_custom_command(TARGET teavoxelui POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E env PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}
    ${Python_EXECUTABLE} -m pybind11_stubgen teavoxelui --output ${CMAKE_CURRENT_BINARY_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating .pyi file for teavoxelui"
)
