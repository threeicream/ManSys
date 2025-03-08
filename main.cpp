#include <QApplication>
//#include <QWidget>
#include "MainWindow.h"
#include <QFile>
#include "DataBaseManSys.h"
#include "LoginDialog.h"


int main(int argc, char* argv[]) {
	//QApplication::setAttribute(Qt::AA_DontUseNativeDialogs);//强制使用 Qt 内置文件对话框
	QApplication a(argc, argv);

	DataBaseManSys::Instance();

	QFile stylefile(":/MainWindow/stylesheet/style1.qss");
	if (stylefile.open(QFile::ReadOnly)) {
		QString stylesheet = QString(stylefile.readAll());
		a.setStyleSheet(stylesheet);//应用样式表
		stylefile.close();
	}
	else
		qWarning() << "样式表打开失败" << stylefile.errorString();

	LoginDialog loginDlg;
	while (loginDlg.exec() == QDialog::Accepted) {
		MainWindow w;
		w.show();
		return /*QApplication::*/a.exec();
	}
}