#include <QApplication>
#include "SetupManager.h"
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    // 创建 SetupManager 实例并启动设置过程
    SetupManager setupManager;
    setupManager.startSetup();
    return app.exec();
}
