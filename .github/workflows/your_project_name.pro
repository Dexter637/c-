QT += core gui 
TARGET = DeployTool
CONFIG += c++17
CONFIG += msvcrt  # 添加运行时库配置，指定使用Visual Studio默认的动态链接运行时库（可根据项目实际需求调整）
# 添加其他必要的 Qt 模块
QT += widgets
# 添加 manifest 文件
QMAKE_LFLAGS += /MANIFEST /MANIFESTFILE:your_app_name.exe.manifest  # 将文件名修改为与TARGET一致，确保文件名匹配（假设你的可执行文件名为DeployTool.exe）
DESTDIR = build  # 添加这行，指定构建产物输出目录为 build 目录
SOURCES += \
    main.cpp \
    your_source_file.cpp
HEADERS += \
    your_header_file.h
FORMS += \
    your_form.ui
