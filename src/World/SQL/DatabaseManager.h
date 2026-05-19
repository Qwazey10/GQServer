#pragma once
#include "DB/DatabasePool.h"
#include "DB/QueryResults.h"
#include "DB/QueryResults.h"

class DatabaseManager
{
public:
    static DatabaseManager& Instance();

    void Init();
    void Shutdown();
    void Pump();

    std::future<QueryResult> Query(Stmt stmt, std::function<void(PreparedStatement&)> binder);
    void ExecuteTransaction(DBTransaction tx);

    void HealthCheck();

private:
    SafeQueue<std::function<void()>> dispatch;

    DatabasePool auth;
    DatabasePool chr;
    DatabasePool content;
};

/*#pragma once

#include "DB/DatabasePool.h"

class DatabaseManager
{
public:
    DatabaseManager() = default;
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;


    static DatabaseManager& Instance()
    {
        static DatabaseManager instance;
        return instance;
    }


    void InitDB();
    void ShutdownDB();

    // High-level API
    void CreateAccount(const std::string& username);
    void AccountExists(const std::string& username,
        std::function<void(bool)> callback);

    //Lets make a test function, Test Get Character Information 
    // The complete the function for Set Character information
    // For testing its in character_gquest and the table is Account, Name, Race, Class, Level, Experience
    // For this the variable types are account(TEXT), name(TEXT), race(INT), class(int), level(int), experience(int)


    DatabasePool m_authPool;
    DatabasePool m_characterPool;
    DatabasePool m_contentPool;
};*/


