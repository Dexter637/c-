    #ifndef SETUPMANAGER_H
#define SETUPMANAGER_H
#include <QObject>
#include <QNetworkAccessManager>
#include <QProcess>
#include <QNetworkReply>
class SetupManager : public QObject
{
    Q_OBJECT
public:
    explicit SetupManager(QObject *parent = nullptr);
    void startSetup();
private slots:
    void onDownloadFinished(QNetworkReply *reply);
    void onVSCodeDownloadFinished(QNetworkReply *reply); // 添加这行声明
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
private:
    QNetworkAccessManager manager;
    QProcess process;
    bool requestAdminRights();
    void downloadMinGW();
    void extractAndConfigureMinGW();
    void downloadVSCode();
    void installVSCode();
    void configureVSCode();
};
#endif // SETUPMANAGER_H
