	QT += core gui
	TARGET = your_app_name
	CONFIG += c++11
	# 添加其他必要的 Qt 模块
	QT += widgets
	# 添加 manifest 文件
	QMAKE_LFLAGS += /MANIFEST /MANIFESTFILE:your_app_name.exe.manifest
	SOURCES += \
	   main.cpp \
	   your_source_file.cpp
	HEADERS += \
	   your_header_file.h
	FORMS += \
	   your_form.ui
