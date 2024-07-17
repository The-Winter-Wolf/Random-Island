#include "cSQLiteImp.h"

#include "../../system/window/cConsoleLogs.h"
extern cConsoleLogs* g_pConsole;

#include <sqlite/sqlite3.h>
#include <iostream>
#include <string>


static cSQLiteImp::sEnemyData m_staticData;

int cSQLiteImp::callback(void* NotUsed, int argc, char** argv, char** azColName) {
    for (int i = 0; i < argc; i++) 
    {
        std::cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << std::endl;
    }
    std::cout << std::endl;

    std::string tempStr = argv[0];
    m_staticData.id = std::stoi(tempStr);

    m_staticData.name = argv[1];

    tempStr = argv[2];
    m_staticData.health = std::stoi(tempStr);

    return 0;
}

cSQLiteImp::cSQLiteImp()
{
    this->m_db = nullptr;
}

cSQLiteImp::~cSQLiteImp()
{
    sqlite3_close(m_db);
}

bool cSQLiteImp::InitDatabase(std::string dbName)
{
    int result = 0;
    char* errorMsg = 0;
    const char* query = 0;

    // Open database
    result = sqlite3_open(dbName.c_str(), &m_db);
    if (result)
    {
        std::string tempError = sqlite3_errmsg(m_db);
        g_pConsole->cError("Can't open database: " + tempError);
        return false;
    }
    else 
    {
        g_pConsole->cSuccess("Database " + dbName + " opened successfully");
        return true;
    }
}

bool cSQLiteImp::CreateTable(std::string tableNameAndParams)
{
    int result = 0;
    char* errorMsg = 0;
    const char* query = 0;

    // Create table
    std::string tempStr = "CREATE TABLE IF NOT EXISTS " + tableNameAndParams + ";";
    query = tempStr.c_str();

    // Execute SQL statement
    result = sqlite3_exec(m_db, query, callback, 0, &errorMsg);
    if (result != SQLITE_OK) 
    {
        std::string tempError = errorMsg;
        g_pConsole->cError("SQL error: " + tempError);
        sqlite3_free(errorMsg);
        return false;
    }
    else 
    {
        g_pConsole->cSuccess("Table " + tableNameAndParams + " created successfully");
        return true;
    }
}

bool cSQLiteImp::InsertData(std::string tableData, std::string newValues)
{
    int result = 0;
    char* errorMsg = 0;
    const char* query = 0;

    // Insert data
    std::string tempStr = "INSERT OR REPLACE INTO " + tableData + " VALUES " + newValues + ";";
    query = tempStr.c_str();

    // Execute SQL statement
    result = sqlite3_exec(m_db, query, callback, 0, &errorMsg);
    if (result != SQLITE_OK)
    {
        std::string tempError = errorMsg;
        g_pConsole->cError("SQL error: " + tempError);
        sqlite3_free(errorMsg);
        return false;
    }
    else 
    {
        g_pConsole->cSuccess("Data " + newValues + " inserted in " + tableData + " successfully");
        return true;
    }
}

bool cSQLiteImp::SelectData(std::string selectData, std::string tableName, std::string whereIs)
{
    int result = 0;
    char* errorMsg = 0;
    const char* query = 0;

    m_staticData.id = -1;
    m_staticData.name = "";
    m_staticData.health = -1;

    // Select and display data
    std::string tempStr = "";
    if (whereIs == "")
    {
        tempStr = "SELECT " + selectData + " FROM " + tableName + ";";       
    }
    else
    {
        tempStr = "SELECT " + selectData + " FROM " + tableName + " WHERE " + whereIs + ";";
    }
    query = tempStr.c_str();

    // Execute SQL statement
    result = sqlite3_exec(m_db, query, callback, 0, &errorMsg);
    if (result != SQLITE_OK) 
    {
        std::string tempError = errorMsg;
        g_pConsole->cError("SQL error: " + tempError);
        sqlite3_free(errorMsg);
        return false;
    }
    else 
    {
        importData = m_staticData;

        if (m_staticData.id == -1)
        {
            g_pConsole->cMessage("Data selected from table " + tableName + " where is " + whereIs + " not found");
            return false;
        }

        if (selectData == "*")
        {
            tempStr = "All data";
        }
        else
        {
            tempStr = "Data " + selectData;
        }

        if (whereIs == "")
        {
            g_pConsole->cSuccess("Data " + selectData + " selected from table " + tableName + " successfully");
        }
        else
        {
            g_pConsole->cSuccess(tempStr + " selected from table " + tableName + " where is " + whereIs + " successfully");
        }
        return true;
    }
}

bool cSQLiteImp::UpdateData(std::string tableName, std::string newData, std::string whereIs)
{
    int result = 0;
    char* errorMsg = 0;
    const char* query = 0;

    // Update data
    std::string tempStr = "UPDATE " + tableName + " SET " + newData + " WHERE " + whereIs + ";";
    query = tempStr.c_str();

    // Execute SQL statement
    result = sqlite3_exec(m_db, query, callback, 0, &errorMsg);
    if (result != SQLITE_OK)
    {
        std::string tempError = errorMsg;
        g_pConsole->cError("SQL error: " + tempError);
        sqlite3_free(errorMsg);
        return false;
    }
    else
    {
        g_pConsole->cSuccess("Data where " + whereIs + " in table " + tableName + " updated to " + newData + " successfully");
        return true;
    }
}

bool cSQLiteImp::Delete(std::string tableName, std::string whereIs)
{
    int result = 0;
    char* errorMsg = 0;
    const char* query = 0;

    // Delete data
    std::string tempStr = "";

    if (whereIs == "")
    {
        tempStr = "DELETE " + tableName + ";"; // Delete all table
    }
    else
    {
        tempStr = "DELETE " + tableName + " WHERE " + whereIs + ";"; // Delete where data
    }
    query = tempStr.c_str();

    // Execute SQL statement
    result = sqlite3_exec(m_db, query, callback, 0, &errorMsg);
    if (result != SQLITE_OK)
    {
        std::string tempError = errorMsg;
        g_pConsole->cError("SQL error: " + tempError);
        sqlite3_free(errorMsg);
        return false;
    }
    else
    {
        if (whereIs == "")
        {
            g_pConsole->cSuccess("Table " + tableName + " deleted successfully");
        }
        else
        {
            g_pConsole->cSuccess("Data " + whereIs + " from table " + tableName + " deleted successfully");
        }
        return true;
    }
}


