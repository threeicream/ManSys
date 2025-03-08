#pragma once

#include <QWidget>
#include <QStringList>
#include <QLabel>

class QChartView;
class QTableWidget;
class QComboBox;
class QPushButton;
class QDateEdit;
class QPieSlice;
class QTableWidgetItem;
class QScrollArea;
class QGridLayout;
class ClickableLabel;

constexpr int imgH = 500;
constexpr int imgW = 300;

class HonorWallWidget : public QWidget
{
	Q_OBJECT

public:
	HonorWallWidget(QWidget* parent = nullptr);
	~HonorWallWidget();
private:
	void setupUI();
	void loadImagesFromDatabase();
	void takeSQLHeader();
	void addImageToWall(const QString& imgPath);
	void addImageToUI(const QPixmap& pixmap);
	void recordImages();
private slots:
	void addImage();
	void modifyImage();
	void deleteImage();
	void onImageClicked();
private:
	enum HonorEnum { ID, IMAGEDATA, DESCRIPTION, ADDEDDATE };
	QPushButton* addBtn;
	QPushButton* modifyBtn;
	QPushButton* delBtn;
	QScrollArea* scrollArea;
	QWidget* contentWidget;
	QGridLayout* gridLayout;
	QStringList fieldNames;
	ClickableLabel* selectedLabel = nullptr;
};


class ClickableLabel :public QLabel 
{
	Q_OBJECT

public:
	explicit ClickableLabel(QWidget* parent = nullptr) :QLabel(parent) {}
signals:
	void clicked();
protected:
	void mousePressEvent(QMouseEvent* event)override {
		emit clicked();
		QLabel::mousePressEvent(event);
	}
};