#pragma once

#include <string>
#include <vector>

class cSQLiteImp;

class cSQLite
{
public:
	~cSQLite();
	static cSQLite* GetSQLite();

	bool GetEnemyData(int id, std::string& enemyName, int& enemyHealth);

	bool SetEnemyData(const std::string& enemyName, int enemyHealth);

private:
	cSQLite();

	bool m_InitEnemiesDB();
	bool m_CreateEnemyTable();

	bool m_DeleteEnemy(int id);
	bool m_DeleteTable();
	
	static cSQLite* m_pSQLite;
	cSQLiteImp* m_pSQLiteImp;
};

