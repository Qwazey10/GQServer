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
    //We will bind some of these functions to the AuthServer, but for now we have it on WorldServer
    void AccountExists(const std::string& username);
    void AccountExists_Callback(DBJob& job);
    void CreateAccount(const std::string& username);

    // ====================== Character DB Functions =================
    // Character Management Functions
    // Create Character
    void CreateCharacter(uint32_t GUID, const std::string& username);

    // Create the Corresponding Database Entries Required for a working Character -- This will expand later.
    // 1. Create the Character Inventory
    void CreateCharacterInventory(uint32_t GUID, const std::string& username);
    void CreateCharacterInventory_Callback(DBJob& JobResult);
    // 2. Create the character spellbook? talent sheet.
    // Add Functions here, when completed, call Finished Creating Character to send all the relevant information to client.
    //Function to call when the character
    void FinishCreatingCharacter(uint32_t GUID, const std::string& username);

    void RetrieveCharacterInformation_GUID(uint32_t GUID);
    void RetrieveCharacterInformation_CharacterName(std::string& characterName);
    void RetrieveCharacterInformation_Callback(DBJob& JobResult);

    void Retrieve_ALL_CharacterInformation_AccountID(uint32_t AccountID);

    void RetrieveCharacterInventory(const std::string& characterName, uint32_t characterId);
    void RetrieveCharacterInventory_Callback(DBJob& JobResult);




    // ====================== WorldFunctions =========================


    // Add more high-level functions here...

private:
    DatabaseManager() = default;

    DatabasePool m_authPool;
    DatabasePool m_characterPool;
    DatabasePool m_contentPool;
};
