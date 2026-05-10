#!/bin/bash
echo "🚀 构建汴京世界..."

# 清理旧构建
rm -rf build
mkdir -p build
cd build

# 配置和构建
echo "🔧 运行CMake..."
cmake .. || { echo "❌ CMake配置失败"; exit 1; }

echo "🔨 编译项目..."
make -j4 || { echo "❌ 编译失败"; exit 1; }

echo "🎮 启动汴京世界..."
./huilin