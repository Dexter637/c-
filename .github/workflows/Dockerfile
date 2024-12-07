# 使用 Microsoft 的 Visual Studio Docker 镜像
FROM mcr.microsoft.com/dotnet/framework/sdk:4.8

# 安装必要的工具
RUN powershell -Command \
    Set-ExecutionPolicy Unrestricted -Scope Process -Force; \
    Invoke-WebRequest -Uri https://aka.ms/vs/17/release/vs_installer.exe -OutFile vs_installer.exe; \
    Start-Process -FilePath vs_installer.exe -ArgumentList '--add', 'Microsoft.VisualStudio.Workload.NativeDesktop', '--quiet', '--norestart' -Wait; \
    Remove-Item -Force vs_installer.exe

# 安装 Qt 5.15.2
RUN Invoke-WebRequest -Uri https://download.qt.io/archive/qt/5.15/5.15.2/qt-opensource-windows-x86-5.15.2.exe -OutFile qt_installer.exe; \
    Start-Process -FilePath qt_installer.exe -ArgumentList "/S", "/D=C:\Qt\5.15.2" -Wait; \
    Remove-Item -Force qt_installer.exe

# 设置 Qt 和 Visual Studio 环境变量
ENV PATH="C:\\Qt\\5.15.2\\msvc2019_64\\bin;C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Tools\\MSVC\\14.28.29333\\bin\\HostX86\\x64;C:\\Program Files\\Microsoft Visual Studio\\2019\\Community\\Common7\\IDE\\CommonExtensions\\Microsoft\\CMake\\CMake\\bin:$PATH"

# 设置默认工作目录
WORKDIR /app

# 设置 ENTRYPOINT 或 CMD 来执行构建命令
CMD ["powershell", "Build.ps1"]
