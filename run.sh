#!/bin/bash
# 获取当前脚本所在目录
SCRIPT_DIR=$(dirname "$(realpath "$0")")
# 从当前目录开始，向上查找直到找到 CMakeLists.txt 文件，确定项目根目录
PROJECT_DIR=$SCRIPT_DIR
while [ ! -f "$PROJECT_DIR/CMakeLists.txt" ]; do
PROJECT_DIR=$(dirname "$PROJECT_DIR")
done
# 获取项目根目录名
PROJECT_NAME=$(basename "$PROJECT_DIR")
echo "Project root directory name: $PROJECT_NAME"

# 创建并进入构建目录
BUILD_DIR="$PROJECT_DIR/build"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# 运行 Cmake 配置和编译项目
cmake ..
make -j 8

# 根据项目名称生成 ELF，BIN 和 HEX 文件路径
ELF_FILE="$BUILD_DIR/${PROJECT_NAME}.elf"
BIN_FILE="$BUILD_DIR/${PROJECT_NAME}.bin"
HEX_FILE="$BUILD_DIR/${PROJECT_NAME}.hex"
# 检查 ELF 文件是否 成功 生成
if [ -f "$ELF_FILE" ]; then
  # 将 ELF 文件转换为 BIN 文件和 HEX 文件
  arm-none-eabi-objcopy -O binary "$ELF_FILE" "$BIN_FILE"
  arm-none-eabi-objcopy -O ihex "$ELF_FILE" "$HEX_FILE"
  echo "Conversion to BIN and HEX completed"
else
  echo "Error: ELF file not found. Compilation might have failed."
  exit 1
fi

# 在 Windows Git Bash 中运行时，转换路径格式
if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
  # 使用 cygpath 转换 HEX_FILE 路径为 Windows 格式
  HEX_FILE_WIN=$(cygpath -w "$HEX_FILE")
  FLASH_CFG_WIN=$(cygpath -w "$PROJECT_DIR/flash.cfg")
  # 将Windows路径中的反斜杠转换为正斜杠
  HEX_FILE_WIN=${HEX_FILE_WIN//\\/\/}
  FLASH_CFG_WIN=${FLASH_CFG_WIN//\\/\/}
  echo "Running on Windows Git Bash, converted path: $HEX_FILE_WIN"
  # 使用转换后的 Windows 路径
  openocd -f "$FLASH_CFG_WIN" -c "program \"$HEX_FILE_WIN\" verify reset exit"
else
  # 在 Linux 或 macOS 上使用原始路径
  echo "Using HEX file: $HEX_FILE"
  openocd -f "$PROJECT_DIR/flash.cfg" -c "program \"$HEX_FILE\" verify reset exit"
fi