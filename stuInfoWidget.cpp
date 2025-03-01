#include "stuInfoWidget.h"
#include <QSqlQuery>
#include <QDialog>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QFormLayout>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QSqlError>
#include <exception>
#include "Delegate.h"

stuInfoWidget::stuInfoWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.tableWidget->verticalHeader()->setDefaultSectionSize(100);
	refreshTable();

	//性别列处理
	ComboBoxDelegate* genderDelegate = new ComboBoxDelegate(QStringList({ "男","女" }), this);
	ui.tableWidget->setItemDelegateForColumn(stuInfoHeader::GENDER, genderDelegate);

	//学习目标列处理
	ComboBoxDelegate* progressDelegate = new ComboBoxDelegate(QStringList({ "0%","20%","30%", "40%", "50%", "60%", "70%", "80%", "90%", "100%" }), this);
	ui.tableWidget->setItemDelegateForColumn(stuInfoHeader::PROGRESS, progressDelegate);

	//日期列处理
	DateEditDelegate* joinDate = new DateEditDelegate(this);
	ui.tableWidget->setItemDelegateForColumn(stuInfoHeader::JOIN_DATE, joinDate);

	DateEditDelegate* birthday = new DateEditDelegate(this);
	ui.tableWidget->setItemDelegateForColumn(stuInfoHeader::BIRTHDAY, birthday);

	//图片列处理
	ImgDelegate* imagData = new ImgDelegate(this);
	ui.tableWidget->setItemDelegateForColumn(stuInfoHeader::PHOTO, imagData);

	connect(ui.tableWidget, &QTableWidget::itemChanged, this, &stuInfoWidget::handleitemChanged);
}

stuInfoWidget::~stuInfoWidget()
{
}

void stuInfoWidget::refreshTable()
{
	ui.tableWidget->blockSignals(true);//避免发出itemChanged信号
	ui.tableWidget->setRowCount(0);
	
	QSqlQuery query("SELECT *FROM studentInfo");
	while (query.next()) {
		int row = ui.tableWidget->rowCount();
		ui.tableWidget->insertRow(row);

		for (int col = 0; col < ui.tableWidget->columnCount(); ++col) {
			QTableWidgetItem* item = new QTableWidgetItem();
			item->setTextAlignment(Qt::AlignCenter);

			if (col == ui.tableWidget->columnCount() - 1) {//单独处理照片列
				QByteArray photoData = query.value(col).toByteArray();
				if (!photoData.isEmpty()) {
					QPixmap photo;
					photo.loadFromData(photoData);
					item->setData(Qt::DecorationRole, photo.scaled(100, 100, Qt::KeepAspectRatio));//Qt::DecorationRole 用于指定要显示的装饰
					item->setData(Qt::UserRole, photoData);
				}
			}
			else {//处理其他列
				item->setText(query.value(col).toString());
				//item->setTextAlignment(Qt::AlignCenter);
			}
			ui.tableWidget->setItem(row, col, item);
		}
	}
	ui.tableWidget->blockSignals(false);
}

