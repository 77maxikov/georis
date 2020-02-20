cmake_minimum_required(VERSION 2.8) # Проверка версии CMake.
set(SRC ../src)
include_directories(../include)
add_executable(geos ${SRC}/test.cpp ${SRC}/constraints.cpp)

