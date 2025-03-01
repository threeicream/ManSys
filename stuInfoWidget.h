#pragma once

#include <QWidget>
#include "ui_stuInfoWidget.h"
#include <QtConcurrent>
class QGroupBox;

class stuInfoWidget : public QWidget
{
	Q_OBJECT

public:
	stuInfoWidget(QWidget *parent = nullptr);
	~stuInfoWidget();
private:
	void refreshTable();
	QGroupBox* createFormGroup();
	QGroupBox* createPhotoGroup();
	void handleDialogAccepted(QGroupBox* formGroup, QGroupBox* photoGroup);
private:
	Ui::stuInfoWidget ui;
	QByteArray photoData;
	//QFutureWatcher<QPixmap> futureWatcher; // 添加这个成员变量
signals:
	//void photoScaled(int row, QByteArray photoData, QPixmap scaledPhoto);

private slots:
	//void onPhotoScaled(int row, QByteArray photoData, QPixmap scaledPhoto);
	void on_btnAdd_clicked();
	void on_btnDelLine_clicked();
	void on_btnDelItem_clicked();
};
