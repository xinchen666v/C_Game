cmake_minimum_required(VERSION 3.28)
project(C_Game)

set(CMAKE_CXX_STANDARD 20)

include_directories(./EasyX/include)
link_directories(./EasyX/lib64)
link_libraries(libeasyx.a)


add_executable(C_Game main.cpp)
target_link_libraries(C_Game msimg32) # 或者 gdi32，取决于你的函数在哪个库中
