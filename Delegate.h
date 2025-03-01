#pragma once

#include <QItemDelegate>
#include <QComboBox>
#include <QDateEdit>
#include <QLabel>
#include <QPainter>
#include <QByteArray>
#include <QDebug>
#include <QMouseEvent>
#include <QFileDialog>
#include <QFile>

class ComboBoxDelegate : public QItemDelegate {
public:
	ComboBoxDelegate(const QStringList& items, QObject* parent = nullptr) :
		QItemDelegate(parent),
		m_items(items) {
	}

	// 创建编辑器 (QComboBox)
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
		QComboBox* comboBox = new QComboBox(parent);
		comboBox->addItems(m_items);
		/*QAbstractItemView* view = comboBox->view();
		if (view) {
			for (int i = 0; i < view->model()->rowCount(); ++i) {
				QModelIndex itemIndex = view->model()->index(i, 0);
				QVariant variant = view->model()->data(itemIndex, Qt::TextAlignmentRole);
				if (!variant.isValid() || variant.isNull()) {
					view->model()->setData(itemIndex, Qt::AlignCenter, Qt::TextAlignmentRole);
				}
			}
		}*/
		return comboBox;
	}

	// 设置编辑器的数据
	/*void setEditorData(QWidget* editor, const QModelIndex& index) const override {
		QComboBox* comboBox = qobject_cast<QComboBox*>(editor);
		if (comboBox) {
			comboBox->setCurrentText(index.model()->data(index, Qt::EditRole).toString());
		}
	}*/

	// 将编辑器的数据保存到 Model 中
	//void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override {
	//	QComboBox* comboBox = qobject_cast<QComboBox*>(editor);
	//	if (comboBox) {
	//		model->setData(index, comboBox->currentText(), Qt::EditRole);//设置角色为编辑角色
	//	}
	//}

	// 更新编辑器的几何形状
	/*void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
		editor->setGeometry(option.rect);
	}*/

private:
	QStringList m_items; // 下拉框的选项
};

class DateEditDelegate :public QItemDelegate {
public:
	DateEditDelegate(QObject* parent = nullptr) :
		QItemDelegate(parent){
	}

	// 创建编辑器 (QComboBox)
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
		QDateEdit* editor = new QDateEdit(parent);
		editor->setDisplayFormat("yyyy-MM-dd"); // 设置日期显示格式
		editor->setCalendarPopup(true);      // 允许弹出日历选择
		return editor;
	}

	// 将编辑器的数据保存到 Model 中
	//void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override {
	//	QDateEdit* dateEdit = qobject_cast<QDateEdit*>(editor);
	//	if (dateEdit) {
	//		model->setData(index, dateEdit->date().toString("yyyy-MM-dd"), Qt::EditRole);//设置角色为编辑角色
	//	}
	//}
};

class ImgDelegate :public QItemDelegate {
public:
	ImgDelegate(QObject* parent = nullptr) :
		QItemDelegate(parent) {
	}

	// 创建编辑器 (QComboBox)
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
		QLabel* editor = new QLabel(parent);
		return editor;
	}

	// 将编辑器的数据保存到 Model 中
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override {
		QLabel* label = qobject_cast<QLabel*>(editor);
		if (label) {
			QByteArray imgData;
			QPixmap pixmap = label->pixmap();//直接获取pixmap对象
			if (!pixmap.isNull()) {
				QBuffer buffer(&imgData);
				buffer.open(QIODevice::WriteOnly);
				pixmap.save(&buffer, "PNG");//保存到buffer内存中，然后buffer传递给imgData
			}
			model->setData(index, imgData, Qt::UserRole);//保存二进制数据到模型
		}
	}

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
		// 绘制前先保存 painter 的状态
		//painter->save();

		QByteArray imgData = index.data(Qt::UserRole).toByteArray();
		if (imgData.isEmpty()) {
			QItemDelegate::paint(painter, option, index);
			return;
		}
		QPixmap pixmap;
		pixmap.loadFromData(imgData);
		if (pixmap.isNull()) {
			QItemDelegate::paint(painter, option, index);
			return;
		}
		//绘制图片
		QRect rect = option.rect;
		QPixmap scaledPixmap = pixmap.scaled(photosize, photosize, Qt::KeepAspectRatio);//智能缩放
		painter->drawPixmap(rect, scaledPixmap);

		// 恢复 painter 的状态
		//painter->restore();

		// 不调用 QItemDelegate::paint，防止绘制默认的背景高亮
	}

	bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) {//处理与编辑器相关的事件
		if (event->type() == QEvent::MouseButtonDblClick) {
			// 双击事件
			QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
			if (mouseEvent->button() == Qt::LeftButton) {
				// 左键双击
				QString filePath = QFileDialog::getOpenFileName(nullptr, "选择图片", "", "Images (*.png *.jpg *.jpeg *.bmp)");
				if (!filePath.isEmpty()) {
					//QPixmap pixmap(filePath);
					//if (!pixmap.isNull()) {
					//	QByteArray imgData;
					//	QBuffer buffer(&imgData);
					//	buffer.open(QIODevice::WriteOnly);
					//	pixmap.save(&buffer, "PNG");
					//	buffer.close();

					//	model->setData(index, imgData, Qt::UserRole); // 保存二进制数据到模型
					//	//emit commitData(); // 提交数据
					//	//emit closeEditor();  // 关闭编辑器

					//	return true; // 事件已被处理
					//}
					QFile file(filePath);
					if (file.open(QIODevice::ReadOnly)) {
						QByteArray imgData = file.readAll();
						file.close();
						model->setData(index, imgData, Qt::UserRole);
					}
				}
				return true;
			}
		}
		return QItemDelegate::editorEvent(event, model, option, index); // 未处理，传递给父类
	}
signals:
	//void commitData();
	//void closeEditor();

private:
	int photosize = 100;
};