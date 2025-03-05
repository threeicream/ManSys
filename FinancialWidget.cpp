#include "FinancialWidget.h"
#include <QChartView>
#include <QTableWidget>
#include <QComboBox>
#include <QPushButton>
#include <QDateEdit>
#include <QLineEdit>
#include <QPieSeries>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDate>
#include <QLabel>
#include <QAbstractItemView>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QMessageBox>
#include <QTimer>

FinancialWidget::FinancialWidget(QWidget *parent)
	: QWidget(parent)
{
	//ui.setupUi(this);
	//pieChartView = new QChartView(this);
	setupUI();
	setupTable();
	populateStudentComboBox();
}

FinancialWidget::~FinancialWidget()
{

}

void FinancialWidget::setupUI()
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	QHBoxLayout* topLayout = new QHBoxLayout();
	QHBoxLayout* middleLayout = new QHBoxLayout();
	chartView = new QChartView();
	mainLayout->addLayout(topLayout);
	mainLayout->addLayout(middleLayout, 60); //占60%高度 
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
	middleLayout->addWidget(tableWidget);

	pieChartView = new QChartView(this);
	middleLayout->addWidget(pieChartView);
	chartView->setRenderHint(QPainter::Antialiasing);//启用 chartView 中的抗锯齿渲染
	chartView->setMinimumHeight(200);//最小高度保障

	//连接
	connect(addButton, &QPushButton::clicked, this, &FinancialWidget::addRecord);
	connect(deleteButton, &QPushButton::clicked, this, &FinancialWidget::delRecord);
	connect(editButton, &QPushButton::clicked, this, &FinancialWidget::editRecord);
	connect(studentComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &FinancialWidget::loadFinancialRecords);
	connect(startDateEdit, &QDateEdit::dateChanged, this, &FinancialWidget::loadFinancialRecords);
	connect(endDateEdit, &QDateEdit::dateChanged, this, &FinancialWidget::loadFinancialRecords);
}

void FinancialWidget::setupTable()
{
	QSqlQuery query;
	query.exec("SELECT * FROM financialRecords");

	QSqlRecord record = query.record(); // 获取 QSqlRecord 对象

	int fieldCount = record.count(); // 获取列数

	for (int i = 0; i < fieldCount; ++i) {
		fieldNames << record.fieldName(i); // 获取第 i 列的名称
	}
}

