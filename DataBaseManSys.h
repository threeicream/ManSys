#pragma once

#include <QObject>
#include <QSqlDatabase>

class DataBaseManSys : public QObject
{
	Q_OBJECT
public:
	static DataBaseManSys& Instance();
	void closeDataBase();
	bool openDataBase(const QString& path);
	QString getDataBasePath()const;
	void setDataBasePath(const QString& path);
	bool copyDatabase(const QString& sourcePath, const QString& destinationPath);
private:
	explicit DataBaseManSys(QObject* parent = nullptr);
	~DataBaseManSys();
private:
	QSqlDatabase db;
	QString dbPath /*= (QString)DATABASE_PATH*/ /*+ "/sql/ManageSys.db"*/;
};
