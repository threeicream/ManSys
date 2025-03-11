#pragma once

#include <QDialog>
class QLineEdit;
class QPushButton;
class QCheckBox;

const QByteArray encryptionKey = "your_encryption_key";

class LoginDialog : public QDialog
{
	Q_OBJECT
private:
	QLineEdit* usernameEdit;
	QLineEdit* passworEdit;
	QPushButton* loginButton;
	QPushButton* cancelButton;
	QCheckBox* cacheCheckBox;
private:
	void checkAndCreateInitialUser();
	QString encryptPassword(const QString& password);//加密函数:配置文件
	QString decryptPassword(const QString& encryptedPassword);//解密函数:配置文件
	QString hashPassword(const QString& password);//加密函数:数据库
	bool validateUser(const QString& username, const QString& userpassword);
	void saveCredentials(const QString& username, const QString& userpassword);
	bool loadCredentials(QString& username, QString& password);
private slots:
	void do_loginButton_clicked();
	void do_checked(int state);
public:
	LoginDialog(QDialog* parent = nullptr);
	~LoginDialog();
};
