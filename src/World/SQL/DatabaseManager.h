#pragma once

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


    DatabasePool m_authPool;
    DatabasePool m_characterPool;
    DatabasePool m_contentPool;
};


