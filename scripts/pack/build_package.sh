#!/bin/bash
# Template Project 打包脚本

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}  Template Project Package Builder  ${NC}"
echo -e "${BLUE}======================================${NC}"

# 检查是否在项目根目录
if [ ! -f "CMakeLists.txt" ]; then
    echo -e "${RED}Error: Please run this script from the project root directory${NC}"
    exit 1
fi

# 清理之前的构建
echo -e "${YELLOW}Cleaning previous build...${NC}"
rm -rf build
rm -f *.deb

# 创建构建目录
echo -e "${YELLOW}Creating build directory...${NC}"
mkdir -p build
cd build

# 配置项目
echo -e "${YELLOW}Configuring project...${NC}"
cmake -DCMAKE_BUILD_TYPE=Release ..

# 编译项目
echo -e "${YELLOW}Building project...${NC}"
make -j$(nproc)

# 创建 .deb 包
echo -e "${YELLOW}Creating DEB package...${NC}"
make package

# 检查生成的包
echo -e "${GREEN}Package created successfully!${NC}"
ls -la *.deb

# 移动包到项目根目录
mv *.deb ../

cd ..

echo -e "${GREEN}======================================${NC}"
echo -e "${GREEN}Package build completed!${NC}"
echo -e "${GREEN}======================================${NC}"

# 显示包信息
if [ -f *.deb ]; then
    echo -e "${BLUE}Package Information:${NC}"
    dpkg-deb --info *.deb
    
    echo -e "${BLUE}Package Contents:${NC}"
    dpkg-deb --contents *.deb
    
    echo -e "${BLUE}Control Scripts:${NC}"
    dpkg-deb --control *.deb /tmp/template-project-control
    ls -la /tmp/template-project-control/
    rm -rf /tmp/template-project-control
fi

echo -e "${GREEN}To install the package, run:${NC}"
echo -e "${GREEN}sudo dpkg -i $(ls *.deb)${NC}"
echo -e "${GREEN}To remove the package, run:${NC}"
echo -e "${GREEN}sudo dpkg -r template-project${NC}" 