#include "MainWindow.h"
#include <QButtonGroup>
#include <QSqlRecord>

MainWindow::MainWindow(QMainWindow *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	QButtonGroup* btnGp = new QButtonGroup(this);

	btnGp->addButton(ui.btnStudentInfo, BtnId::StudentInfo);
	btnGp->addButton(ui.btnSchedule, BtnId::Schedule);
	btnGp->addButton(ui.btnFinance, BtnId::Finance);
	btnGp->addButton(ui.btnHonor, BtnId::Honor);
	btnGp->addButton(ui.btnSetting, BtnId::Setting);

	btnGp->button(BtnId::StudentInfo)->setChecked(true);
	ui.stackedWidget->setCurrentIndex(BtnId::StudentInfo);

	connect(btnGp, &QButtonGroup::idClicked, ui.stackedWidget, &QStackedWidget::setCurrentIndex);
}

MainWindow::~MainWindow()
{

}

