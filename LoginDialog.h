#pragma once

#include <QDialog>
class QLineEdit;
class QPushButton;

class LoginDialog : public QDialog
{
	Q_OBJECT
private:
	QLineEdit* usernameEdit;
	QLineEdit* passworEdit;
	QPushButton* loginButton;
	QPushButton* cancelButton;
private:
	void checkAndCreateInitialUser();
	bool loadCredentials(QString name, QString password);
private slots:
	void do_loginButton_clicked();
public:
	LoginDialog(QDialog* parent = nullptr);
	~LoginDialog();
};
