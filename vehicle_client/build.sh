#!/bin/bash

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
BUILD_DIR="${SCRIPT_DIR}/build"

mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

cmake ..
if [ $? -ne 0 ]; then
    echo "cmake 执行失败"
    exit 1
fi

make -j16
if [ $? -ne 0 ]; then
    echo "make 执行失败"
    exit 1
fi

echo "编译完成"

# 部署到龙芯板
scp -O sender root@172.20.10.2:/home/root/
if [ $? -eq 0 ]; then
    echo "部署完成 -> root@172.20.10.2:/home/root/sender"
else
    echo "scp 失败，请检查板子网络"
fi