QGroupBox* stuInfoWidget::createFormGroup()
{
	QGroupBox* formGroup = new QGroupBox("基本信息");
	QFormLayout* formLayout = new QFormLayout(formGroup);//表布局
	
	QLabel* idLabel = new QLabel("编号:");
	QLineEdit* idLineEdit = new QLineEdit();
	idLineEdit->setObjectName("idLineEdit");//设置对象名称，从而可以在其他函数中调用

	QLabel* nameLabel = new QLabel("姓名:");
	QLineEdit* nameLineEdit = new QLineEdit();
	nameLineEdit->setObjectName("nameLineEdit");

	QLabel* genderLabel = new QLabel("性别:");
	QComboBox* genderComboBox = new QComboBox;
	genderComboBox->setObjectName("genderComboBox");
	genderComboBox->addItem("男");
	genderComboBox->addItem("女");

	QLabel* birthDateLabel = new QLabel("出生日期:");
	QDateEdit* birthDateEdit = new QDateEdit(QDate::currentDate());
	birthDateEdit->setObjectName("birthDateEdit");
	birthDateEdit->setDisplayFormat("yyyy-MM-dd"); // 设置日期显示格式
	birthDateEdit->setCalendarPopup(true);      // 允许弹出日历选择

	QLabel* enrollmentDateLabel = new QLabel("入学日期:");
	QDateEdit* enrollmentDateEdit = new QDateEdit(QDate::currentDate());
	enrollmentDateEdit->setObjectName("enrollmentDateEdit");
	enrollmentDateEdit->setDisplayFormat("yyyy-MM-dd");
	enrollmentDateEdit->setCalendarPopup(true);

	QLabel* learningGoalLabel = new QLabel("学习目标:");
	QLineEdit* learningGoalLineEdit = new QLineEdit;
	learningGoalLineEdit->setObjectName("learningGoalLineEdit");

	QLabel* currentProgressLabel = new QLabel("当前进度:");
	QComboBox* currentProgressComboBox = new QComboBox;
	currentProgressComboBox->setObjectName("currentProgressComboBox");
	currentProgressComboBox->addItems({ "0%","20%","30%", "40%", "50%", "60%", "70%", "80%", "90%", "100%"});

	// 基本信息布局
	formLayout->addRow(idLabel, idLineEdit);
	formLayout->addRow(nameLabel, nameLineEdit);
	formLayout->addRow(genderLabel, genderComboBox);
	formLayout->addRow(birthDateLabel, birthDateEdit);
	formLayout->addRow(enrollmentDateLabel, enrollmentDateEdit);
	formLayout->addRow(learningGoalLabel, learningGoalLineEdit);
	formLayout->addRow(currentProgressLabel, currentProgressComboBox);

	return formGroup;
}

QGroupBox* stuInfoWidget::createPhotoGroup()
{
	QGroupBox* photoGroup = new QGroupBox("照片上传");
	QVBoxLayout* photoLayout = new QVBoxLayout(photoGroup);
	//初始化控件
	QLabel* photoLabel = new QLabel();
	photoLabel->setFixedSize(200, 200); // 设置照片显示区域大小
	photoLabel->setScaledContents(true);   // 让照片自适应QLabel大小
	photoLabel->setStyleSheet("border: 1px solid black;"); // 添加边框

	QPushButton* uploadButton = new QPushButton("上传照片");
	connect(uploadButton, &QPushButton::clicked, this, [this, photoLabel]() {
		qDebug() << "调用 QFileDialog::getOpenFileName() 前";
		QString fileName = QFileDialog::getOpenFileName(this, "打开照片", QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) , "Images (*.png *.jpg *.ico *.jpeg);;all files (*.*)");
		qDebug() << "调用 QFileDialog::getOpenFileName() 后，文件名：" << fileName;
		if (!fileName.isEmpty()) {
			QPixmap pixmap(fileName);
			if (!pixmap.isNull()) {
				pixmap = pixmap.scaled(photoLabel->width() - 30, photoLabel->height() - 30, Qt::KeepAspectRatio);//智能缩放
				QPixmap copiedPixmap = pixmap.copy(); // 创建深拷贝
				photoLabel->setPixmap(copiedPixmap);
				QBuffer buffer(&photoData);
				buffer.open(QIODevice::WriteOnly);
				pixmap.save(&buffer, "PNG");//保存到buffer内存中，然后buffer传递给photoData
			}
			else
				QMessageBox::warning(this, "警告", "图片打开错误！");
		}
	});

	//添加控件
	photoLayout->addWidget(photoLabel, 0, Qt::AlignCenter);//0表示不进行拉伸
	photoLayout->addWidget(uploadButton);

	return photoGroup;
}

