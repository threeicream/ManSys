#include "ScheduleWidget.h"
#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QMap>
#include <QVector>
#include <QPair>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QDate>
#include <QCalendarWidget>
#include <QMessageBox>
#include <QOverload>
#include <QSqlQuery>
#include <QFormLayout>
#include "stuInfoWidget.h"
#include <QTimeEdit>
#include <QSqlTableModel>
#include <QSqlError>

ScheduleWidget::ScheduleWidget(QWidget *parent)
	: QWidget(parent)
{
	// 初始化 UI 元素
	tableWidget = new QTableWidget(this);
	yearComboBox = new QComboBox(this);
	weekComboBox = new QComboBox(this);
	dateRangeLabel = new QLabel(this);
	addButton = new QPushButton("增加课程", this);
	deleteButton = new QPushButton("删除课程", this);
	prevWeekBtn = new QPushButton("上一周", this);
	nextWeekBtn = new QPushButton("下一周", this);

	//tableWidget->setAlternatingRowColors(true);
	tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	// 设置 UI 布局
	setupUI();

	/*yearComboBox->setCurrentText(QString::number(QDate::currentDate().year()));
	weekComboBox->setCurrentText(QString("第%1周").arg(customWeekNumber(QDate::currentDate())));*/

	loadSchedule();
	connect(yearComboBox, QOverload<int>::of(&QComboBox:: currentIndexChanged), this, &ScheduleWidget:: loadSchedule);//显示使用int版本的信号
	connect(weekComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ScheduleWidget::loadSchedule);//显示使用int版本的信号
	connect(addButton, &QPushButton:: clicked, this, &ScheduleWidget:: addCourse);
	connect(deleteButton, &QPushButton:: clicked, this, &ScheduleWidget::deleteCourse);
	connect(prevWeekBtn, &QPushButton:: clicked, this, &ScheduleWidget:: showPreviousWeek);
	connect(nextWeekBtn, &QPushButton:: clicked, this, &ScheduleWidget:: showNextWeek);
	connect(tableWidget, &QTableWidget:: itemChanged,this, &ScheduleWidget:: handleItemChanged);
}

ScheduleWidget::~ScheduleWidget()
{
}

void ScheduleWidget::setupUI()
{
	// 顶部布局
	QHBoxLayout* topLayout = new QHBoxLayout;
	topLayout->addWidget(new QLabel("年份:"));
	topLayout->addWidget(yearComboBox);
	topLayout->addWidget(new QLabel("周:"));
	topLayout->addWidget(weekComboBox);
	topLayout->addWidget(dateRangeLabel);
	topLayout->addStretch();

	// 底部布局
	QHBoxLayout* bottomLayout = new QHBoxLayout;
	bottomLayout->addWidget(addButton);
	bottomLayout->addWidget(deleteButton);
	bottomLayout->addStretch();
	bottomLayout->addWidget(prevWeekBtn);
	bottomLayout->addWidget(nextWeekBtn);
	//bottomLayout->addStretch(); // 将按钮推到左侧
	bottomLayout->setAlignment(Qt::AlignCenter);
	bottomLayout->setContentsMargins(0, 2, 0, 2);

	// 主布局
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->addLayout(topLayout);
	mainLayout->addWidget(tableWidget);
	mainLayout->addLayout(bottomLayout);
	
	//this->setLayout(mainLayout);

	setupTable();
}

void ScheduleWidget::setupTable()
{
	// 初始化年份下拉菜单
	auto Year = QDate::currentDate().year();
	yearComboBox->addItem(QString::number(Year - 1), Year - 1);
	yearComboBox->addItem(QString::number(Year), Year);
	yearComboBox->addItem(QString::number(Year + 1), Year + 1);

	// 初始化周下拉菜单
	for (int i = 1; i <= 52; ++i) {
		weekComboBox->addItem(QString("第%1周").arg(i),i);
	}
	
	yearComboBox->setCurrentText(QString::number(QDate::currentDate().year()));
	weekComboBox->setCurrentText(QString("第%1周").arg(customWeekNumber(QDate::currentDate())));

	// 设置表格的基本属性
	QStringList headerLabels = { "星期一", "星期二", "星期三", "星期四", "星期五", "星期六", "星期日" };
	QStringList rowLabels = { "上午1", "上午2", "下午1", "下午2", "晚上1", "晚上2" };
	times = rowLabels;
	tableWidget->setRowCount(rowLabels.size());   // 星期一到星期日
	tableWidget->setColumnCount(headerLabels.size()); // 时间段数量
	
	int year = yearComboBox->currentData().toInt();
	int week = weekComboBox->currentData().toInt();
	QPair<QDate, QDate>weekRange = getWeekRange(year, week);
	QDate startDate = weekRange.first;
	QStringList ColLabels;
	for (int i = 0; i < 7; ++i) {
		QDate currentDate = startDate.addDays(i);
		ColLabels.append(QString("%1\n%2").arg(headerLabels[i]).arg(currentDate.toString("MM/dd")));
	}
	tableWidget->setHorizontalHeaderLabels(ColLabels);
	tableWidget->setVerticalHeaderLabels(rowLabels);
	tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//设置为伸展模式
	tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);//双击/f2可以进行编辑

	// 设置日期范围标签的文本
	dateRangeLabel->setText(QString("%1到%2").arg(weekRange.first.toString("yyyy/MM/dd")).arg(weekRange.second.toString("yyyy/MM/dd")));
}

