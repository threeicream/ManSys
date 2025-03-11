#pragma once

#include <QWidget>
class QPushButton;
class QLineEdit;
class QCheckBox;
class QGridLayout;
class QTextEdit;

class SystemSettingWidget : public QWidget
{
	Q_OBJECT
private:
	void setupUI();
	void loadSettings();//加载数据库设置
	bool validatePasswordChange();//判断能否修改密码
private slots:
	void browseDatabasePath();//选择数据库存储位置
	void updatePassword();//修改密码
	void saveSettings();
public:
	SystemSettingWidget(QWidget* parent = nullptr);
	~SystemSettingWidget();
private:
	enum USerEnum { ID, USERNAME, PASSWORD };
	QLineEdit* dbPathEdit;
	QPushButton* browseBtn;
	QLineEdit* oldPwdEdit;
	QLineEdit* newPwdEdit;
	QLineEdit* confirmPwdEdit;
	QCheckBox* cacheCheckBox;
	QPushButton* saveBtn;
	QTextEdit* versionInfoEdit;
	QGridLayout* mainLayout;
};
