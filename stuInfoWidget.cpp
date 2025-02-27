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

stuInfoWidget::stuInfoWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	refreshTable();

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

		for (int col = 0; col < ui.tableWidget->colorCount(); ++col) {
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
			else
				item->setText(query.value(col).toString());//处理其他列
			ui.tableWidget->setItem(row, col, item);
		}
		ui.tableWidget->blockSignals(false);
	}
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
	QComboBox* learningGoalLineEdit = new QComboBox;
	learningGoalLineEdit->setObjectName("learningGoalLineEdit");
	learningGoalLineEdit->addItems({ "20%","30%", "40%", "50%", "60%", "70%", "80%", "90%", "100%" });

	QLabel* currentProgressLabel = new QLabel("当前进度:");
	QLineEdit* currentProgressLineEdit = new QLineEdit;
	currentProgressLineEdit->setObjectName("currentProgressLineEdit");

	// 基本信息布局
	formLayout->addRow(idLabel, idLineEdit);
	formLayout->addRow(nameLabel, nameLineEdit);
	formLayout->addRow(genderLabel, genderComboBox);
	formLayout->addRow(birthDateLabel, birthDateEdit);
	formLayout->addRow(enrollmentDateLabel, enrollmentDateEdit);
	formLayout->addRow(learningGoalLabel, learningGoalLineEdit);
	formLayout->addRow(currentProgressLabel, currentProgressLineEdit);

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
		QString fileName = QFileDialog::getOpenFileName(this, "打开照片", QDir::currentPath(), "Images (*.png *.jpg *.ico *.jpeg);;all files (*.*)");
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
