#include "FinancialWidget.h"
#include <QChartView>
#include <QTableWidget>
#include <QComboBox>
#include <QPushButton>
#include <QDateEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDate>
#include <QLabel>
#include <QAbstractItemView>

FinancialWidget::FinancialWidget(QWidget *parent)
	: QWidget(parent)
{
	//ui.setupUi(this);
	setupUI();
}

FinancialWidget::~FinancialWidget()
{
}

void FinancialWidget::setupUI()
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	QHBoxLayout* topLayout = new QHBoxLayout();
	QHBoxLayout* middleLayyout = new QHBoxLayout();
	chartView = new QChartView();
	mainLayout->addLayout(topLayout);
	mainLayout->addLayout(middleLayyout, 60); //占60%高度 
	mainLayout->addWidget(chartView, 40); //占40%高度顶部筛选条件与按钮布局  

	topLayout->addWidget(new QLabel ("学生姓名:", this));
	studentComboBox = new QComboBox(this);
	topLayout->addWidget(studentComboBox);

	topLayout->addWidget(new QLabel ("起始 日期:", this));
	startDateEdit = new QDateEdit(QDate::currentDate().addMonths(-1));
	startDateEdit->setCalendarPopup(true);
	topLayout->addWidget(startDateEdit);

	topLayout->addWidget(new QLabel("结束日期:", this));
	endDateEdit = new QDateEdit(QDate::currentDate());
	endDateEdit->setCalendarPopup(true);
	topLayout->addWidget(endDateEdit);

	addButton = new QPushButton("增加");
	deleteButton = new QPushButton("删除");
	editButton = new QPushButton("编辑");
	topLayout->addWidget(addButton);
	topLayout->addWidget(deleteButton);
	topLayout->addWidget(editButton);
	topLayout->addStretch();

	tableWidget = new QTableWidget();
	tableWidget->setFixedWidth(550);
	tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//编辑模式
	//tableWidget->setAlternatingRowColors(true);//隔行颜色
	QStringList header{ "ID","学生姓名","缴费日期","金额","支付类型","备注" };
	tableWidget->setColumnCount(header.count());//设置列数量
	tableWidget->setHorizontalHeaderLabels(header);
	tableWidget->setColumnHidden(0, true);//隐藏 tableWidget 中的第 0 列 (第一列)。
	middleLayyout->addWidget(tableWidget);

	pieChartView = new QChartView();
	middleLayyout->addWidget(pieChartView);
	chartView->setRenderHint(QPainter::Antialiasing);//启用 chartView 中的抗锯齿渲染
	chartView->setMinimumHeight(200);//最小高度保障

	//连接
	connect(addButton, &QPushButton::clicked, this, &FinancialWidget::addRecord);
	connect(deleteButton, &QPushButton::clicked, this, &FinancialWidget::delRecord);
	connect(editButton, &QPushButton::clicked, this, &FinancialWidget::editRecord);
	setupTable();
}

void FinancialWidget::setupTable()
{
	//tableWidget
}

void FinancialWidget::addRecord()
{
}

void FinancialWidget::delRecord()
{
}

void FinancialWidget::editRecord()
{
}