int ScheduleWidget::customWeekNumber(const QDate& date)
{
	QDate startYear(date.year(), 1, 1);
	int dayWeek = startYear.dayOfWeek();
	int days = startYear.daysTo(date);
	int week = (days + dayWeek - 1) / 7 + 1;
	return week;
}

QPair<QDate, QDate> ScheduleWidget::getWeekRange(int year, int week) {
	QDate startDate(year, 1, 1);
	int daysToSubtract = startDate.dayOfWeek() - Qt::Monday;
	if (daysToSubtract > 0) startDate = startDate.addDays(-daysToSubtract);
	QDate weekStart = startDate.addDays((week - 1) * 7);
	QDate weekEnd = weekStart.addDays(6);
	return qMakePair(weekStart, weekEnd);
}

void ScheduleWidget::loadSchedule()
{
	tableWidget->blockSignals(true);
	tableWidget->clearContents();

	int year = yearComboBox->currentData().toInt();
	int week = weekComboBox->currentData().toInt();
	QPair<QDate, QDate>weekRange = getWeekRange(year, week);
	QDate startDate = weekRange.first;
	QDate endDate = weekRange.second;
	dateRangeLabel->setText(QString("%1到%2").arg(startDate.toString("yyyy-MM-dd")).arg(endDate.toString("yyyy-MM-dd")));

	QVector<QVector<QString>> courses(times.count(), QVector<QString>(7, ""));

	QSqlQuery query;
	query.prepare("SELECT date, time, course_name FROM schedule WHERE date BETWEEN :startDate AND :endDate");
	query.bindValue(":startDate", startDate.toString("yyyy-MM-dd"));
	query.bindValue(":endDate", endDate.toString("yyyy-MM-dd"));
	qDebug() << startDate.toString("yyyy-MM-dd");
	qDebug() << endDate.toString("yyyy-MM-dd");
	if (query.exec()) {//执行SQL语句
		while (query.next()) {
			QDate date = QDate::fromString(query.value(SChedule::DATE).toString(), "yyyy-MM-dd");
			QString time = query.value(SChedule::TIME).toString();
			int dayIndex = startDate.daysTo(date);
			int timeIndex = times.indexOf(time);
			if (dayIndex >= 0 && dayIndex < 7 && timeIndex != -1) {
				courses[timeIndex][dayIndex] = query.value(SChedule::COURSENAME).toString();
			}
		}
	}
	for (int day = 0; day < 7; ++day) {
		for (int time = 0; time < times.count(); ++time) {
			QTableWidgetItem* item = new QTableWidgetItem(courses[time][day]);
			item->setTextAlignment(Qt::AlignCenter); tableWidget->setItem(time, day, item);
		}
	}
	tableWidget->blockSignals(false);
}

void ScheduleWidget::addCourse()
{
	int dayindex = tableWidget->currentColumn();
	int timeindex = tableWidget->currentRow();
	if (dayindex == -1 || timeindex == -1) {
		QMessageBox::warning(this, "错误", "请正确选择时间！");
		return;
	}

	if (!tableWidget->item(timeindex, dayindex)->text().isEmpty()) {
		QMessageBox::warning(this, "错误", "已占用！");
		return;
	}

	QDialog dialog(this);
	dialog.setWindowTitle("添加课程");
	dialog.setMinimumSize(172,134);
	QFormLayout layout(&dialog);
	QComboBox namecombo;
	QComboBox object;
	object.addItems(QStringList{ "语文","数学","英语","物理","化学" });
	QSqlQuery namequery("SELECT name FROM studentInfo");
	while (namequery.next()) {
		namecombo.addItem(namequery.value(0).toString());
	}
	QMap<int, QTime>timePresets = {
		{0,QTime(9,0)},{1,QTime(11,0)},{2,QTime(14,0)},
		{3,QTime(16,0)},{4,QTime(19,0)},{5,QTime(21,0)}
	};
	QTimeEdit timeEdit;//时间选择控件
	timeEdit.setDisplayFormat("HH:mm");
	timeEdit.setTime(timePresets[timeindex]);
	timeEdit.setTimeRange(timePresets[timeindex], timePresets[timeindex].addSecs(3600));

	layout.addRow("学生姓名：", &namecombo);
	layout.addRow("学科：", &object);
	layout.addRow("课程时间：", &timeEdit);
	QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	buttons.button(QDialogButtonBox::Ok)->setText("确认");
	buttons.button(QDialogButtonBox::Cancel)->setText("取消");
	layout.addRow(&buttons);
	connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
	connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
	if (dialog.exec() != QDialog::Accepted)return;
	//生成组合字符串
	QString coursename = QString("%1：%2 %3").arg(namecombo.currentText()).arg(object.currentText()).arg(timeEdit.time().toString("HH:mm"));

	//获取日期信息
	int year = yearComboBox->currentData().toInt();
	int week = weekComboBox->currentData().toInt();
	QPair<QDate, QDate>weekRange = getWeekRange(year, week);
	QDate currentDate = weekRange.first.addDays(dayindex);
	QString timeslot = times[timeindex];

	//数据库保存
	/*QSqlTableModel model;
	model.setTable("schedule");
	qDebug() << model.headerData(0, Qt::Horizontal).toString();*/
	QSqlDatabase::database().transaction();
	QSqlQuery addtquery;
	addtquery.prepare(
		"INSERT INTO schedule"
		"(date,time,course_name)"
		"VALUES(:date, :time, :course_name)"
	);
	addtquery.bindValue(":date", currentDate.toString("yyyy-MM-dd"));
	addtquery.bindValue(":time", timeslot);
	addtquery.bindValue(":course_name", coursename);

	if (!addtquery.exec()) {
		QSqlDatabase::database().rollback();//回滚
		QMessageBox::critical(this, "错误", "数据插入失败：" + addtquery.lastError().text());
	}
	else {
		QSqlDatabase::database().commit();//提交
		loadSchedule();
		QMessageBox::information(this, "成功", "课程已成功保存！");
	}
}

