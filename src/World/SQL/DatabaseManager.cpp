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
        "127.0.0.1",
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
void DatabaseManager::CreateCharacter(uint64_t GUID, uint32_t AccountID, CreateCharacterStruct CharCreateStruct) {
    DBJob job;
    job.stmt = Stmt::CHAR_INS_CHARACTER;
    job.GUID = GUID;
    job.AccountID = AccountID;
    job.Description = "Creating character " + CharCreateStruct.CharacterName; // Fixed string concat space
    job.bHasCallback = false;

    // --- Parameter Binding (0-based index) ---
    // SQL Order: guid, account_id, character_name, race_id, class_id_primary, class_id_second, class_id_third,
    //            gender, hair_id, face_id, skin_id, level, xp, money, zone_id, pos(x,y,z,o), trans(x,y,z,o)

    // 0: guid
    job.params.SetUInt64(0, GUID);
    // 1: account_id
    job.params.SetUInt32(1, job.AccountID);
    // 2: character_name (FIXED: Use CharCreateStruct, not job)
    job.params.SetString(2, CharCreateStruct.CharacterName);

    // 3: race_id
    job.params.SetUInt32(3, CharCreateStruct.race_id);
    // 4: class_id_primary
    job.params.SetUInt32(4, CharCreateStruct.prim_class_id);
    // 5: class_id_second
    job.params.SetUInt32(5, CharCreateStruct.second_class_id);
    // 6: class_id_third
    job.params.SetUInt32(6, CharCreateStruct.third_class_id);

    // 7: gender
    job.params.SetUInt32(7, CharCreateStruct.gender);
    // 8: hair_id
    job.params.SetUInt32(8, CharCreateStruct.hair_id);
    // 9: face_id
    job.params.SetUInt32(9, CharCreateStruct.face_id);
    // 10: skin_id
    job.params.SetUInt32(10, CharCreateStruct.skin_id);

    // 11: level
    job.params.SetUInt32(11, CharCreateStruct.level);
    // 12: xp
    job.params.SetUInt32(12, CharCreateStruct.xp);
    // 13: money
    job.params.SetUInt32(13, CharCreateStruct.money);

    // 14: zone_id
    job.params.SetUInt32(14, CharCreateStruct.zone_id);

    // Position
    job.params.SetFloat(15, CharCreateStruct.position_x);
    job.params.SetFloat(16, CharCreateStruct.position_y);
    job.params.SetFloat(17, CharCreateStruct.position_z);
    job.params.SetFloat(18, CharCreateStruct.position_o);

    // Transport
    job.params.SetFloat(19, CharCreateStruct.transport_x);
    job.params.SetFloat(20, CharCreateStruct.transport_y);
    job.params.SetFloat(21, CharCreateStruct.transport_z);
    job.params.SetFloat(22, CharCreateStruct.transport_o);

    // REMOVED: job.params.SetUInt32(23, ...)
    // Index 23 does not exist if you only have 23 placeholders (0-22).

    // Add the job to the job pool
    m_characterPool.Submit(std::move(job));

    // Call Any Additional Creation functions here
    CreateCharacterInventory(GUID, CharCreateStruct.CharacterName);
}

void DatabaseManager::CreateCharacterInventory(uint64_t GUID, const std::string &CharacterName) {
    std::cout << "Creating Inventory for Character -- Character Name: " << CharacterName << std::endl;
}



void DatabaseManager::CreateCharacterInventory_Callback(DBJob &JobResult) {

    //FinishCreatingCharacter(123)
}

void DatabaseManager::FinishCreatingCharacter(uint64_t GUID, const std::string &CharacterName) {
}

void DatabaseManager::CheckCharacterExists_GUID(uint64_t GUID) {
}


void DatabaseManager::CheckCharacterExists_CharacterName(const std::string &CharacterName) {
}

void DatabaseManager::CheckCharacterExists_Callback(DBJob &JobResult) {
}




void DatabaseManager::RetrieveCharacterInformation_GUID(uint64_t GUID) {
    DBJob job;

    job.stmt = Stmt::CHAR_GET_ALL_INVENTORY;
    job.GUID = GUID;
    job.Description = "Requesting Character Information by GUID";

    job.params.SetInt64(0,job.GUID);

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
    if (JobResult.result.Empty())
    {
        std::cout << "Inventory empty\n";
        return;
    }

    for (auto& row : JobResult.result.rows)
    {
        int itemId = row[0].GetInt();
        int quantity = row[1].GetInt();

    }
}

void DatabaseManager::RetrieveCharacterInventory(const std::string& characterName,uint32_t characterID){

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


    void DatabaseManager::RetrieveCharacterInventory_Callback(DBJob& JobResult) {
    if (JobResult.result.Empty())
    {
        std::cout << "Inventory empty\n";
        return;
    }

    for (auto& row : JobResult.result.rows) {
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