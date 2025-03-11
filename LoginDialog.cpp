#include "LoginDialog.h"
#include "Settings.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDialog>
#include <qDebug>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QCheckBox>

void LoginDialog::checkAndCreateInitialUser()
{
	const QString initialUsername = "admin";// 初始用户名和密码 
	const QString initialPassword = "admin123";
	QSqlQuery query;
	query.exec("SELECT COUNT (*) FROM users");
	if (query.next() && query.value(0).toInt() == 0){ // 检查 users 表是否为空,表为空, 插入初始用户 
		QString hashedInitialPassword = hashPassword(initialPassword);
		query.prepare("INSERT INTO users "
			"(username, password) "
			" VALUES (:username, :password)");
		query.bindValue(":username", initialUsername);
		query.bindValue(":password", hashedInitialPassword);
		if (!query.exec()) 
			qDebug() << "插入初始用户失败：" << query.lastError().text();
	}
}

QString LoginDialog::hashPassword(const QString& password)
{
	QByteArray passwordBytes = password.toUtf8();
	QByteArray hashBytes = QCryptographicHash::hash(passwordBytes, QCryptographicHash::Sha256);
	return QString(hashBytes.toHex());
}

bool LoginDialog::validateUser(const QString& username, const QString& userpassword)
{
	QString hashedPassword = hashPassword(userpassword);
	QSqlQuery query;
	query.prepare("SELECT * FROM users WHERE username = :username AND password = :password");
	query.bindValue(":username", username);
	query.bindValue(":password", hashedPassword);
	if (!query.exec()) {
		qDebug() << "查询错误：" << query.lastError().text();
	}
	return query.next();
}

void LoginDialog::saveCredentials(const QString& username, const QString& userpassword)
{
	//存储用户名和加密密码
	Settings::getInstance().getQSettings().setValue("username", username);
	QString encryptedPassword = encryptPassword(userpassword);
	Settings::getInstance().getQSettings().setValue("password", encryptedPassword);
}

bool LoginDialog::loadCredentials(QString& username, QString& password)
{
	username = Settings::getInstance().getQSettings().value("username").toString();
	QString encryptedPassword = Settings::getInstance().getQSettings().value("password").toString();
	if (!username.isEmpty() && !encryptedPassword.isEmpty()) {//存在该用户名和加密密码
		password = decryptPassword(encryptedPassword);
		return true;
	}
	return false;
}

QString LoginDialog::encryptPassword(const QString& password)
{
	QByteArray passwordBytes = password.toUtf8();
	QByteArray encryptedBytes;
	for (int i = 0; i < passwordBytes.size(); ++i)
		encryptedBytes.append(passwordBytes[i]^encryptionKey[i % encryptionKey.size()]);
	return encryptedBytes.toBase64();
}

QString LoginDialog::decryptPassword(const QString& encryptedPassword)
{
	QByteArray encryptedBytes = QByteArray::fromBase64(encryptedPassword.toUtf8());
	QByteArray decryptedBytes;
	for (int i = 0; i < encryptedBytes.size(); ++i)
		decryptedBytes.append(encryptedBytes[i] ^ encryptionKey[i % encryptionKey.size()]);
	return QString::fromUtf8(decryptedBytes);
}

void LoginDialog::do_loginButton_clicked()
{
	QString username= usernameEdit->text() ;
	QString password = passworEdit->text();

	if (validateUser(username, password)) {//验证
		// 登录成功,保存登录信息
		saveCredentials(username, password);
		// 将当前登录的用户名保存到 Settings 中
		Settings::getInstance().setLastLoginUser(username);
		accept(); // 关闭对话框
	}
	else
		QMessageBox::warning(this,"登录失败","用户名或密码错误。"); 
	
}

void LoginDialog::do_checked(int state)
{
	Settings::getInstance().setLoginCacheEnabled(cacheCheckBox->isChecked());
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
	cacheCheckBox = new QCheckBox("记住登录信息", this);
	//布局
	QGridLayout* mainlayout = new QGridLayout(this);
	mainlayout->addWidget(usernameLabel, 0, 0);
	mainlayout->addWidget(usernameEdit, 0, 1); 
	mainlayout->addWidget(passwordLabel, 1, 0); 
	mainlayout->addWidget(passworEdit, 1, 1) ; 
	mainlayout->addWidget(cacheCheckBox, 2, 0);
	QHBoxLayout* buttonLayout = new QHBoxLayout;
	buttonLayout->addWidget(loginButton);
	buttonLayout-> addWidget(cancelButton);
	mainlayout->addLayout(buttonLayout, 3, 0, 1, 2) ; // 跨两列
	setLayout(mainlayout);

	cacheCheckBox->setChecked(Settings::getInstance().isLoginCacheEnabled());

	connect(loginButton, &QPushButton::clicked, this, &LoginDialog::do_loginButton_clicked);
	connect(cancelButton, &QPushButton::clicked, this, &LoginDialog::reject);
	connect(cacheCheckBox, &QCheckBox::stateChanged,this,&LoginDialog::do_checked);

	//尝试加载缓存的登录信息
	QString cachedUsername, cachedPassword;
	if (Settings::getInstance().isLoginCacheEnabled()
		&& loadCredentials(cachedUsername, cachedPassword)) {
		usernameEdit->setText(cachedUsername);
		passworEdit->setText(cachedPassword);
	}
}

LoginDialog::~LoginDialog()
{
}
