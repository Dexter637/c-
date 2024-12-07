#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QTextBrowser>
#include <QProcess>
#include <QDebug>
#include <QMessageBox>
#include <QProcessEnvironment>
#include <QFile>
#include <QTextStream>
#include <QInputDialog>

class DeployTool : public QWidget {
    Q_OBJECT
public:
    DeployTool(QWidget *parent = nullptr) : QWidget(parent) {
        auto *layout = new QVBoxLayout(this);
        auto *label = new QLabel("Click to deploy VS Code C++ environment", this);
        layout->addWidget(label);
        auto *deployButton = new QPushButton("Deploy", this);
        layout->addWidget(deployButton);
        logBrowser = new QTextBrowser(this);
        layout->addWidget(logBrowser);
        connect(deployButton, &QPushButton::clicked, this, &DeployTool::deployEnvironment);
    }

private slots:
    void deployEnvironment() {
        logBrowser->append("Starting deployment...");
        // Step 1: Download VS Code installer
        logBrowser->append("Downloading VS Code installer...");
        if (!runProcess("powershell", QStringList() << "-Command" << "(New-Object Net.WebClient).DownloadFile('https://code.visualstudio.com/sha/download?build=stable&os=win32-x64', 'vscodeSetup.exe')")) {
            return;
        }
        logBrowser->append("VS Code installer downloaded.");
        // Step 2: Install VS Code
        logBrowser->append("Installing VS Code...");
        if (!runProcess("vscodeSetup.exe", QStringList() << "/S")) {
            return;
        }
        logBrowser->append("VS Code installed.");
        // Step 3: Install C++ extension
        logBrowser->append("Installing C++ extension...");
        if (!runProcess("powershell", QStringList() << "-Command" << "code --install-extension ms-vscode.cpptools")) {
            return;
        }
        logBrowser->append("C++ extension installed.");
        // Step 4: Set environment variables
        logBrowser->append("Setting environment variables...");
        if (!setEnvironmentVariable("Path", QString("%1;C:\\Program Files\\Microsoft VS Code\\bin").arg(qgetenv("Path")))) {
            return;
        }
        // Check deployment
        if (checkDeployment()) {
            logBrowser->append("Deployment completed successfully!");
            // 创建配置文件，可根据用户输入等更灵活配置
            createConfigFiles();
            QMessageBox::information(this, "Success", "Deployment completed successfully!");
        } else {
            logBrowser->append("Deployment failed. Please check the logs for details.");
        }
    }

private:
    bool runProcess(const QString &program, const QStringList &arguments) {
        QProcess process;
        process.start(program, arguments);
        process.waitForFinished();
        QString stdOutput = process.readAllStandardOutput();
        QString stdError = process.readAllStandardError();
        if (process.exitCode()!= 0) {
            logBrowser->append(QString("Failed to run: %1 %2").arg(program, arguments.join(" ")));
            showError(stdError, stdOutput);
            return false;
        }
        logBrowser->append(stdOutput);
        return true;
    }

    bool setEnvironmentVariable(const QString &variable, const QString &value) {
        QProcess process;
        process.start("cmd", QStringList() << "/C" << QString("setx %1 \"%2\" /M").arg(variable, value));
        process.waitForFinished();
        QString stdOutput = process.readAllStandardOutput();
        QString stdError = process.readAllStandardError();
        if (process.exitCode()!= 0) {
            logBrowser->append(QString("Failed to set environment variable %1.").arg(variable));
            showError(stdError, stdOutput);
            return false;
        }
        logBrowser->append(QString("Environment variable %1 set successfully.").arg(variable));
        return true;
    }

    void showError(const QString &stdError, const QString &stdOutput) {
        QString errorMessage = stdError;
        if (!stdOutput.isEmpty()) {
            errorMessage += "\nStandard Output:\n" + stdOutput;
        }
        QMessageBox::critical(this, "Error", errorMessage);
        qDebug() << "Error:" << errorMessage;
    }

    bool isVSCodeInstalled() {
        QProcess process;
        process.start("cmd", QStringList() << "/C" << "code --version");
        process.waitForFinished();
        return process.exitCode() == 0;
    }

    bool isCppExtensionInstalled() {
        QProcess process;
        process.start("cmd", QStringList() << "/C" << "code --list-extensions");
        process.waitForFinished();
        QString output = process.readAllStandardOutput();
        return output.contains("ms-vscode.cpptools");
    }

