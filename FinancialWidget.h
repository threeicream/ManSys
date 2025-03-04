#pragma once

#include <QWidget>
#include "ui_FinancialWidget.h"
class QChartView;
class QTableWidget;
class QComboBox;
class QPushButton;
class QDateEdit;

class FinancialWidget : public QWidget
{
	Q_OBJECT

public:
	FinancialWidget(QWidget *parent = nullptr);
	~FinancialWidget();
private:
	void setupUI(); 
	void setupTable();
	void addRecord();
	void delRecord();
	void editRecord();
private:
	Ui::FinancialWidget ui;
private:
	QChartView* pieChartView;
	QTableWidget* tableWidget;
	QComboBox* studentComboBox;
	QPushButton* addButton;
	QPushButton* deleteButton;
	QPushButton* editButton;
	QChartView* chartView;
	QDateEdit* startDateEdit;
	QDateEdit* endDateEdit;
};
