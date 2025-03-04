#pragma once

#include <QWidget>
#include <QMap>
#include <QVector>
#include <QStringList>
#include <QDate>
class QTableWidget;
class QComboBox;
class QLabel;
class QPushButton;
class QTableWidgetItem;

class ScheduleWidget : public QWidget
{
	Q_OBJECT

public:
	ScheduleWidget(QWidget* parent = nullptr);
	~ScheduleWidget();
private:
	void setupUI();
	void setupTable();
	int customWeekNumber(const QDate& date);
	QPair<QDate, QDate>getWeekRange(int year, int week);
private:
	enum SChedule{DATE,TIME,COURSENAME};
	QTableWidget* tableWidget; // 用于放置课程
	QComboBox* yearComboBox; // 年的下拉菜单
	QComboBox* weekComboBox; // 周的下拉菜单
	QLabel* dateRangeLabel; // 显示日期范围的标签
	QPushButton* addButton; // 增加课程
	QPushButton* deleteButton; // 删除课程
	QPushButton* prevWeekBtn; // 跳转到上一周
	QPushButton* nextWeekBtn; // 跳转到下一周
	QMap<QPair<int, int>, QVector<QVector<QString>>> scheduleData; // 课程数据存储结构： 键为 (year, week)， 值为课程表数据
	QStringList weeks;
	QStringList times; // 上午1， 上午2...
	QStringList SchSqlName;
private slots:
	void loadSchedule();
	void addCourse();
	void deleteCourse();
	void showPreviousWeek();
	void showNextWeek();
	void handleItemChanged(QTableWidgetItem* item);
};
