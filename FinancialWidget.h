#pragma once

#include <QWidget>
//#include "ui_FinancialWidget.h"
#include <QStringList>
//#include <QChart>
class QChartView;
class QTableWidget;
class QComboBox;
class QPushButton;
class QDateEdit;
class QPieSlice;
class QTableWidgetItem;

class FinancialWidget : public QWidget
{
	Q_OBJECT

public:
	FinancialWidget(QWidget *parent = nullptr);
	~FinancialWidget();
private:
	void setupUI(); 
	void setupTable();
//private:
//	Ui::FinancialWidget ui;
private slots:
	void do_PieHovered(QPieSlice* slice, bool state);
	void addRecord();
	void delRecord();
	void editRecord();
	void loadFinancialRecords();
	void updateChart();
	void updatePieChart();
	void populateStudentComboBox();
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
	//std::shared_ptr<QChart> chart;
};
