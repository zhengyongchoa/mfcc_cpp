# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.1)

PROJECT(mfcc_cpp)                                             #定义工程名称

#SET THE BUILD TYPE HERE, Release, Debug
SET(CMAKE_BUILD_TYPE Debug)


SET(CMAKE_C_FLAGS_DEBUG "-g -Wall")          # 指定编译器 
                                             # CMAKE_C_FLAGS_DEBUG            ----  C 编译器
                                             # CMAKE_CXX_FLAGS_DEBUG        ----  C++ 编译器
                                             # -g：只是编译器，在编译的时候，产生调试信息。
                                             # -Wall：生成所有警告信息。一下是具体的选项，可以单独使用


# 查找当前目录下的所有源文件
add_subdirectory(libmfcc)

# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(. DIR_SRCS)
# 指定生成目标
add_executable(Demo ${DIR_SRCS})

target_link_libraries(Demo libmfcc -lm)