void ScheduleWidget::deleteCourse()
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
	
	int dayindex = tableWidget->currentColumn();
	int timeindex = tableWidget->currentRow();
	if (dayindex == -1 || timeindex == -1) {
		QMessageBox::warning(this, "错误", "请正确选择时间！");
		return;
	}

	QTableWidgetItem* item = tableWidget->item(timeindex, dayindex);
	if (item->text().isEmpty()) {
		QMessageBox::warning(this, "错误", "为空！");
		return;
	}

	//获取日期信息
	int year = yearComboBox->currentData().toInt();
	int week = weekComboBox->currentData().toInt();
	QPair<QDate, QDate>weekRange = getWeekRange(year, week);
	QDate date = weekRange.first.addDays(dayindex);
	QString time = times[timeindex];

	QSqlDatabase::database().transaction();
	QSqlQuery query;
	query.prepare("DELETE FROM schedule WHERE date =:date AND time=:time");
	query.bindValue(":date", date.toString("yyyy-MM-dd"));
	query.bindValue(":time", time);

	if (!query.exec()) {
		QSqlDatabase::database().rollback();//回滚
		QMessageBox::critical(this, "错误", "数据删除失败：" + query.lastError().text());
	}
	else {
		QSqlDatabase::database().commit();//提交
		QMessageBox::information(this, "成功", "删除成功！");
		loadSchedule();
	}
}

void ScheduleWidget::showPreviousWeek()
{
	int currentWeek = weekComboBox->currentIndex();
	int currentYear = yearComboBox->currentIndex();
	if (currentWeek > 0) {
		weekComboBox->setCurrentIndex(currentWeek - 1);
	}
	else {
		if (yearComboBox->currentIndex() > 0) {
			yearComboBox->setCurrentIndex(currentYear - 1);
			//跳转到上一年最后一周（第52周） 
			weekComboBox->setCurrentIndex(51);
		}
	}
}

void ScheduleWidget::showNextWeek()
{
	int currentWeek = weekComboBox->currentIndex();
	int currentYear = yearComboBox-> currentIndex(); 
	if (currentWeek < 51){
		weekComboBox-> setCurrentIndex(currentWeek + 1);
	}
	else {
		if (yearComboBox->currentIndex() > yearComboBox->count() - 1) {
			yearComboBox->setCurrentIndex(currentYear + 1);
			weekComboBox->setCurrentIndex(0);
		}
	}
}

void ScheduleWidget::handleItemChanged(QTableWidgetItem* item)
{
	const int timeslot = item->row();
	const int day = item->column();
	QString newcourse = item->text().trimmed();

	//获取日期信息
	int year = yearComboBox->currentData().toInt();
	int week = weekComboBox->currentData().toInt();
	QPair<QDate, QDate>weekRange = getWeekRange(year, week);
	QDate date = weekRange.first.addDays(day);
	QString time = times[timeslot];

	QSqlDatabase::database().transaction();
	QSqlQuery query;
	if(newcourse.isEmpty()){//删除
		query.prepare("DELETE FROM schedule WHERE date =:date AND time=:time");
		query.bindValue(":date", date.toString("yyyy-MM-dd"));
		query.bindValue(":time", time);
	}
	else {
		query.prepare(
			"INSERT OR REPLACE INTO schedule"
			"(date,time,course_name)"
			"VALUES (:date,:time,:course_name)"
		);
		query.bindValue(":date", date.toString("yyyy-MM-dd"));
		query.bindValue(":time", time);
		query.bindValue(":course_name", newcourse);
		qDebug() << date.toString("yyyy-MM-dd") << time << newcourse;
	}

	if (!query.exec()) {
		QSqlDatabase::database().rollback();//回滚
		QMessageBox::critical(this, "错误", "数据修改失败：" + query.lastError().text());
		loadSchedule();
	}
	else {
		QSqlDatabase::database().commit();//提交
		QMessageBox::information(this, "成功", "修改成功！");
	}
}

