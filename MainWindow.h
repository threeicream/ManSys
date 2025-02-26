#pragma once

#include <QMainWindow>
#include "ui_MainWindow.h"
class QSqlTableModel;
class QItemSelectionModel;

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	enum BtnId {
		StudentInfo,
		Schedule,
		Finance,
		Honor,
		Setting
	};
public:
	MainWindow(QMainWindow *parent = nullptr);
	~MainWindow();

private:
	Ui::MainWindow ui;
private:
	QSqlTableModel* tabmodel = nullptr;
	QItemSelectionModel* selmodel = nullptr;
};
