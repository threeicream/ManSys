#pragma once

#include <QWidget>
#include <QSettings>

class Settings : public QWidget
{
	Q_OBJECT
public:
	static Settings& getInstance();
	QString getDatabasePath()const;
	void setDatabasePath(const QString& path);
	bool isLoginCacheEnabled() const;
	void setLoginCacheEnabled(bool enabled);
	QString getLastLoginUser() const;
	void setLastLoginUser(const QString& user);

	// 禁止拷贝构造函数和赋值运算符，确保单例只有一个实例
	Settings(const Settings&) = delete;
	Settings& operator=(const Settings&) = delete;
private:
	Settings(QWidget* parent = nullptr);
	~Settings();
private:
	QSettings settings;
};
