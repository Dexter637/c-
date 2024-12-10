#include "SetupManager.h"
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QSettings>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <iostream>
#include <locale>

SetupManager::SetupManager(QObject *parent) : QObject(parent)
{
    connect(&manager, &QNetworkAccessManager::finished, this, &SetupManager::onDownloadFinished);
    connect(&process, &QProcess::finished, this, &SetupManager::onProcessFinished);
}

void SetupManager::startSetup()
{
    if (!requestAdminRights()) {
        std::cerr << "Error: This application requires administrator privileges." << std::endl;
        return;
    }
    // 先在桌面创建 vscode 文件夹，再在其内部创建.vscode 文件夹
    createVSCodeFolders();
    // Output the prompt message that MinGW download is starting to the command line
    std::cout << "Current step: Start downloading MinGW" << std::endl;
    downloadMinGW();
}

bool SetupManager::requestAdminRights()
{
    process.start("net session");
    process.waitForFinished();
    return process.exitCode() == 0;
}

// 函数用于创建桌面的 vscode 文件夹以及其内部的.vscode 文件夹
void SetupManager::createVSCodeFolders()
{
    QDir desktopDir = QDir::home();
    desktopDir.cd("Desktop");
    if (!desktopDir.exists("vscode")) {
        desktopDir.mkdir("vscode");
    }
    QDir vscodeDir(desktopDir.absoluteFilePath("vscode"));
    if (!vscodeDir.exists(".vscode")) {
        vscodeDir.mkpath(".vscode");
    }
}

void SetupManager::downloadMinGW()
{
    QUrl url("https://nchc.dl.sourceforge.net/project/mingw/Installer/mingw-get-setup.exe?viasf=1");
    manager.get(QNetworkRequest(url));
}

void SetupManager::onDownloadFinished(QNetworkReply *reply)
{
    if (reply->error()) {
        std::cerr << "Download error: " << reply->errorString().toStdString() << std::endl;
        return;
    }
    QFile file("mingw-get-setup.exe");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(reply->readAll());
        file.close();
        // Output the prompt message that MinGW download is completed and extraction and configuration are starting to the command line
        std::cout << "Current step: MinGW download completed, start extracting and configuring" << std::endl;
        extractAndConfigureMinGW();
    }
}

void SetupManager::extractAndConfigureMinGW()
{
    // Output the prompt message that MinGW is being extracted and configured to the command line
    std::cout << "Current step: Extracting and configuring MinGW" << std::endl;
    process.start("mingw-get-setup.exe", QStringList() << "/S");
    process.waitForFinished();
    QSettings settings("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment", QSettings::NativeFormat);
    QString path = settings.value("Path", "").toString();
    path += ";C:\\MinGW\\bin";
    settings.setValue("Path", path);
    // Output the prompt message that MinGW configuration is completed and VSCode download is starting to the command line
    std::cout << "Current step: MinGW configuration completed, start downloading VSCode" << std::endl;
    downloadVSCode();
}

void SetupManager::downloadVSCode()
{
    // Output the prompt message that VSCode download is starting to the command line
    std::cout << "Current step: Start downloading VSCode" << std::endl;
    QUrl url("https://update.code.visualstudio.com/latest/win32-x64-user/stable");
    QNetworkRequest request(url);
    QNetworkReply *reply = manager.get(request);
    // Use lambda expression to connect the signal and slot
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error()) {
            std::cerr << "Download error: " << reply->errorString().toStdString() << std::endl;
            return;
        }
        QFile file("VSCodeSetup.exe");
        if (file.open(QIODevice::WriteOnly)) {
            file.write(reply->readAll());
            file.close();
            // Output the prompt message that VSCode download is completed and installation is starting to the command line
            std::cout << "Current step: VSCode download completed, start installing" << std::endl;
            installVSCode();
        }
    });
}

void SetupManager::onVSCodeDownloadFinished(QNetworkReply *reply)
{
    if (reply->error()) {
        std::cerr << "Download error: " << reply->errorString().toStdString() << std::endl;
        return;
    }
    QFile file("VSCodeSetup.exe");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(reply->readAll());
        file.close();
        installVSCode();
    }
}

void SetupManager::installVSCode()
{
    // Output the prompt message that VSCode is being installed to the command line
    std::cout << "Current step: Installing VSCode" << std::endl;
    process.start("VSCodeSetup.exe", QStringList() << "/S");
    process.waitForFinished();
    // Output the prompt message that VSCode installation is completed and configuration is starting to the command line
    std::cout << "Current step: VSCode installation completed, start configuring" << std::endl;
    configureVSCode();
}

void SetupManager::configureVSCode()
{
    // Output the prompt message that VSCode is being configured to the command line
    std::cout << "Current step: Configuring VSCode" << std::endl;
    process.start("code", QStringList() << "--install-extension" << "ms-vscode.cpptools");
    process.waitForFinished();

    // 写入 settings.json 文件配置内容
    writeSettingsJsonConfig();

    // 写入 tasks.json 文件配置内容
    writeTasksJsonConfig();

    // 写入 launch.json 文件配置内容
    writeLaunchJsonConfig();

    // 写入 c_cpp_properties.json 文件配置内容
    writeCppPropertiesJsonConfig();

    // 配置完成后清理临时文件
    cleanTempFiles();

    // 配置完成后输出提示信息给用户
    std::cout << "Your desktop will have a 'vscode' folder. Please open this folder in VSCode and create your first project in the top-level directory." << std::endl;

    // Output the prompt message that all configurations are completed to the command line
    std::cout << "Current step: All configurations completed, setup is finished" << std::endl;
    std::cout << "Setup Complete: You can now use VSCode for C++ development." << std::endl;
}