void stuInfoWidget::handleDialogAccepted(QGroupBox* formGroup, QGroupBox* photoGroup)
{
	// 1. 获取表单数据
	QLineEdit* idLineEdit = formGroup->findChild<QLineEdit*>("idLineEdit");
	QLineEdit* nameLineEdit = formGroup->findChild<QLineEdit*>("nameLineEdit");
	QComboBox* genderComboBox = formGroup->findChild<QComboBox*>("genderComboBox");
	QDateEdit* birthDateEdit = formGroup->findChild<QDateEdit*>("birthDateEdit");
	QDateEdit* enrollmentDateEdit = formGroup->findChild<QDateEdit*>("enrollmentDateEdit");
	QLineEdit* learningGoalLineEdit = formGroup->findChild<QLineEdit*>("learningGoalLineEdit");
	QComboBox* currentProgressComboBox = formGroup->findChild<QComboBox*>("currentProgressComboBox");

	// 2. 判空校验，学号和姓名不能为空
	if (idLineEdit->text().isEmpty() || nameLineEdit->text().isEmpty())
	{
		QMessageBox::warning(this, "警告", "学号和姓名不能为空！");
		return; // 停止执行后续操作
	}
	qDebug() << idLineEdit->text();
	qDebug() << nameLineEdit->text();
	//// 3. 数据类型校验（例如：当前进度必须是数字）
	//bool ok;
	//int learningGoal = learningGoalLineEdit->text().toInt(&ok);
	//if (!ok)
	//{
	//	QMessageBox::warning(this, "警告", "当前进度必须是数字！");
	//	return; // 停止执行后续操作
	//}

	// 4. 获取表单的各个值
	QString id = idLineEdit->text();
	QString name = nameLineEdit->text();
	QString gender = genderComboBox->currentText();
	QDate birthDate = birthDateEdit->date();
	QDate enrollmentDate = enrollmentDateEdit->date();
	QString learningGoal = learningGoalLineEdit->text();
	QString currentProgress = currentProgressComboBox->currentText();

	qDebug() << id;
	qDebug() << name;
	qDebug() << gender;
	qDebug() << birthDate;
	qDebug() << enrollmentDate;
	qDebug() << learningGoal;
	qDebug() << currentProgress;

	// 5. 检查学号唯一性
	QSqlQuery query;
	query.prepare("SELECT id FROM studentInfo WHERE id = ?");
	query.addBindValue(id);
	if (query.exec()&&query.next())//查询必须成功执行 ( query.exec() 返回 true )结果集中至少要有一行数据 ( query.next() 返回 true )
	{
		QMessageBox::critical(this, "错误", tr("学号%1已存在！").arg(id));
		return; // 停止执行后续操作
	}

	//6.插入数据
	QSqlDatabase::database().transaction();//用于开始一个数据库事务。
	//调用 QSqlDatabase::database().transaction(); 会告诉数据库： "从现在开始，直到我显式地提交 ( commit() ) 或回滚 ( rollback() )，所有数据库操作都属于同一个事务。"
	QSqlQuery insertquery;
	insertquery.prepare(
		"INSERT INTO studentInfo"
		"(id, name, gender, birthday, join_date, study_goal, progress, photo)"
		"VALUES(:id, :name, :gender, :birthday, :join_date, :study_goal, :progress, :photo)"
	);
	//绑定参数
	insertquery.bindValue(":id", id);
	insertquery.bindValue(":name", name);
	insertquery.bindValue(":gender", gender);
	insertquery.bindValue(":birthday", birthDate);
	insertquery.bindValue(":join_date", enrollmentDate);
	insertquery.bindValue(":study_goal", learningGoal);
	insertquery.bindValue(":progress", currentProgress);
	insertquery.bindValue(":photo", photoData); //保存照片数据
	
	if (!insertquery.exec()) {
		QSqlDatabase::database().rollback();//回滚
		QMessageBox::critical(this, "错误", "数据插入失败：" + insertquery.lastError().text());
	}
	else {
		QSqlDatabase::database().commit();//提交
		refreshTable();
		QMessageBox::information(this, "成功", "学生信息已成功保存！");
	}
}

