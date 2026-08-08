cmake_minimum_required(VERSION 3.10)
project(Rasterizer)

find_package(OpenCV REQUIRED core imgproc highgui imgcodecs)

set(CMAKE_CXX_STANDARD 17)

# OpenCV 头文件路径（兼容 Windows/MSYS2 的 <prefix>/include/opencv4 布局）
include_directories(${OpenCV_INCLUDE_DIRS})
if(EXISTS "${OpenCV_INSTALL_PATH}/include/opencv4")
    include_directories("${OpenCV_INSTALL_PATH}/include/opencv4")
endif()

add_executable(Rasterizer main.cpp rasterizer.hpp rasterizer.cpp Triangle.hpp Triangle.cpp)
target_link_libraries(Rasterizer ${OpenCV_LIBRARIES})
