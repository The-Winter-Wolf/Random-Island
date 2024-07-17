#pragma once
#include "cSQLite.h"

#include <sqlite/sqlite3.h>
#include <string>
#include <vector>

class cSQLiteImp
{
public:
	cSQLiteImp();
	~cSQLiteImp();

	bool InitDatabase(std::string dbName);

	bool CreateTable(std::string tableNameAndParams);

	bool InsertData(std::string tableData, std::string newData);

	bool SelectData(std::string selectData, std::string tableName, std::string whereIs = "");

	bool UpdateData(std::string tableName, std::string newData, std::string whereIs);
	
	bool Delete(std::string tableName, std::string whereIs = "");

	struct sEnemyData
	{
		int id = 0;
		int health = 0;
		std::string name = "";
	};

	sEnemyData importData;

private:

	static int callback(void* NotUsed, int argc, char** argv, char** azColName);

	sqlite3* m_db;	
};

