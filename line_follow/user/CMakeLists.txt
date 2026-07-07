cmake_minimum_required(VERSION 3.5.0)

# 基础路径与项目名称配置
set(CURRENT_DIR ${CMAKE_CURRENT_LIST_DIR})
get_filename_component(PARENT_DIR ${CURRENT_DIR}/.. ABSOLUTE)
get_filename_component(PROJ_NAME ${PARENT_DIR} NAME)           # 取上级目录名作为项目名
project(${PROJ_NAME} VERSION 0.1.0 LANGUAGES C CXX)

# 编译参数（龙芯LoongArch适配）
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 通用编译优化与龙芯架构参数
set(COMMON_FLAGS "-O3 -march=loongarch64 -mtune=loongarch64 -fPIC -ffunction-sections -fdata-sections -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS} -pthread -Wno-delete-non-virtual-dtor -Wno-placement-new")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")

# 交叉编译工具链
include(cross.cmake)

# OpenCV依赖配置
set(CMAKE_PREFIX_PATH "/opt/ls_2k0300_env/opencv_4_10_build")
find_package(OpenCV REQUIRED)
message(STATUS "OpenCV_INCLUDE: ${OpenCV_INCLUDE_DIRS}, OpenCV_LIBS: ${OpenCV_LIBS}")

# 头文件路径汇总（拆分TFLM路径）
include_directories(
        ${OpenCV_INCLUDE_DIRS}
        # 业务代码头文件
        ../user
        ../code/imgprocessing
        ../code/inc
        ../code/src
        ../model
        ../../deps/zf_common
        ../../deps/zf_device
        ../../deps/zf_driver
        ../../deps/zf_components/seekfree_assistant

        # TFLM核心头文件（完全拆分）
        ../../deps/zf_components/tflm
        ../../deps/zf_components/tflm/third_party/flatbuffers/include
        ../../deps/zf_components/tflm/third_party/gemmlowp
        ../../deps/zf_components/tflm/third_party/gemmlowp/fixedpoint
        ../../deps/zf_components/tflm/third_party/ruy
        ../../deps/zf_components/tflm/third_party/ruy/ruy
        ../../deps/zf_components/tflm/tensorflow/lite/micro
        ../../deps/zf_components/tflm/tensorflow/lite/micro/kernels
        ../../deps/zf_components/tflm/tensorflow/lite/kernels/internal
        ../../deps/zf_components/tflm/tensorflow/lite/core/api
        ../../deps/zf_components/tflm/tensorflow/lite/core/c
        ../../deps/zf_components/tflm/tensorflow/lite/schema
        ../../deps/zf_components/ncnn/include/ncnn
)

# 静态库路径
link_directories(
        ../../deps/zf_components/tflm/lib
        ../../deps/zf_components/ncnn/lib
)

# 业务源文件
aux_source_directory(../user DIR_SRCS)
aux_source_directory(../code/imgprocessing DIR_SRCS)  # ✅ 图像处理
aux_source_directory(../code/src DIR_SRCS)            # ✅ 业务逻辑（pid.cpp, Menu.cpp, GPIO.cpp等）
aux_source_directory(../model DIR_SRCS)
aux_source_directory(../../deps/zf_common DIR_SRCS)
aux_source_directory(../../deps/zf_device DIR_SRCS)
aux_source_directory(../../deps/zf_driver DIR_SRCS)
aux_source_directory(../../deps/zf_components/seekfree_assistant DIR_SRCS)

# 生成可执行文件 & 配置属性
add_executable(${PROJ_NAME} ${DIR_SRCS}
        ../code/inc/usart.hpp
        ../code/src/usart.cpp)
set_target_properties(${PROJ_NAME} PROPERTIES
        INSTALL_RPATH "/opt/ls_2k0300_env/opencv_4_10_build/lib"
        BUILD_WITH_INSTALL_RPATH ON
)

# 链接依赖库
target_link_libraries(
        ${PROJ_NAME}
        ${OpenCV_LIBS}
)