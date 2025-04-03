#include "SystemSettingWidget.h"
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QTextEdit>
#include <QGridLayout>
#include <QLabel>
#include <QFileDialog>
#include "Settings.h"
#include "DataBaseManSys.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>

void SystemSettingWidget::setupUI()
{
	dbPathEdit = new QLineEdit(this);
	browseBtn = new QPushButton("浏览...", this);
	oldPwdEdit = new QLineEdit(this);
	newPwdEdit = new QLineEdit(this);
	confirmPwdEdit = new QLineEdit(this);
	cacheCheckBox = new QCheckBox("记住登录信息", this);
	saveBtn = new QPushButton(" 保存", this);
	versionInfoEdit = new QTextEdit(this); 
	
	oldPwdEdit->setEchoMode(QLineEdit::Password);
	newPwdEdit->setEchoMode(QLineEdit::Password);
	confirmPwdEdit->setEchoMode(QLineEdit::Password);

	versionInfoEdit->setPlainText("XX管理系统1.0\n开发环境：vs2022,Qt6.8,c++,win10");
	versionInfoEdit->setReadOnly(true);

	mainLayout = new QGridLayout(this);
	mainLayout->addWidget(new QLabel("数据库：",this), 0, 0);
	mainLayout->addWidget(dbPathEdit, 0, 1);
	mainLayout->addWidget(browseBtn, 0, 2);
	mainLayout->addWidget(new QLabel("旧密码：",this), 1, 0);
	mainLayout->addWidget(oldPwdEdit, 1, 1, 1, 2);
	mainLayout->addWidget(new QLabel("新密码：", this), 2, 0);
	mainLayout->addWidget(newPwdEdit, 2, 1, 1, 2);
	mainLayout->addWidget(new QLabel("确认密码：", this), 3, 0);
	mainLayout->addWidget(confirmPwdEdit, 3, 1, 1, 2);
	mainLayout->addWidget(cacheCheckBox, 4, 0, 1, 3);
	mainLayout->addWidget(saveBtn, 5, 1, 1, 2);
	mainLayout->addWidget(versionInfoEdit, 6, 0, 1, 3);//占据的行数和列数(跨度)。
	this->setLayout(mainLayout);

	connect(browseBtn, &QPushButton::clicked, this, &SystemSettingWidget::browseDatabasePath);
	connect(saveBtn, &QPushButton::clicked, this, &SystemSettingWidget::saveSettings);

}

void SystemSettingWidget::loadSettings()
{
	dbPathEdit->setText(Settings::getInstance().getDatabasePath());
	cacheCheckBox->setChecked(Settings::getInstance().isLoginCacheEnabled());

}

bool SystemSettingWidget::validatePasswordChange()
{
	if (newPwdEdit->text() != confirmPwdEdit->text()) {
		QMessageBox::warning(this, "错误", "两次密码不一致");
		return false;
	}
	QString currentUser = Settings::getInstance().getLastLoginUser();
	if (currentUser.isEmpty()) {
		QMessageBox::warning(this, "错误", "未找到当前用户");
		return false;
	}
	QSqlQuery query;
	query.prepare("SELECT password FROM users WHERE username=:username");
	query.bindValue(":username", currentUser);
	if (!query.exec() || !query.next()) {
		QMessageBox::warning(this, "错误", "查询失败：" + query.lastError().text());
		return false;
	}
	QString storeHash = query.value(0).toString();
	QString inputHash = QString(QCryptographicHash::hash(oldPwdEdit->text().toUtf8(),
		QCryptographicHash::Sha256).toHex());
	if (storeHash != inputHash) {
		QMessageBox::warning(this, "错误", "原密码不一致！");
		return false;
	}
	return true;
}

void SystemSettingWidget::browseDatabasePath()
{
	QString dbpath = Settings::getInstance().getDatabasePath();
	QString path = QFileDialog::getOpenFileName(this, "选择数据库路径", dbpath, "db(*.db *.db3 *.sqlite);;all(*.*)");
	if (!path.isEmpty())
		dbPathEdit->setText(path);
}

void SystemSettingWidget::updatePassword()
{
	if (!validatePasswordChange()) //当前输入密码是否合规
		return;
	QString newHash = QString(QCryptographicHash::hash(newPwdEdit->text().toUtf8(),
		QCryptographicHash::Sha256
	).toHex());

	QSqlQuery query;
	query.prepare("UPDATE users SET password = ? WHERE username = ?");
	query.addBindValue(newHash);
	query.addBindValue(/*Settings::getInstance().getLastLoginUser()*/Settings::getInstance().getQSettings().value("username").toString());
	if (!query.exec()) {
		QMessageBox::critical(this, "错误", "密码更新失败：" + query.lastError().text()); 
		return;
	}
	QMessageBox::information(this, "提示", "密码更新成功");
}

void SystemSettingWidget::saveSettings()
{
	QString newDbPath = dbPathEdit->text();

	if (!newPwdEdit->text().isEmpty()) {
		updatePassword();
	}
	if (newDbPath != Settings::getInstance().getDatabasePath()) {
		QMessageBox::information(this, "提示", "数据库路径将在重启后修改");
		Settings::getInstance().setDatabasePath(newDbPath);
		Settings::getInstance().setLoginCacheEnabled(cacheCheckBox->isChecked());
		DataBaseManSys::Instance().setDataBasePath(newDbPath);
	}
}

SystemSettingWidget::SystemSettingWidget(QWidget *parent)
	: QWidget(parent)
{
	this->setFixedSize(400, 600);
	setupUI();
	loadSettings();
}

SystemSettingWidget::~SystemSettingWidget()
{
}
