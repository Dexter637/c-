# Qt 项目文件

QT += core gui widgets  # 添加你项目所需的 Qt 模块

TARGET = DeployTool  # 设置目标名称
CONFIG += c++17  # 指定 C++ 版本
CONFIG += msvcrt  # 使用 Visual Studio 默认的运行时库（可根据项目需求调整）

# 添加 manifest 文件
QMAKE_LFLAGS += /MANIFEST /MANIFESTFILE:DeployTool.exe.manifest  # 将文件名修改为与目标一致

# 设置输出目录
DESTDIR = build  # 指定构建产物输出目录为 build 目录

# 添加源文件、头文件和 UI 文件
SOURCES += \
    main.cpp \
    your_source_file.cpp  # 添加你的源代码文件

HEADERS += \
    your_header_file.h  # 添加你的头文件

FORMS += \
    your_form.ui  # 添加 UI 文件（如果有的话）

# 添加其他必要的设置或依赖（如果有的话）
