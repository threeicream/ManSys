#include "Settings.h"

Settings::Settings(QWidget *parent)
	: QWidget(parent), settings("config.ini", QSettings::IniFormat)
{
}

Settings::~Settings()
{
}

Settings& Settings::getInstance()
{
	// 静态局部变量，在第一次调用时初始化，之后每次调用都会返回相同的实例
	static Settings instance;
	return instance;
}

QString Settings::getDatabasePath() const
{
	return settings.value("database/path", "").toString(); // 默认值为空字符串
}

void Settings::setDatabasePath(const QString& path)
{
	settings.setValue("database/path", path);
}

bool Settings::isLoginCacheEnabled() const
{
	return settings.value("login/cacheEnabled", false).toBool(); // 默认值为false
}

void Settings::setLoginCacheEnabled(bool enabled)
{
	settings.setValue("login/cacheEnabled", enabled);
}

QString Settings::getLastLoginUser() const
{
	return settings.value("login/lastUser", "").toString(); // 默认值为空字符串
}

void Settings::setLastLoginUser(const QString& user)
{
	settings.setValue("login/lastUser", user);
}