void stuInfoWidget::on_btnAdd_clicked()
{
	QDialog dlg(this);
	dlg.setWindowTitle(tr("添加学生信息"));
	dlg.setMinimumSize(600, 400);
	//初始化对话框布局
	QVBoxLayout* mainlayout = new QVBoxLayout(&dlg);//垂直
	QHBoxLayout* contentlayout = new QHBoxLayout();//水平
	mainlayout->addLayout(contentlayout);
	//添加表单和照片区域
	QGroupBox* formGroup = createFormGroup();
	QGroupBox* photoGroup = createPhotoGroup();
	contentlayout->addWidget(formGroup, 1);//1控制布局比例
	contentlayout->addWidget(photoGroup, 1);
	//添加按钮区域
	QHBoxLayout* btnLayout = new QHBoxLayout();
	QPushButton* btnConfirm = new QPushButton(tr("确认"));
	QPushButton* btnCancel = new QPushButton(tr("取消"));
	//配置按钮
	btnConfirm->setFixedWidth(150);
	btnCancel->setFixedWidth(150);
	//添加按钮到布局
	btnLayout->addStretch();//添加一个弹簧
	btnLayout->addWidget(btnConfirm);
	btnLayout->addWidget(btnCancel);
	btnLayout->addStretch();
	//链接按钮信号
	connect(btnConfirm, &QPushButton::clicked, &dlg, &QDialog::accept);
	connect(btnCancel, &QPushButton::clicked, &dlg, &QDialog::reject);
	mainlayout->addLayout(btnLayout);
	//执行对话框
	if (dlg.exec() == QDialog::Accepted)
		handleDialogAccepted(formGroup, photoGroup);
}

void stuInfoWidget::on_btnDelLine_clicked()
{
	auto selected = ui.tableWidget->selectionModel()->selectedRows();
	if (selected.isEmpty()) {
		QMessageBox::warning(this, "警告", "没有选择行");
		return;
	}
	QSqlDatabase::database().transaction();
	for (const auto& index : selected) {
		QString id = ui.tableWidget->item(index.row(), 0)->text();
		QSqlQuery query;
		query.prepare("DELETE FROM studentInfo WHERE id=:id");
		query.bindValue(":id", id);
		if (!query.exec()) {
			QSqlDatabase::database().rollback();
			QMessageBox::critical(this, "错误", "删除失败：" + query.lastError().text());
			return;
		}
	}
	QSqlDatabase::database().commit();
	refreshTable();
}

void stuInfoWidget::on_btnDelItem_clicked()
{
	auto selected = ui.tableWidget->selectedItems();
	if (selected.isEmpty()) {
		QMessageBox::warning(this, "警告", "没有选择单元格！");
		return;
	}
	QSqlDatabase::database().transaction();
	for (auto item : selected) {
		int row = item->row();
		int col = item->column();
		QString id = ui.tableWidget->item(row, 0)->text();

		const QStringList columns = { "id", "name", "gender", "birthday", "join_date", "study_goal", "progress", "photo" };

		QSqlQuery query;
		query.prepare(QString("UPDATE studentInfo SET %1 =:%2 WHERE id=:id").arg(columns[col]).arg(columns[col]));
		query.bindValue(columns[col], "");
		query.bindValue(":id", id);
		if (!query.exec()) {
			QSqlDatabase::database().rollback();
			QMessageBox::critical(this, "错误", "删除失败：" + query.lastError().text());
			return;
		}
	}
	QSqlDatabase::database().commit();
	refreshTable();
}

void stuInfoWidget::handleitemChanged(QTableWidgetItem* item)
{
	const int row = item->row();
	const int col = item->column();

	if (col == stuInfoHeader::ID) {
		QMessageBox::warning(this, "警告", "不能修改ID");
		refreshTable();
		return;
	}

	const QString originalId = ui.tableWidget->item(row, 0)->text();//原始学号
	const QString columnName = QStringList{ "id", "name", "gender", "birthday", "join_date", "study_goal", "progress", "photo" } [col] ;

	//事务开始
	QSqlDatabase::database().transaction();
	try
	{
		QSqlQuery query;
		query.prepare(QString("UPDATE studentInfo SET %1 =:%2 WHERE id=:id").arg(columnName).arg(columnName));
		if (col == stuInfoHeader::PHOTO) {
			query.bindValue(":" + columnName, item->data(Qt::UserRole).toByteArray());
		}
		else
			query.bindValue(":" + columnName, item->text().trimmed());
		qDebug() << item->text().trimmed();
		query.bindValue(":id", originalId);

		if (!query.exec()) {
			throw std::runtime_error("更新失败！" + query.lastError().text().toStdString());
		}
		QSqlDatabase::database().commit();
	}
	catch (const std::exception& e)
	{
		QSqlDatabase::database().rollback();
		refreshTable();
		QMessageBox::critical(this, "错误", "操作失败：" + QString::fromUtf8(e.what()));
	}
}