void FinancialWidget::addRecord()
{
	QDialog dialog(this);
	dialog.setWindowTitle(tr("添加缴费记录"));
	dialog.setFixedSize(195, 220);
	QFormLayout* mainlayout = new QFormLayout(&dialog);
	QComboBox* stuNameBox = new QComboBox(&dialog);
	QSqlQuery namequery("SELECT id,name FROM studentInfo");
	while (namequery.next()) {
		QString id = namequery.value(0).toString();
		QString name = namequery.value(1).toString();
		stuNameBox->addItem(name, QVariant(id));
	}
	QDateEdit* coinTimeEdit = new QDateEdit(QDate::currentDate(), &dialog);
	coinTimeEdit->setCalendarPopup(true);
	QLineEdit* CoinNumEdit = new QLineEdit(&dialog);
	QLineEdit* payMetodEdit = new QLineEdit(&dialog);
	QLineEdit* noteEdit = new QLineEdit(&dialog);
	QPushButton* btnEnter = new QPushButton("确认", &dialog);
	QPushButton* btnCancel = new QPushButton("取消", &dialog);

	mainlayout->addRow("学生名称：", stuNameBox);
	mainlayout->addRow("缴费日期：", coinTimeEdit);
	mainlayout->addRow("金额：", CoinNumEdit);
	mainlayout->addRow("支付类型：", payMetodEdit);
	mainlayout->addRow("备注：", noteEdit);
	mainlayout->addRow(btnEnter, btnCancel);
	/*QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
	buttonBox.button(QDialogButtonBox::Ok)->setText("确定");
	buttonBox.button(QDialogButtonBox::Cancel)->setText("取消");
	mainlayout->addRow(&buttonBox);
	connect(&buttonBox, &QDialogButtonBox::clicked, &dialog, &QDialog::accept);
	connect(&buttonBox, &QDialogButtonBox::clicked, &dialog, &QDialog::reject);*/
	connect(btnEnter, &QPushButton::clicked, &dialog, &QDialog::accept);
	connect(btnCancel, &QPushButton::clicked, &dialog, &QDialog::reject);

	if (dialog.exec() == QDialog::Accepted) {
		QString studentId = stuNameBox->currentData().toString();
		QString payMethodDate = coinTimeEdit->date().toString("yyyy-MM-dd");
		double amount = CoinNumEdit->text().toDouble();
		QString payMethod = payMetodEdit->text();
		QString note = noteEdit->text();

		/*QSqlQuery query;
		query.prepare(
			"INSERT INTO financialRecords"
			"(id, student_id, gender, birthday, join_date, study_goal, progress, photo)"
			"VALUES(:id, :name, :gender, :birthday, :join_date, :study_goal, :progress, :photo)"
		);*/
		QSqlDatabase::database().transaction();//用于开始一个数据库事务。
		QSqlQuery query;
		QString tableheader = "(";
		QString valuesPlaceholder = "(";

		for (int i = 1; i < fieldNames.size(); ++i) {
			tableheader += fieldNames[i];
			valuesPlaceholder += ":" + fieldNames[i];

			if (i < fieldNames.size() - 1) {
				tableheader += ",";
				valuesPlaceholder += ",";
			}
		}

		tableheader += ")";
		valuesPlaceholder += ")";

		QString sql = "INSERT INTO financialRecords " + tableheader + " VALUES " + valuesPlaceholder;
		query.prepare(sql);

		query.bindValue(":" + fieldNames[1], studentId);
		query.bindValue(":" + fieldNames[2], payMethodDate);
		query.bindValue(":" + fieldNames[3], amount);
		query.bindValue(":" + fieldNames[4], payMethod);
		query.bindValue(":" + fieldNames[5], note);

		if (!query.exec()) {
			QSqlDatabase::database().rollback();//回滚
			QMessageBox::critical(this, "错误", "数据插入失败：" + query.lastError().text());
		}
		else {
			QSqlDatabase::database().commit();//提交
			loadFinancialRecords();
			QMessageBox::information(this, "成功", "学生信息已成功保存！");
		}
	}
}

void FinancialWidget::delRecord()
{
	QMessageBox confirmBox(this);
	confirmBox.setWindowTitle("删除");
	confirmBox.setText("确认删除记录？");

	QPushButton* btnyes = confirmBox.addButton("确定", QMessageBox::YesRole);
	QPushButton* btncancel = confirmBox.addButton("取消", QMessageBox::NoRole);

	confirmBox.setDefaultButton(btncancel);

	confirmBox.exec();

	if (confirmBox.clickedButton() != btnyes) {
		confirmBox.close();
		return;
	}

	auto selected = tableWidget->selectionModel()->selectedRows();
	if (selected.isEmpty()) {
		QMessageBox::warning(this, "警告", "没有选择行");
		return;
	}
	QSqlDatabase::database().transaction();
	for (const auto& index : selected) {
		int id = tableWidget->item(index.row(), FinAncialEnum::ID)->text().toInt();
		QSqlQuery query;
		query.prepare("DELETE FROM financialRecords WHERE id=:id");
		query.bindValue(":id", id);
		if (!query.exec()) {
			QSqlDatabase::database().rollback();
			QMessageBox::critical(this, "错误", "删除失败：" + query.lastError().text());
			return;
		}
	}
	QSqlDatabase::database().commit();
	loadFinancialRecords();
}

void FinancialWidget::editRecord()
{
}

