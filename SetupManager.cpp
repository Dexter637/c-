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
    // Output the prompt message that all configurations are completed to the command line
    std::cout << "Current step: All configurations completed, setup is finished" << std::endl;
    std::cout << "Setup Complete: You can now use VSCode for C++ development." << std::endl;
}

void SetupManager::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::CrashExit) {
        std::cerr << "Process error: Process crashed" << std::endl;
    } else if (exitCode!= 0) {
        std::cerr << "Process error: Process exited with error code " << exitCode << std::endl;
    }
}
