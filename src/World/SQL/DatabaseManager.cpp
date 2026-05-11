#include "DatabaseManager.h"
#include <memory>
#include <string>
#include <iostream>


void DatabaseManager::InitDB()
{
    attemptConnection_AuthDB();
    attemptConnection_CharacterDB();
    attemptConnection_ContentDB();
}



void DatabaseManager::CloseDB()
{
    if (m_auth_gquest_Connection && m_auth_gquest_Connection->is_open())
    {
        m_auth_gquest_Connection->close();
        std::cout << "[Database Manager] Connection to AUTH DB closed\n";
    }
    if (m_character_gquest_Connection && m_character_gquest_Connection->is_open())
    {
        m_character_gquest_Connection->close();
        std::cout << "[Database Manager] Connection to CHARACTER DB closed\n";
    }
    if (m_content_gquest_Connection && m_content_gquest_Connection->is_open())
    {
        m_content_gquest_Connection->close();
        std::cout << "[Database Manager] Connection to CONTENT DB closed\n";
    }
}

bool DatabaseManager::attemptConnection_AuthDB()
{

    const std::string AuthDBString =
        "host=192.168.1.13 "
        "port=5432 "
        "dbname=auth_gquest "
        "user=gquestdev "
        "password=Apple1";
    try
    {
        m_auth_gquest_Connection = std::make_unique<pqxx::connection>(AuthDBString);

        if (m_auth_gquest_Connection->is_open())
        {
            std::cout << "[DataBase Manager] -  Connection to AUTH DB successful!\n";
            return true;
        }

        return false;
    }
    catch (const std::exception& e)
    {
        std::cout << "[Database Manager] - AUTH DB connection failed: "
            << e.what() << std::endl;

        return false;
    }
}

bool DatabaseManager::attemptConnection_CharacterDB()
{
    const std::string CharacterDBString =
        "host=192.168.1.13 "
        "port=5432 "
        "dbname=character_gquest "
        "user=gquestdev "
        "password=Apple1";
    try
    {
        m_character_gquest_Connection = std::make_unique<pqxx::connection>(CharacterDBString);

        if (m_character_gquest_Connection->is_open())
        {
            std::cout << "[DataBase Manager] -  Connection to CHARACTER DB successful\n";
            return true;
        }

        return false;
    }
    catch (const std::exception& e)
    {
        std::cout << "[Database Manager] - CHARACTER DB connection failed: "
            << e.what() << std::endl;

        return false;
    }
}

bool DatabaseManager::attemptConnection_ContentDB()
{
    const std::string ContentDBString =
        "host=192.168.1.13 "
        "port=5432 "
        "dbname=content_gquest "
        "user=gquestdev "
        "password=Apple1";

    try
    {
        m_content_gquest_Connection = std::make_unique<pqxx::connection>(ContentDBString);

        if (m_content_gquest_Connection->is_open())
        {
            std::cout << "[DataBase Manager] -  Connection to CONTENT DB succsefful!\n";
            return true;
        }

        return false;
    }
    catch (const std::exception& e)
    {
        std::cout << "[Database Manager] - CONTENT DB connection failed: "
            << e.what() << std::endl;

        return false;
    }
}

/*bool DatabaseManager::Heartbeat()
{
    try
    {
        pqxx::work txn(*m_Connection);

        txn.exec("SELECT 1");

        txn.commit();

        return true;
    }
    catch (const std::exception& e)
    {
        std::cout << "Heartbeat failed: "
            << e.what() << std::endl;

        return false;
    }
}*/