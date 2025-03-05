#pragma once

#include <QWidget>
#include "ui_FinancialWidget.h"
#include <QStringList>
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
	void loadFinancialRecords();
	void updateChart();
	void updatePieChart();
	void populateStudentComboBox();
private:
	Ui::FinancialWidget ui;
private:
	enum FinAncialEnum { ID, STUDENTID, PAYMENTDATE, AMOUNT, PAYMENTTYPE, NOTES };
	QChartView* pieChartView;
	QTableWidget* tableWidget;
	QComboBox* studentComboBox;
	QPushButton* addButton;
	QPushButton* deleteButton;
	QPushButton* editButton;
	QChartView* chartView;
	QDateEdit* startDateEdit;
	QDateEdit* endDateEdit;
	QStringList fieldNames;
};
