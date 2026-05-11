#pragma once

#include <pqxx/pqxx>
#include <memory>
#include <string>

class DatabaseManager
{
public:
    // Instance Declaration
    static DatabaseManager& Instance() {
        static DatabaseManager instance;
        return instance;
    }

    void InitDB();
    void CloseDB();

    bool attemptConnection_AuthDB();
    bool attemptConnection_CharacterDB();
    bool attemptConnection_ContentDB();



    bool Heartbeat();

    bool CreateAccount(const std::string& Username,
        const std::string& PasswordHash);

    bool AccountExists(const std::string& Username);

private:
    std::unique_ptr<pqxx::connection> m_auth_gquest_Connection;
    std::unique_ptr<pqxx::connection> m_character_gquest_Connection;
    std::unique_ptr<pqxx::connection> m_content_gquest_Connection;
};