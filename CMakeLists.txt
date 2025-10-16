# 指定最低 CMake 版本要求
cmake_minimum_required(VERSION 3.10)

# 定义项目名称和语言
project(mymuduo LANGUAGES CXX)

# ============================================================
# 📦 输出路径设置
# ============================================================
# 编译生成的库和可执行文件的输出目录
set(LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

# ============================================================
# ⚙️ 编译配置
# ============================================================

# 使用 C++17 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)   # 强制要求支持 C++17，否则报错
set(CMAKE_CXX_EXTENSIONS OFF)         # 禁止使用 GNU 扩展，比如 -std=gnu++17，改为纯 -std=c++17
set(CMAKE_POSITION_INDEPENDENT_CODE ON)  # 自动加 -fPIC，生成可重定位代码（动态库需要）

# 默认构建类型（仅当用户未指定时）
if(NOT CMAKE_BUILD_TYPE)
    # CACHE 表示写入 CMake 缓存文件（CMakeCache.txt），下次仍然保留
    # FORCE 表示无论用户是否设置过，都强制写入 Debug
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# 根据不同构建类型添加编译参数
# -Wall: 打开大多数警告
# -Wextra: 打开额外警告（比 -Wall 更多）
# -Werror: 将所有警告视为错误（代码必须无警告才能通过）
# -O0/-O3: 优化级别
# -g: 含调试符号，用于 gdb 调试
set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g -Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -Wall -Wextra")

# ============================================================
# 🔍 源码收集
# ============================================================

# 自动查找源码文件（支持 .cpp/.cc）
# 你也可以指定 src 目录，例如 file(GLOB SRC_LIST "src/*.cpp")
file(GLOB SRC_LIST
    "${PROJECT_SOURCE_DIR}/src/logger.cpp"
    "${PROJECT_SOURCE_DIR}/src/timestamp.cpp"
)

# ============================================================
# 🏗️ 动态库构建
# ============================================================

# 生成动态库 libmymuduo.so
add_library(mymuduo SHARED ${SRC_LIST})

# 指定头文件搜索路径
# PUBLIC 表示：
#   1. 当前目标可以用
#   2. 其他依赖此库的目标也会继承这个 include 路径
target_include_directories(mymuduo PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

# ============================================================
# 🧱 编译选项（额外）
# ============================================================

# target_compile_options 只对当前 target 生效，不污染全局
# PRIVATE 表示仅当前目标使用这些选项
target_compile_options(mymuduo PRIVATE
    -Wall        # 打开常见警告（建议开启）
    -Wextra      # 打开更多警告（推荐）
    -Werror      # 将警告视为错误，强制写出高质量代码
)

# ============================================================
# 📥 安装规则（可选）
# ============================================================

# 使得执行 `make install` 时：
#   1. 库文件会被安装到 /usr/local/lib
#   2. 头文件会被安装到 /usr/local/include/mymuduo
#   （默认安装路径可以通过 cmake -DCMAKE_INSTALL_PREFIX=/xxx 修改）
install(TARGETS mymuduo
    LIBRARY DESTINATION lib      # 动态库安装路径
    ARCHIVE DESTINATION lib      # 静态库安装路径
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include)

# ============================================================
# 🧾 打印构建信息（方便调试）
# ============================================================

message(STATUS "===================================================")
message(STATUS " Project Name:     ${PROJECT_NAME}")
message(STATUS " C++ Standard:     ${CMAKE_CXX_STANDARD}")
message(STATUS " Build Type:       ${CMAKE_BUILD_TYPE}")
message(STATUS " Output Library:   ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
message(STATUS "===================================================")
