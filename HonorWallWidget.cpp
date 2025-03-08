#include "HonorWallWidget.h"
#include <QSqlQuery>
#include <QDialog>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QScrollArea>
#include <QFormLayout>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlRecord>
#include <QLayoutItem>
#include <QStandardPaths>
#include <QBuffer>

HonorWallWidget::HonorWallWidget(QWidget *parent)
	: QWidget(parent)
{
	setupUI();
	takeSQLHeader();
	loadImagesFromDatabase();
}

HonorWallWidget::~HonorWallWidget()
{
	if (selectedLabel)
		delete selectedLabel;
}

void HonorWallWidget::setupUI()
{
	//主布局
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	//按钮布局
	QHBoxLayout* buttonLayout = new QHBoxLayout;
	//添加按钮
	addBtn = new QPushButton("添加图片", this);
	connect (addBtn, &QPushButton::clicked, this, &HonorWallWidget::addImage);
	buttonLayout->addWidget(addBtn);
	//修改按钮
	modifyBtn = new QPushButton ("修改图片", this);
	connect(modifyBtn, &QPushButton::clicked, this, &HonorWallWidget::modifyImage);
	buttonLayout->addWidget(modifyBtn);
	//删除按钮
	delBtn = new QPushButton("删除图片", this);
	connect(delBtn, &QPushButton::clicked, this, &HonorWallWidget::deleteImage);
	buttonLayout->addWidget(delBtn);
	mainLayout->addLayout(buttonLayout);
	//滚动区域
	scrollArea = new QScrollArea(this);
	scrollArea->setWidgetResizable(true); //允许内容区域调整大小 
	//内容区域
	contentWidget = new QWidget(scrollArea);
	gridLayout = new QGridLayout(contentWidget);//布局管理器
	contentWidget->setLayout(gridLayout);
	//设置滚动区域的内容
	scrollArea->setWidget(contentWidget);//明确地告诉 QScrollArea 哪个部件是它需要管理和滚动的具体内容。
	mainLayout->addWidget(scrollArea);
	//设置主布局
	this->setLayout(mainLayout);
}

void HonorWallWidget::loadImagesFromDatabase()
{
	QLayoutItem* item;
	while ((item = gridLayout->takeAt(0)) != nullptr)//这个函数只是从布局中移除了这个布局项，并没有删除它。
		delete item;

	QSqlQuery query("SELECT id, image_data FROM honorWall");
	if (query.exec()) {
		while (query.next()) {
			int id = query.value(HonorEnum::ID).toInt();
			QByteArray imageData = query.value(HonorEnum::IMAGEDATA).toByteArray();

			//将二进制数据转换为QPixmap
			QPixmap pixmap;
			pixmap.loadFromData(imageData);

			if (!pixmap.isNull()) {
				//将图片显示在界面
				ClickableLabel* imageLabel = new ClickableLabel(contentWidget);
				QPixmap scaledPixmap = pixmap.scaled(imgW, imgH, Qt::KeepAspectRatio, Qt::SmoothTransformation);//只能缩放+平滑
				imageLabel->setPixmap(scaledPixmap);
				imageLabel->setAlignment(Qt::AlignCenter);
				imageLabel->setStyleSheet("border: 1px solid #ccc; padding: 5px; ");
				imageLabel->setProperty("id", id); // 设置id属性

				connect(imageLabel, &ClickableLabel::clicked, this, &HonorWallWidget::onImageClicked); // 动态添加到网格布局
				int row = gridLayout->count() / 3; // 每行3张图片
				int col = gridLayout->count() % 3;
				gridLayout->addWidget(imageLabel, row, col);
			}
			else
				qWarning() << "无法加载图片";
		}
	}
}

void HonorWallWidget::takeSQLHeader()
{
	QSqlQuery query;
	query.exec("SELECT * FROM honorWall");

	QSqlRecord record = query.record(); // 获取 QSqlRecord 对象

	int fieldCount = record.count(); // 获取列数

	for (int i = 0; i < fieldCount; ++i) {
		fieldNames << record.fieldName(i); // 获取第 i 列的名称
	}
}

void HonorWallWidget::addImageToWall(const QString& imgPath)
{
	QPixmap pixmap(imgPath);
	if (pixmap.isNull()) {
		QMessageBox::warning(this, "错误", "无法加载图片");
		return;
	}
	QByteArray imgData;
	QBuffer buffer(&imgData);
	buffer.open(QIODevice::WriteOnly);
	pixmap.save(&buffer, "PNG");//保存到buffer内存中，然后buffer传递给imgData

	QSqlQuery insertquery;
	insertquery.prepare(
		"INSERT INTO honorWall "
		"(image_data, description, added_date) "
		"VALUES(:image_data, :description, :added_date)"
	);
	insertquery.bindValue(":image_data", imgData);
	insertquery.bindValue(":description", "未描述");
	insertquery.bindValue(":added_date", QDate::currentDate().toString());

	if (!insertquery.exec()) {
		qWarning() << "插入图片数据失败:" << insertquery.lastError().text();
		return;
	}
	addImageToUI(pixmap);
}