void FinancialWidget::loadFinancialRecords()
{
	tableWidget->setRowCount(0);

	QString stuId = studentComboBox->currentData().toString();
	QDate startDate = startDateEdit->date();
	QDate endDate = endDateEdit->date();
	
	QSqlQuery query;
	query.prepare("SELECT fr.id, s.name, fr.payment_date, fr.amount, fr.payment_type, fr.notes "
		"FROM financialRecords fr "
		"JOIN studentInfo s ON fr.student_id = s.id "
		"WHERE fr.payment_date BETWEEN :startDate AND :endDate "
		"AND (:stuId = '-1' OR fr.student_id = :stuId)");
	query.bindValue(":startDate", startDate.toString("yyyy-MM-dd"));
	query.bindValue(":endDate", endDate.toString("yyyy-MM-dd"));
	query.bindValue(":stuId", stuId);

	if(query.exec())
	{
		while (query.next()) {
			int row = tableWidget->rowCount();
			tableWidget->insertRow(row);

			for (int col = 0; col < query.record().count(); ++col) {
				QTableWidgetItem* item = new QTableWidgetItem(query.value(col).toString());
				item->setTextAlignment(Qt::AlignCenter);
				tableWidget->setItem(row, col, item);
			}
		}
	}
	else {
		QMessageBox::critical(this, "警告", QString("财务语句运行错误:%1").arg(query.lastError().text()));
		return;
	}
	tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
	updateChart();
	updatePieChart();
}

void FinancialWidget::updateChart()
{
}

void FinancialWidget::updatePieChart()
{
	QString studentId = studentComboBox->currentData().toString();
	QDate startDate = startDateEdit->date();
	QDate endDate = endDateEdit->date();
	QSqlQuery query;
	query.prepare(
		"SELECT payment_type, SUM(amount) "
		"FROM financialRecords "
		"WHERE payment_date BETWEEN :startDate AND :endDate "
		"AND (:stuId = '-1' OR student_id = :stuId) "
		"GROUP BY payment_type "
	);
	query.bindValue(":startDate", startDate.toString("yyyy-MM-dd"));
	query.bindValue(":endDate", endDate.toString("yyyy-MM-dd"));
	query.bindValue(":stuId", studentId);

	QPieSeries* series = new QPieSeries();
	qreal totalAmount = 0;

	if (!query.exec()) { // 增加错误处理
		QMessageBox::critical(this, "SQL 错误", query.lastError().text());
		return; // 退出函数
	}

	while (query.next()) {
		QString type = query.value(0).toString();
		qreal value = query.value(1).toDouble();
		totalAmount += value;

		if (value > 0) {
			QString legendLabel = QString("%1<br>%2元").arg(type).arg(value);
			QPieSlice* slice = new QPieSlice(legendLabel, value);
			slice->setLabelVisible(false);
			series->append(slice);
		}
	}

	QChart* chart = new QChart();
	chart->addSeries(series);
	chart->setTitle("支付类型分布");

	//图例设置
	chart->legend()->setVisible(true);
	chart->legend()->setAlignment(Qt::AlignBottom);
	chart->legend()->setBackgroundVisible(true);
	chart->legend()->setBrush(Qt::white);
	chart->legend()->setLabelColor(Qt::black);
	chart->legend()->setContentsMargins(10, 10, 10, 10);
	
	if (!pieChartView) {
		qDebug() << "错误：pieChartView 为空指针！";
		return; // 退出函数，避免访问空指针
	}

	if (!chart) {
		qDebug() << "错误：chart 为空指针！";
		return; // 退出函数，避免访问空指针
	}

	//饼图尺寸
	series->setPieSize(0.75);
	// 移除旧的 chart，并释放内存
	if (pieChartView->chart()) {
		QChart* oldChart = pieChartView->chart(); // 移除旧的 QChart 对象
		delete oldChart; // 释放旧的 QChart 对象
	}
	//pieChartView->setChart(chart);
	QTimer::singleShot(0, [this, chart]() {
		pieChartView->setChart(chart);
		});

	//强制重绘
	pieChartView->repaint();
}

void FinancialWidget::populateStudentComboBox()
{
	studentComboBox->clear();
	studentComboBox->addItem("所有学生", QVariant(-1));

	QSqlQuery namequery("SELECT id,name FROM studentInfo");
	while (namequery.next()) {
		QString id = namequery.value(0).toString();
		QString name = namequery.value(1).toString();
		studentComboBox->addItem(name, QVariant(id));
	}
}
