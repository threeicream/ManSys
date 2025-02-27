#include "DataBaseManSys.h"
//#include <QMessageBox>
#include <QSqlError>

DataBaseManSys& DataBaseManSys::Instance()
{
	static DataBaseManSys instance;
	return instance;
}

void DataBaseManSys::closeDataBase()
{
	if (db.isOpen())
		db.close();
}

bool DataBaseManSys::openDataBase(const QString& path)
{
	db.setDatabaseName(path);
	if (!db.open()) {
		qDebug() << "无法打开数据库" << db.lastError().text();
		return false;
	}
	return true;
}

QString DataBaseManSys::getDataBasePath() const
{
	return dbPath;
}

void DataBaseManSys::setDataBasePath(const QString& path)
{
	dbPath = path;
	closeDataBase();
	openDataBase(dbPath);
}

DataBaseManSys::DataBaseManSys(QObject* parent)
	: QObject(parent)
{
	db = QSqlDatabase::addDatabase("QSQLITE");
	openDataBase(dbPath);

}

DataBaseManSys::~DataBaseManSys()
{
	closeDataBase();
}
