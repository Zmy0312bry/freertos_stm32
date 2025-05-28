# 程序主入口
`Core\Src\freertos.c`

# 启动烧录
# 全部执行（先构建再烧录）
```bash
./run.sh

# 全部执行（Debug模式）
./run.sh debug

# 仅构建
./run.sh build

# 仅构建（Debug模式）
./run.sh build debug

# 仅烧录
./run.sh flash

# 仅烧录指定文件
./run.sh flash path/to/firmware.hex

# 清理构建文件
./run.sh clean
```

# 增加Utils部分

在`cmake\smartcar`目录下的CMakeLists.txt下增加
```cmake
target_sources(smartcar INTERFACE
    ../../Utils/motor.c   # 增加相应的.c文件
)
```
在`Utils/`下创建控制模块

在根目录下的CMakeLists.txt下修改如下
```diff
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
+   smartcar
    # Add user defined libraries
)
```

