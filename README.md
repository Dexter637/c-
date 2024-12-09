# DeployTool 使用指南

## 概述

DeployTool 是一个基于 Qt 框架的桌面应用程序，旨在帮助用户一键部署 Visual Studio Code（VS Code）及其 C++ 开发环境。本指南将介绍如何安装和运行 DeployTool，以及如何使用它来部署 VS Code 和 C++ 扩展。

## 运行所需环境

- **操作系统**：Windows 7 及以上版本
- **Qt 框架**：Qt 5 或更高版本
- **编译器**：支持 Qt 开发的编译器，如 MinGW 或 MSVC
- **网络**：稳定的网络连接，以便下载 VS Code 安装包和扩展

## 安装 DeployTool

1. **下载 Qt 框架**：确保您的系统上已安装 Qt 框架。如果没有，请从 [Qt 官方网站](https://www.qt.io/download) 下载并安装。

2. **下载 DeployTool 源码**：从项目的代码仓库中获取 DeployTool 的源码。您可以使用 Git 克隆仓库，或者下载压缩包并解压到指定目录。

3. **编译和构建**：使用 Qt Creator 或命令行工具（如 qmake 和 make）编译和构建 DeployTool 项目。
   
4. **直接运行**：或者下载package文件夹到本地，找到DeployTool.exe管理员运行即可

## 使用 DeployTool

1. **启动 DeployTool**：构建成功后，运行 DeployTool 可执行文件。

2. **查看日志**：在 DeployTool 的界面中，您将在命令行里看到程序的运行过程，用于显示部署过程中的日志信息。

3. **点击部署**：DeployTool 将按照以下步骤自动执行：

   - **下载 MINGW 安装包**：使用 PowerShell 从官方网站下载 MINGW 的稳定版安装包。
   - **安装 VS Code：静默安装 VS Code（/S 参数表示静默安装）。
   - **安装 C++ 扩展**：使用 VS Code 的命令行接口安装 C++ 扩展（ms-vscode.cpptools）。
   - **设置环境变量**：将 VS Code 的 bin 目录添加到系统的 PATH 环境变量中。

4. **检查部署**：部署完成后，DeployTool 将检查 VS Code 和 C++ 扩展是否成功安装。如果一切正常，将显示成功消息框；否则，将显示错误日志。

## 注意事项

- 在部署过程中，请确保网络连接稳定，以便顺利下载 VS Code 安装包和扩展。
- 如果在部署过程中遇到任何问题，请查看日志信息以获取更多详细信息，并根据需要进行故障排除。
- DeployTool 可能会根据需要进行更新和改进。请定期检查项目的代码仓库以获取最新版本和更新信息。
- 程序需要管理员权限运行，请授予。
- 请把.pro文件和manifest文件替换到您的本地项目中。

## 联系方式

如果您对 DeployTool 有任何疑问或建议，请通过项目的代码仓库提交问题或拉取请求。我们将尽快回复您的反馈并进行改进。
