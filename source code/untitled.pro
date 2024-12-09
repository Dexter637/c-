TEMPLATE = app
CONFIG += console c++11
QT += core gui network widgets
TARGET = untitled
CONFIG += debug_and_release
CONFIG += qmake_default_deployment
SOURCES += main.cpp \
   SetupManager.cpp
HEADERS += \
   SetupManager.h
# 指定清单文件
manifest.input = $$PWD/your_manifest_file.xml
# 指定清单文件
RC_FILE = your_manifest_file.rc
CONFIG += deploy
qmake_default_deployment
DEPLOYMENTFOLDERS = app
win32: DEPLOYMENTFOLDERS += plugins
