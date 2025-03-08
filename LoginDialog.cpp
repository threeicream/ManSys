#include "LoginDialog.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDialog>
#include <qDebug>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>

void LoginDialog::checkAndCreateInitialUser()
{
	const QString initialUsername = "admin";// 初始用户名和密码 
	const QString initialPassword = "admin123";
	QSqlQuery query;
	query.exec("SELECT COUNT (*) FROM users");
	if (query.next() && query.value(0).toInt() == 0){ // 检查 users 表是否为空,表为空, 插入初始用户 QString hashedIni tialPassword = hashPassword(ini tialPassword);
		query.prepare("INSERT INTO users (username, password) VALUES (:username, : password) ");
		query.bindValue(":username", initialUsername);
		//query.bindValue(":password", hashedInitialPassword);
		if (!query.exec()) 
			qDebug() << "插入初始用户失败：" << query.lastError().text();
	}
}

bool LoginDialog::loadCredentials(QString name, QString password)
{
	return false;
}

void LoginDialog::do_loginButton_clicked()
{

}

LoginDialog::LoginDialog(QDialog *parent)
	: QDialog(parent)
{
	checkAndCreateInitialUser();
	this->setWindowTitle("xx管理系统");
	this->setWindowIcon(QIcon(":/MainWindow/image/search.png"));
	this->setFixedSize(260, 180);

	QLabel* usernameLabel = new QLabel("用户名：", this);
	QLabel* passwordLabel = new QLabel("密码：", this);
	usernameEdit = new QLineEdit(this);
	passworEdit = new QLineEdit(this);
	passworEdit->setEchoMode(QLineEdit::Password);//密码模式
	loginButton = new QPushButton("登录", this);
	cancelButton = new QPushButton("取消", this);
	//布局
	QGridLayout* mainlayout = new QGridLayout(this);
	mainlayout->addWidget(usernameLabel, 0, 0);
	mainlayout->addWidget(usernameEdit, 0, 1); 
	mainlayout->addWidget(passwordLabel, 1, 0); 
	mainlayout->addWidget(passworEdit, 1, 1) ; 
	QHBoxLayout* buttonLayout = new QHBoxLayout;
	buttonLayout->addWidget(loginButton);
	buttonLayout-> addWidget(cancelButton);
	mainlayout->addLayout(buttonLayout, 2, 0, 1, 2) ; // 跨两列 
	setLayout(mainlayout);

	connect(loginButton, &QPushButton::clicked, this, &LoginDialog::do_loginButton_clicked);
	connect(cancelButton, &QPushButton::clicked, this, &LoginDialog::reject);
	//尝试加载缓存的登录信息
	QString cachedUsername, cachedPassword;
	if (loadCredentials(cachedUsername, cachedPassword)) {
		usernameEdit->setText(cachedUsername);
		passworEdit->setText(cachedPassword);
	}
}

LoginDialog::~LoginDialog()
{
}
