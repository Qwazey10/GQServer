// DatabaseManager.cpp
#include "DatabaseManager.h"
#include "DB/DBTypes.h"
#include <future>
#include <iostream>

DatabaseManager& DatabaseManager::Instance()
{
    static DatabaseManager instance;
    return instance;
}

void DatabaseManager::InitDB()
{
    std::cout << "[Database] Initializing pools...\n";

    m_authPool.Start(
        DatabaseClassification::AUTH_DATABASE,
        1,
        "192.168.1.13",
        "devgquest",
        "Apple1",
        "auth_gquest");

    m_characterPool.Start(
        DatabaseClassification::CHAR_DATABASE,
        1,
        "192.168.1.13",
        "devgquest",
        "Apple1",
        "character_gquest");

    m_contentPool.Start(
        DatabaseClassification::WRLD_DATABASE,
        1,
        "192.168.1.13",
        "devgquest",
        "Apple1",
        "content_gquest");

    std::cout << "[Database] All pools initialized!\n";
}

void DatabaseManager::ShutdownDB()
{
    m_authPool.Stop();
    m_characterPool.Stop();
    m_contentPool.Stop();
}

void DatabaseManager::ProcessCallbacks() {
    m_authPool.PumpCallbacks();
    m_characterPool.PumpCallbacks();
    m_contentPool.PumpCallbacks();
}


// ====================== Auth ======================

void DatabaseManager::AccountExists(const std::string& username)
{

}

void DatabaseManager::AccountExists_Callback(DBJob &job) {

}


void DatabaseManager::CreateAccount(const std::string& username)
{
    //Add an Account if needed
}

// ====================== Character ======================

void DatabaseManager::RetrieveCharacterInformation_GUID(uint32_t GUID) {
    DBJob job;

    job.stmt = Stmt::CHAR_GET_ALL_INVENTORY;
    job.GUID = GUID;
    job.Description = "Requesting Character Information by GUID";

    job.params.SetString(0, job.GUID);
    job.params.SetInt32(0,job.GUID);

    job.callback =
        [this](DBJob& job)
        {
            RetrieveCharacterInventory_Callback(job);
        };

    m_characterPool.Submit(std::move(job));

}

void DatabaseManager::RetrieveCharacterInformation_CharacterName(std::string &characterName) {
}

void DatabaseManager::RetrieveCharacterInformation_Callback(DBJob &JobResult) {
}

void DatabaseManager::RetrieveCharacterInventory(

    const std::string& characterName,uint32_t characterID)
{
    DBJob job;

    job.stmt = Stmt::CHAR_GET_ALL_INVENTORY;
    job.CharacterName = characterName;
    job.Description = "Requesting ALL Inventory Information for";

    job.params.SetString(0, characterName);

    job.callback =
        [this](DBJob& job)
        {
            RetrieveCharacterInventory_Callback(job);
        };

    m_characterPool.Submit(std::move(job));
}

void DatabaseManager::RetrieveCharacterInventory_Callback(DBJob& JobResult)
{
    if (JobResult.result.Empty())
    {
        std::cout << "Inventory empty\n";
        return;
    }

    for (auto& row : JobResult.result.rows)
    {
        int itemId = row[0].GetInt();
        int quantity = row[1].GetInt();

        std::cout
            << "Item "
            << itemId
            << " x "
            << quantity
            << std::endl;

        /*
        Player* player =
            WorldSessionMgr::Instance()
            .GetPlayer(...);

        if(player)
        {
            player->AddItem(itemId, quantity);
        }
        */
    }
}


