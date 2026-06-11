// DatabaseManager.h
#pragma once
#include <future>
#include "DB/DatabasePool.h"


class DatabaseManager
{
public:
    static DatabaseManager& Instance();


    void InitDB();
    void ShutdownDB();
    void ProcessCallbacks(); // Call every frame/tick in main loop

    // Optional: Const versions
    DatabasePool& GetAuthPool()             { return m_authPool; }
    DatabasePool& GetCharacterPool()        { return m_characterPool; }
    DatabasePool& GetContentPool()          { return m_contentPool; }


    // ====================== Auth DB Functions ======================
    void AccountExists(const std::string& username);
    void AccountExists_Callback(DBJob& job);
    void CreateAccount(const std::string& username);

    // ====================== Character DB Functions =================
    //Retreive the Entiery of the Characters Inventory
    void RetrieveCharacterInventory(const std::string& characterName, uint32_t characterId);
    void RetrieveCharacterInventory_Callback(DBJob& JobResult);


    void CreateCharacter();
    void CreateCharacterInventory(uint32_t GUID, const std::string& username);
    void CreatecharacterInventory_Callback(DBJob& JobResult);
    void RetrieveCharacterInformation(const std::string& characterName, uint32_t characterId);
    void RetrieveCharacterInformation_Callback(DBJob& JobResult);




    // ====================== WorldFunctions =========================


    // Add more high-level functions here...

private:
    DatabaseManager() = default;

    DatabasePool m_authPool;
    DatabasePool m_characterPool;
    DatabasePool m_contentPool;
};
