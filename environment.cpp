#include <QApplication>
	#include <QWidget>
	#include <QPushButton>
	#include <QLabel>
	#include <QTextBrowser>
	#include <QProcess>
	#include <QDebug>
	#include <QMessageBox>
	#include <QProcessEnvironment>
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
	      if (process.exitCode() != 0) {
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
	      if (process.exitCode() != 0) {
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
