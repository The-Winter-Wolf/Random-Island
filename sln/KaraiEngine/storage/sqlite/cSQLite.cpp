#include "cSQLite.h"
#include "cSQLiteImp.h"

cSQLite* cSQLite::m_pSQLite = NULL;

cSQLite* cSQLite::GetSQLite()
{
    if (cSQLite::m_pSQLite == NULL)
    {
        cSQLite::m_pSQLite = new cSQLite();
    }
    return cSQLite::m_pSQLite;
}

cSQLite::cSQLite()
{
    this->m_pSQLiteImp = new cSQLiteImp();
    this->m_InitEnemiesDB();
    this->m_CreateEnemyTable();
}
cSQLite::~cSQLite()
{
    delete this->m_pSQLiteImp;
}

bool cSQLite::SetEnemyData(const std::string& enemyName, int enemyHealth)
{
    std::string tempStr1 = "name = '" + enemyName + "'";
    if (this->m_pSQLiteImp->SelectData("*", "Enemies", tempStr1))
    {
        int id = this->m_pSQLiteImp->importData.id;

        tempStr1 = "health = '" + std::to_string(enemyHealth) + "'";
        std::string tempStr2 = "id = " + std::to_string(id);

        this->m_pSQLiteImp->UpdateData("Enemies", tempStr1, tempStr2);
        return true;
    }

    tempStr1 = "('" + enemyName + "', " + std::to_string(enemyHealth) + ")";
    return this->m_pSQLiteImp->InsertData("Enemies(name, health)", tempStr1);
}

bool cSQLite::GetEnemyData(int id, std::string& enemyName, int& enemyHealth)
{
    std::string tempStr = "id = " + std::to_string(id);
    if (this->m_pSQLiteImp->SelectData("*", "Enemies", tempStr))
    {
        enemyName = this->m_pSQLiteImp->importData.name;
        enemyHealth = this->m_pSQLiteImp->importData.health;
        return true;
    }

    enemyName = "";
    enemyHealth = -1;
    return false;
}

bool cSQLite::m_InitEnemiesDB()
{
    return m_pSQLiteImp->InitDatabase("../../extern/assets/configs/sqlite/enemies.db");
}

bool cSQLite::m_CreateEnemyTable()
{
    std::string query = "Enemies(id INTEGER PRIMARY KEY AUTOINCREMENT, " \
                        "name TEXT NOT NULL, health INT NOT NULL)";

    return m_pSQLiteImp->CreateTable(query);
}

bool cSQLite::m_DeleteEnemy(int id)
{
    std::string tempStr = "id = " + std::to_string(id);

    return this->m_pSQLiteImp->Delete("Enemies", tempStr);
}

bool cSQLite::m_DeleteTable()
{
    return this->m_pSQLiteImp->Delete("Enemies");
}