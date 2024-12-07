# 使用 Windows Server Core 作为基础镜像
FROM mcr.microsoft.com/windows/servercore:ltsc2022

# 安装 Chocolatey（Windows 包管理器）
RUN powershell -Command Set-ExecutionPolicy Bypass -Scope Process -Force; \
    iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))

# 安装 Visual Studio 和 C++ 编译工具
RUN choco install visualstudio2019community --package-parameters "--add Microsoft.VisualStudio.Workload.NativeDesktop --includeRecommended --quiet"

# 安装 Qt
RUN choco install qt5 --version=5.15.2 --package-parameters "--install-dir=C:\Qt"

# 安装其他可能需要的工具，如 Git、CMake、nmake 等
RUN choco install git cmake

# 设置环境变量
ENV PATH="C:\Qt\5.15.2\msvc2019_64\bin;C:\Program Files\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin;C:\tools\git\bin;$PATH"

# 启动时默认执行的命令
CMD ["cmd"]