// 用于写入 c_cpp_properties.json 配置内容的函数，按照之前要求的配置内容设置
void SetupManager::writeCppPropertiesJsonConfig()
{
    QDir vscodeDir(QDir::home().absoluteFilePath("Desktop/vscode/.vscode"));
    QFile file(vscodeDir.absoluteFilePath("c_cpp_properties.json"));
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QString config = R"({
    "configurations": [
      {
        "name": "Win64",
        "includePath": ["${workspaceFolder}/**"],
        "defines": ["_DEBUG", "UNICODE", "_UNICODE"],
        "windowsSdkVersion": "10.0.18362.0",
        "compilerPath": "C:/MinGW/bin/g++.exe",
        "cStandard": "c17",
        "cppStandard": "c++17",
        "intelliSenseMode": "gcc-x64"
      }
    ],
    "version": 4
  })";
        file.write(config.toUtf8());
        file.close();
    }
}

// 用于写入 settings.json 配置内容的函数，仅为示意，实际需完善逻辑，比如创建文件夹、处理文件写入错误等情况
void SetupManager::writeSettingsJsonConfig()
{
    QDir vscodeDir(QDir::home().absoluteFilePath("Desktop/vscode/.vscode"));
    QFile file(vscodeDir.absoluteFilePath("settings.json"));
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QString config = R"({
    "files.defaultLanguage": "c",
    "editor.formatOnType": true,
    "editor.suggest.snippetsPreventQuickSuggestions": false,
    "editor.acceptSuggestionOnEnter": "off",
    "code-runner.runInTerminal": true,
    "code-runner.executorMap": {
        "c": "gcc '$fileName' -o '$fileNameWithoutExt.exe' -Wall -O2 -m64 -lm -static-libgcc -fexec-charset=GBK -D__USE_MINGW_ANSI_STDIO && &'./$fileNameWithoutExt.exe'",
        "cpp": "g++ '$fileName' -o '$fileNameWithoutExt.exe' -Wall -O2 -m64 -static-libgcc -fexec-charset=GBK && &'./$fileNameWithoutExt.exe'"
    },
    "code-runner.saveFileBeforeRun": true,
    "code-runner.preserveFocus": true,
    "code-runner.clearPreviousOutput": false,
    "code-runner.ignoreSelection": true,
    "code-runner.fileDirectoryAsCwd": true,
    "C_Cpp.clang_format_sortIncludes": true
})";
        file.write(config.toUtf8());
        file.close();
    }
}

// 用于写入 tasks.json 配置内容的函数，仅为示意，实际需完善逻辑，比如创建文件夹、处理文件写入错误等情况
void SetupManager::writeTasksJsonConfig()
{
    QDir vscodeDir(QDir::home().absoluteFilePath("Desktop/vscode/.vscode"));
    QFile file(vscodeDir.absoluteFilePath("tasks.json"));
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QString config = R"({
    "tasks": [
        {
            "type": "cppbuild",
            "label": "C/C++: g++.exe 生成活动文件",
            "command": "g++.exe",
            "args": [
                "-fdiagnostics-color=always",
                "-g",
                "${file}",
                "-static-libgcc",
                "-o",
                "${fileDirname}\\debug\\${fileBasenameNoExtension}.exe",
                "-fexec-charset=GBK"
            ],
            "options": {
                "cwd": "${fileDirname}"
            },
            "problemMatcher": [
                "$gcc"
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "detail": "调试器生成的任务。"
        }
    ],
    "version": "2.0.0"
})";
        file.write(config.toUtf8());
        file.close();
    }
}

// 用于写入 launch.json 配置内容的函数，仅为示意，实际需完善逻辑，比如创建文件夹、处理文件写入错误等情况
void SetupManager::writeLaunchJsonConfig()
{
    QDir vscodeDir(QDir::home().absoluteFilePath("Desktop/vscode/.vscode"));
    QFile file(vscodeDir.absoluteFilePath("launch.json"));
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QString config = R"({
    "version": "0.2.0",
    "configurations": [
        {
            "name": "g++.exe - 生成和调试活动文件",
            "type": "cppdbg",
            "request": "launch",
            "program": "${fileDirname}\\debug\\${fileBasenameNoExtension}.exe",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${fileDirname}",
            "environment": [],
            "externalConsole": true,
            "MIMode": "gdb",
            "miDebuggerPath": "D:\\Aizo\\Tools\\mingw64\\bin\\gdb.exe",
            "setupCommands": [
                {
                    "description": "为 gdb 启用整齐打印",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                },
                {
                    "description": "将反汇编风格设置为 Intel",
                    "text": "-gdb-set disassembly-flavor intel",
                    "ignoreFailures": true
                }
            ],
            "preLaunchTask": "C/C++: g++.exe 生成活动文件"
        }
    ]
})";
        file.write(config.toUtf8());
        file.close();
    }
}

void SetupManager::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::CrashExit) {
        std::cerr << "Process error: Process crashed" << std::endl;
    } else if (exitCode!= 0) {
        std::cerr << "Process error: Process exited with error code " << exitCode << std::endl;
    }
}

// 清理临时文件的函数，删除下载的安装包文件等临时文件
void SetupManager::cleanTempFiles()
{
    QFile::remove("mingw-get-setup.exe");
    QFile::remove("VSCodeSetup.exe");
    // 这里还可以添加代码来删除其他可能产生的临时文件或文件夹等，根据实际情况扩展
}
