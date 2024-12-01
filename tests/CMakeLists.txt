cmake_minimum_required(VERSION 3.22)

include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.15.2 # 替换为需要的版本
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
if(MSVC)
  string(REPLACE "-utf-8" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif(MSVC)


FetchContent_MakeAvailable(googletest)

add_library(GTest::GTest INTERFACE IMPORTED)
# 链接gtest
target_link_libraries(GTest::GTest INTERFACE gtest_main)

# 将我们写的tests编译成可执行文件
add_executable(multiply_test multiply_test.cpp)

# 把GTest和被测函数都链接到 可执行文件 上
target_link_libraries(multiply_test PRIVATE GTest::GTest)

# cmake原生测试
# add_test(multiply_gtests multiply_test)
# gtest封装，自动添加和扫描
include(GoogleTest)
gtest_add_tests(TARGET      multiply_test
                TEST_SUFFIX .noArgs
                TEST_LIST   noArgsTests
)

add_executable(sb_thread sb_thread.cpp)