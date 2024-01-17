CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)

PROJECT(Qcap VERSION 0.1.0)#设置工程名称和版本
SET(CMAKE_BUILD_TYPE "Debug")#设置编译模式为debug版本，可以gdb调试

SET (CMAKE_CXX_STANDARD 17)
#asan编译选项暂时无法运行
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize-recover=address -fno-omit-frame-pointer -fsanitize=leak -fsanitize=undefined")
MESSAGE(STATUS "PROJECT_BINARY_DIR =  " ${PROJECT_BINARY_DIR})#输出提示信息,二进制文件的目录
MESSAGE(STATUS "PROJECT_SOURCE_DIR =  " ${PROJECT_SOURCE_DIR})#输出提示信息,工程文件的目录
MESSAGE(STATUS "CMAKE_SOURCE_DIR   =  " ${CMAKE_SOURCE_DIR})#输出提示信息,工程文件的目录

INCLUDE_DIRECTORIES(/usr/local/include)#添加头文件搜索目录
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/thirdpart) #包含spdlog的头文件

find_package(PkgConfig REQUIRED)
pkg_check_modules(dpdk REQUIRED IMPORTED_TARGET libdpdk)

file(GLOB_RECURSE SRC_DIR_LIST "./src/*.cpp")
# add the executable 
add_executable(Qcap ${SRC_DIR_LIST}) #添加可执行文件

target_link_libraries(Qcap PRIVATE PkgConfig::dpdk ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(Qcap PRIVATE stdc++fs)
target_link_libraries(Qcap PRIVATE pthread)