    bool checkDeployment() {
        logBrowser->append("Checking deployment...");
        // Check if VS Code is installed
        if (!isVSCodeInstalled()) {
            logBrowser->append("VS Code is not installed properly.");
            return false;
        } else {
            logBrowser->append("VS Code is installed.");
        }
        // Check if the C++ extension is installed
        if (!isCppExtensionInstalled()) {
            logBrowser->append("C++ extension is not installed properly.");
            return false;
        } else {
            logBrowser->append("C++ extension is installed.");
        }
        // Additional checks can be added here as needed
        logBrowser->append("Deployment checks passed.");
        return true;
    }

    // 创建c_cpp_properties.json文件，针对MinGW编译器配置更完善
    void createC_CppPropertiesJson()
    {
        QString compilerPath = "C:/MinGW/bin/gcc.exe";  // 假设MinGW安装在该默认路径，可根据实际修改
        QString cStandard;
        QString cppStandard;
        QStringList includePaths;

        // 询问C标准版本，提供常见选项让用户选择
        QStringList cStandards = {"c89", "c99", "c11", "c17", "c18"};
        bool ok;
        cStandard = QInputDialog::getItem(this, "C Standard", "Please choose the C standard:", cStandards, 2, false, &ok);  // 默认选择c11
        if (!ok) {
            logBrowser->append("Invalid input for C standard. Using default (c11).");
            cStandard = "c11";
        }

        // 询问C++标准版本，提供常见选项让用户选择
        QStringList cppStandards = {"c++98", "c++11", "c++14", "c++17", "c++20"};
        cppStandard = QInputDialog::getItem(this, "C++ Standard", "Please choose the C++ standard:", cppStandards, 3, false, &ok);  // 默认选择c++17
        if (!ok) {
            logBrowser->append("Invalid input for C++ standard. Using default (c++17).");
            cppStandard = "c++17";
        }

        // 询问包含路径，可多次添加，示例简化为一次输入逗号分隔的多个路径
        QString inputIncludePaths = QInputDialog::getText(this, "Include Paths", "Please enter the include paths separated by commas (e.g., C:/my_lib/include,C:/another_lib/include):");
        includePaths = inputIncludePaths.split(',', Qt::SkipEmptyParts);

        QFile file("c_cpp_properties.json");
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << "{\n";
            out << "    \"configurations\": [\n";
            out << "        {\n";
            out << QString("            \"name\": \"Win32_MinGW\",\n");
            out << "            \"includePath\": [\n";
            for (const QString &path : includePaths) {
                out << QString("                \"%1\",\n").arg(path.trimmed());
            }
            out << "                \"${workspaceFolder}/**\"\n";
            out << "            ],\n";
            out << "            \"defines\": [\n";
            out << "                \"_DEBUG\",\n";
            out << "                \"UNICODE\",\n";
            out << "                \"_UNICODE\"\n";
            out << "            ],\n";
            out << QString("            \"compilerPath\": \"%1\",\n").arg(compilerPath);
            out << QString("            \"cStandard\": \"%1\",\n").arg(cStandard);
            out << QString("            \"cppStandard\": \"%1\",\n").arg(cppStandard);
            out << "            \"intelliSenseMode\": \"gcc-x64\"\n";
            out << "        }\n";
            out << "    ],\n";
            out << "    \"version\": 4\n";
            out << "}\n";
            file.close();
            logBrowser->append("c_cpp_properties.json created successfully.");
        } else {
            logBrowser->append("Failed to create c_cpp_properties.json.");
            // 可以进一步尝试获取具体错误原因，如权限问题、磁盘空间不足等并提示用户
        }
    }

    // 创建tasks.json文件，适配MinGW编译器的编译参数等配置
    void createTasksJson()
    {
        QString compiler = "gcc.exe";
        QStringList args;

        // 询问是否添加常用编译参数，如优化级别、警告显示等，这里简化为一个选项对话框
        QStringList options = {"Yes", "No"};
        bool ok;
        QString addCommonArgs = QInputDialog::getItem(this, "Add Common Arguments", "Do you want to add common compilation arguments? (e.g., -O2 -Wall)", options, 0, false, &ok);
        if (ok && addCommonArgs == "Yes") {
            args << "-O2" << "-Wall";
        }

        // 询问是否添加链接库相关参数，同样简化为选项对话框
        QString addLibArgs = QInputDialog::getItem(this, "Add Library Arguments", "Do you want to add library related arguments? (e.g., -lmy_lib)", options, 0, false, &ok);
        if (ok && addLibArgs == "Yes") {
            QString inputLibArgs = QInputDialog::getText(this, "Library Arguments", "Please enter the library arguments separated by commas (e.g., -lmy_lib,-lother_lib):");
            args << inputLibArgs.split(',', Qt::SkipEmptyParts);
        }

        // 添加生成可执行文件的参数
        args << "${file}" << "-o" << "${fileDirname}/${fileBasenameNoExtension}.exe";

        QFile file("tasks.json");
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << "{\n";
            out << "    \"version\": \"2.0.0\",\n";
            out << "    \"tasks\": [\n";
            out << "        {\n";
            out << QString("            \"label\": \"Build\",\n");
            out << QString("            \"type\": \"shell\",\n");
            out << QString("            \"command\": \"%1\",\n").arg(compiler);
            out << "            \"args\": [\n";
            for (const QString &arg : args) {
                out << QString("                \"%1\",\n").arg(arg.trimmed());
            }
            out << "            ],\n";
            out << "            \"group\": {\n";
            out << "                \"kind\": \"build\",\n";
            out << "                \"isDefault\": true\n";
            out << "            }\n";
            out << "        }\n";
            out << "    ]\n";
            out << "}\n";
            file.close();
            logBrowser->append("tasks.json created successfully.");
        } else {
            logBrowser->append("Failed to create tasks.json.");
        }
    }

    // 创建launch.json文件，适配MinGW编译器对应的调试配置（使用gdb调试器）
    void createLaunchJson()
    {
        QString debuggerType = "cppdbg";
        QString programPath;
        QStringList args;

        // 获取要调试的程序路径
        programPath = QInputDialog::getText(this, "Program Path", "Please enter the path of the program to debug:");
        if (programPath.isEmpty()) {
            logBrowser->append("Program path not provided.");
            return;
        }

        // 询问调试时传入程序的参数
        QString inputArgs = QInputDialog::getText(this, "Debug Arguments", "Please enter the arguments for the program during debugging separated by commas:");
        args = inputArgs.split(',', Qt::SkipEmptyParts);

        QFile file("launch.json");
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << "{\n";
            out << "    \"version\": \"0.2.0\",\n";
            out << "    \"configurations\": [\n";
            out << "        {\n";
            out << QString("            \"name\": \"(Windows) Launch with GDB\",\n");
            out << QString("            \"type\": \"%1\",\n").arg(debuggerType);
            out << "            \"request\": \"launch\",\n";
            out << QString("            \"program\": \"%1\",\n").arg(programPath);
            out << "            \"args\": [\n";
            for (const QString &arg : args) {
                out << QString("                \"%1\",\n").arg(arg.trimmed());
            }
            out << "            ],\n";
            out << "            \"stopAtEntry\": false,\n";
            out << "            \"cwd\": \"${workspaceFolder}\",\n";
            out << "            \"environment\": [],\n";
            out << "            \"externalConsole\": true,\n";
            out << "            \"MIMode\": \"gdb\",\n";  // 明确指定使用gdb调试器
            out << "            \"miDebuggerPath\": \"C:/MinGW/bin/gdb.exe\",  // MinGW的gdb路径，根据实际调整\n";
            out << "            \"setupCommands\": [\n";
            out << "                {\n";
            out << "                    \"description\": \"Enable pretty-printing for gdb\",\n";
            out << "                    \"text\": \"-enable-pretty-printing\",\n";
            out << "                    \"ignoreFailures\": true\n";
            out << "                }\n";
            out << "            ]\n";
            out << "        }\n";
            out << "    ]\n";
            out << "}\n";
            file.close();
            logBrowser->append("launch.json created successfully.");
        } else {
            logBrowser->append("Failed to create launch.json.");
        }
    }

    // 统一调用创建配置文件的函数
    void createConfigFiles()
    {
        createC_CppPropertiesJson();
        createTasksJson();
        createLaunchJson();
    }
};