void HonorWallWidget::addImageToUI(const QPixmap& pixmap)
{
	if (pixmap.isNull()) {
		qWarning() << "插入图片数据失败";
		return;
	}
	QPixmap scaledPixmap = pixmap.scaled(imgW, imgH, Qt::KeepAspectRatio, Qt::SmoothTransformation);//只能缩放+平滑

	ClickableLabel* imageLabel = new ClickableLabel(contentWidget);
	imageLabel->setPixmap(scaledPixmap);
	imageLabel->setAlignment(Qt::AlignCenter);
	imageLabel->setStyleSheet("border: 1px solid #ccc; padding: 5px; ");

	connect(imageLabel, &ClickableLabel::clicked, this, &HonorWallWidget::onImageClicked); // 动态添加到网格布局
	int row = gridLayout->count() / 3; // 每行3张图片
	int col = gridLayout->count() % 3;
	gridLayout->addWidget(imageLabel, row, col);
}

void HonorWallWidget::recordImages()
{
	//清空布局中的所有内容
	QLayoutItem* item;
	while ((item = gridLayout->takeAt(0)) != nullptr) {
		if (item->widget()) {
			item->widget()->setParent(nullptr);
			item->widget()->deleteLater();
		}
		delete item;
	}
	loadImagesFromDatabase();
}

void HonorWallWidget::modifyImage()
{
	if (!selectedLabel) {
		QMessageBox::warning(this, "警告", "没有选择图片");
		return;
	}

	QString imgPath = QFileDialog::getOpenFileName(this, "添加图片", QStandardPaths::writableLocation(QStandardPaths::PicturesLocation), "Images (*.png *.jpg *.ico *.jpeg);;all files (*.*)");
	if (imgPath.isEmpty())
		return;
	QPixmap pixmap(imgPath);
	if (pixmap.isNull()) {
		QMessageBox::warning(this, "错误", "无法加载图片");
		return;
	}
	QByteArray imgData;
	QBuffer buffer(&imgData);
	buffer.open(QIODevice::WriteOnly);
	pixmap.save(&buffer, "PNG");//保存到buffer内存中，然后buffer传递给imgData

	int id = selectedLabel->property("id").toInt();
	QSqlDatabase::database().transaction();
	QSqlQuery query;
	query.prepare("UPDATE honorWall SET image_data =:image_data WHERE id=:id");
	query.bindValue(":image_data", imgData);
	query.bindValue(":id", id);
	if (!query.exec()) {
		QSqlDatabase::database().rollback();//回滚
		QMessageBox::critical(this, "错误", "图片更新失败：" + query.lastError().text());
	}
	else {
		QSqlDatabase::database().commit();//提交
		QMessageBox::information(this, "成功", "图片更新成功！");
	}

	QPixmap scaledPixmap = pixmap.scaled(imgW, imgH, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	selectedLabel->setPixmap(scaledPixmap);
}

void HonorWallWidget::deleteImage()
{
	QMessageBox confirmBox(this);
	confirmBox.setWindowTitle("删除");
	confirmBox.setText("确认删除图片？");

	QPushButton* btnyes = confirmBox.addButton("确定", QMessageBox::YesRole);
	QPushButton* btncancel = confirmBox.addButton("取消", QMessageBox::NoRole);

	confirmBox.setDefaultButton(btncancel);

	confirmBox.exec();

	if (confirmBox.clickedButton() != btnyes) {
		confirmBox.close();
		return;
	}

	if (!selectedLabel) {
		QMessageBox::warning(this, "警告", "没有选择图片");
		return;
	}

	int id = selectedLabel->property("id").toInt();
	//数据库删除图片
	QSqlDatabase::database().transaction();
	QSqlQuery query;
	query.prepare("DELETE FROM honorWall WHERE id=:id");
	query.bindValue(":id", id);
	if (!query.exec()) {
		QSqlDatabase::database().rollback();//回滚
		QMessageBox::critical(this, "错误", "图片删除失败：" + query.lastError().text());
	}
	else {
		QSqlDatabase::database().commit();//提交
		QMessageBox::information(this, "成功", "图片删除成功！");
	}

	gridLayout->removeWidget(selectedLabel);
	delete selectedLabel;
	selectedLabel = nullptr;
	recordImages();
}

void HonorWallWidget::onImageClicked()
{
	if (selectedLabel) {
		selectedLabel->setStyleSheet("border: 1px solid #ccc; padding: 5px");
	}
	selectedLabel = qobject_cast<ClickableLabel*>(sender());
	if (selectedLabel) {
		selectedLabel->setStyleSheet("border: 2px solid red; padding: 5px");
	}
}

void HonorWallWidget::addImage()
{
	QString imgPath=QFileDialog::getOpenFileName(this,"添加图片", QStandardPaths::writableLocation(QStandardPaths::PicturesLocation), "Images (*.png *.jpg *.ico *.jpeg);;all files (*.*)");
	if (!imgPath.isEmpty())
		addImageToWall(imgPath);
}
