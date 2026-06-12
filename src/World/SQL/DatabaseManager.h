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

    //Getters for the Database Pool Functions. Use these to reference the pools and forcequeue for non-async queries
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
    // Create Character :Statement CHAR_INS_CHARACTER
    void CreateCharacter(uint64_t GUID, uint32_t AccountID, CreateCharacterStruct CharCreateStruct);


    // Create the Corresponding Database Entries Required for a working Character -- This will expand later.
    // 1. Create the Character Inventory
    void CreateCharacterInventory(uint64_t GUID, const std::string& CharacterName);
    void CreateCharacterInventory_Callback(DBJob& JobResult);
    // 2. Create the character spellbook? talent sheet.
    // Add Functions here, when completed, call Finished Creating Character to send all the relevant information to client.
    //Function to call when the character
    void FinishCreatingCharacter(uint64_t GUID, const std::string& CharacterName);

    // Check if Character Exists via GUID or CharacterName, Statement: CHAR_SEL_CHARACTER_EXISTS,
    void CheckCharacterExists_GUID(uint64_t GUID);
    void CheckCharacterExists_CharacterName(const std::string& CharacterName);
    void CheckCharacterExists_Callback(DBJob& JobResult);

    // Delete a Character. Statement:CHAR_DEL_CHARACTER_GUID_ACCOUNT
    void DeleteCharacter(uint64_t GUID, uint32_t AccountID, const std::string& CharacterName);


    // Save a Character via GUID. Statement: CHAR_SAV_CHARACTER_GUID
    void SaveCharacterInformation_GUID(uint64_t GUID, SaveCharacterStruct& SaveStruct);
    // Save a Character via CharacterName. Statement:CHAR_SAV_CHARACTER_CHARACTER_NAME
    void SaveCharacterInformation_CharacterName(const std::string& CharacterName, SaveCharacterStruct& SaveStruct);

    //Retrieve character information via GUID. Statement: CHAR_GET_CHARACTER_GUID
    void RetrieveCharacterInformation_GUID(uint64_t GUID);
    //Retrieve character information via character name. Statement: CHAR_GET_CHARACTER_NAME
    void RetrieveCharacterInformation_CharacterName(std::string& characterName);
    void RetrieveCharacterInformation_Callback(DBJob& JobResult);

    //Get CharactCHAR_GET_CHARACTER_GUID
    void Retrieve_ALL_CharacterInformation_AccountID(uint64_t AccountID);
    void Retrieve_ALL_CharacterInformation_AccountID_Callback(DBJob& JobResult);
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
