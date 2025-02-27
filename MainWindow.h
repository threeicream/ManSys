#pragma once

#include <QMainWindow>
#include "ui_MainWindow.h"
#include <QTableWidget>
#include <QSqlQuery>

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
	
private:
	Ui::MainWindow ui;
private:
	
};