//#include "stuInfoWidget.h"
//#include "ui_stuInfoWidget.h"
//#include <QSqlQuery>
//#include <QtConcurrent>
//#include <QFutureWatcher>
//#include <QDebug> // 添加 QDebug 头文件
//
//stuInfoWidget::stuInfoWidget(QWidget* parent)
//	: QWidget(parent), ui(Ui::stuInfoWidget()) // 初始化 ui 成员变量
//{
//	ui.setupUi(this);
//	connect(this, &stuInfoWidget::photoScaled, this, &stuInfoWidget::onPhotoScaled);
//
//	// 将 connect 移动到构造函数中，只连接一次
//	connect(&futureWatcher, &QFutureWatcher<QPixmap>::finished, [this]() {
//		if (futureWatcher.future().isFinished()) {
//			QPixmap scaledPhoto = futureWatcher.result();
//			int row = futureWatcher.property("row").toInt(); // 获取行号
//			QByteArray photoData = futureWatcher.property("photoData").toByteArray(); // 获取原始数据
//
//			emit photoScaled(row, photoData, scaledPhoto);
//			qDebug() << "onPhotoScaled emitted for row:" << row; // 调试信息
//		}
//		else {
//			qDebug() << "Future not finished!";
//		}
//		});
//
//	refreshTable();
//}
//
//stuInfoWidget::~stuInfoWidget()
//{
//	
//}
//
//
//void stuInfoWidget::refreshTable()
//{
//	ui.tableWidget->blockSignals(true);
//	ui.tableWidget->setRowCount(0);
//
//	QSqlQuery query("SELECT * FROM studentInfo");
//	while (query.next()) {
//		int row = ui.tableWidget->rowCount();
//		ui.tableWidget->insertRow(row);
//
//		for (int col = 0; col < ui.tableWidget->columnCount(); ++col) { // Use columnCount() here
//			QTableWidgetItem* item = new QTableWidgetItem();
//			item->setTextAlignment(Qt::AlignCenter);
//
//			if (col == ui.tableWidget->columnCount() - 1) {//处理照片
//				QByteArray photoData = query.value(col).toByteArray();
//				if (!photoData.isEmpty()) {
//					// 将 photoData 传递给 lambda 表达式
//					QFuture<QPixmap> future = QtConcurrent::run([photoData]() -> QPixmap {
//						QPixmap photo;
//						photo.loadFromData(photoData);  // 在线程内部加载
//						return photo.scaled(100, 100, Qt::KeepAspectRatio);
//						});
//
//					// 设置 future 的属性，传递 row 和 photoData
//					futureWatcher.setProperty("row", row);
//					futureWatcher.setProperty("photoData", photoData);
//
//					futureWatcher.setFuture(future);
//
//					item->setData(Qt::UserRole, photoData);
//				}
//			}
//			else {
//				item->setText(query.value(col).toString());
//			}
//			ui.tableWidget->setItem(row, col, item);
//		}
//	}
//	ui.tableWidget->blockSignals(false);
//}
//
//
//void stuInfoWidget::onPhotoScaled(int row, QByteArray photoData, QPixmap scaledPhoto)
//{
//	QTableWidgetItem* item = ui.tableWidget->item(row, ui.tableWidget->columnCount() - 1); // 获取对应的 item
//	if (item) {
//		item->setData(Qt::DecorationRole, scaledPhoto);
//	}
//	qDebug() << "Photo scaled for row:" << row;  //添加调试信息
//}
