# 交叉编译工具链 (若不存在则为本地编译)
set(CROSS_COMPILING OFF)
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/cross.cmake")
    include("${CMAKE_CURRENT_LIST_DIR}/cross.cmake")
    set(CROSS_COMPILING ON)
endif()

cmake_minimum_required(VERSION 3.5.0)

get_filename_component(PROJ_NAME ${CMAKE_CURRENT_LIST_DIR} NAME)
project(${PROJ_NAME} VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 编译优化
if(CROSS_COMPILING)
    set(COMMON_FLAGS "-O3 -march=loongarch64 -mtune=loongarch64 -fPIC -ffunction-sections -fdata-sections -Wall")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")
    set(CMAKE_PREFIX_PATH "/opt/ls_2k0300_env/opencv_4_10_build")
    message(STATUS "交叉编译模式: LoongArch64")
else()
    set(COMMON_FLAGS "-O3 -fPIC -Wall")
    message(STATUS "本地编译模式: ${CMAKE_HOST_SYSTEM_PROCESSOR}")
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS} -pthread")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS}")

find_package(OpenCV REQUIRED)
message(STATUS "OpenCV: ${OpenCV_VERSION}")

file(GLOB SENDER_SRCS ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

add_executable(${PROJ_NAME} ${SENDER_SRCS})
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(${PROJ_NAME} ${OpenCV_LIBS})
