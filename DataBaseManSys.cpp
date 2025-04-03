#include "DataBaseManSys.h"
//#include <QMessageBox>
#include <QSqlError>
#include "Settings.h"
#include <QFile>
#include <QDir>
#include <QFileInfo>

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
	closeDataBase();
	copyDatabase(dbPath, path);
	dbPath = path;
	openDataBase(dbPath);
}

bool DataBaseManSys::copyDatabase(const QString& sourcePath, const QString& destinationPath)
{
	QFile sourceFile(sourcePath);
	if (!sourceFile.exists()) {
		qDebug() << "Source file does not exist: " << sourcePath;
		return false;
	}

	QDir destinationDir(QFileInfo(destinationPath).path());
	if (!destinationDir.exists()) {
		if (destinationDir.mkpath(".")) {  // 递归创建目录
			qDebug() << "Destination directory created successfully.";
		}
		else {
			qDebug() << "Failed to create destination directory.";
			return false; // 或者抛出一个异常
		}
	}

	// 如果目标文件存在，删除它 (可选)
	if (QFile::exists(destinationPath)) {
		if (!QFile::remove(destinationPath)) {
			qDebug() << "Failed to remove existing destination file: " << destinationPath;
			return false;
		}
		qDebug() << "Destination file already exists. Overwriting.";
	}
	if (sourceFile.copy(destinationPath)) {
		qDebug() << "Database file copied successfully to: " << destinationPath;
		sourceFile.close(); //关闭文件，释放资源
		return true;
	}
	else {
		qDebug() << "Failed to copy database file from " << sourcePath << " to " << destinationPath;
		qDebug() << "Error: " << sourceFile.errorString();
		sourceFile.close(); //关闭文件，释放资源
		return false;
	}
}

DataBaseManSys::DataBaseManSys(QObject* parent)
	: QObject(parent)
{
	dbPath = Settings::getInstance().getDatabasePath();
	db = QSqlDatabase::addDatabase("QSQLITE");
	openDataBase(dbPath);

}

DataBaseManSys::~DataBaseManSys()
{
	closeDataBase();
}
