#include "LootManager.h"
#include <iostream>
#include <utility>
#include "World/SQL/DatabaseManager.h"


LootManager& LootManager::Instance()
{
    static LootManager instance;
    return instance;
}

void LootManager::Initialize() {
    std::cout << "[LootManager] Loading static loot data...\n";

    auto& db = DatabaseManager::Instance().GetContentPool();
    QueryResult lootResult = db.ForceQuery_NoParams(Stmt::WORLD_GET_ALL_LOOTTABLES);

    if (lootResult.Empty())
    {
        std::cout << "[LootManager] No loot tables found.\n";
        return;
    }

    std::cout << "[Debug] Total rows returned: " << lootResult.rows.size() << "\n";

    for (size_t i = 0; i < lootResult.rows.size(); ++i)
    {
        auto& row = lootResult.rows[i];

        std::cout << "Row " << i << " raw values:\n";
        for (size_t c = 0; c < row.size() && c < 14; ++c)
        {
            std::cout << "  [" << c << "] = '" << row[c].value << "' (length: "
                      << row[c].value.length() << ")\n";
        }

        uint32_t tableID = row[0].GetUInt32();
        std::string desc = row[1].GetString();

        std::cout << "→ Parsed: TableID=" << tableID
                  << ", Description='" << desc << "'\n\n";
    }
}



void LootManager::AddLootTableStruct(uint32_t LootTableID, std::string LootTableDescription, uint32_t LootID_00,
    float LootDropRate_00, uint32_t mincount_00, uint32_t maxcount_00, uint32_t LootID_01, float LootDropRate_01,
    uint32_t mincount_01, uint32_t maxcount_01, uint32_t LootID_02, float LootDropRate_02, uint32_t mincount_02,
    uint32_t maxcount_02) {

    LootObject NewLootObject;
    NewLootObject.LootTableID = LootTableID;
    NewLootObject.LootTableDescription = std::move(LootTableDescription);

    NewLootObject.LootID_00 = LootID_00;
    NewLootObject.LootDropRate_00 = LootDropRate_00;
    NewLootObject.mincount_00 = mincount_00;
    NewLootObject.maxcount_00 = maxcount_00;

    NewLootObject.LootID_01 = LootID_01;
    NewLootObject.LootDropRate_01 = LootDropRate_01;
    NewLootObject.mincount_01 = mincount_01;
    NewLootObject.maxcount_01 = maxcount_01;

    NewLootObject.LootID_02= LootID_02;
    NewLootObject.LootDropRate_02 = LootDropRate_02;
    NewLootObject.mincount_02 = mincount_02;
    NewLootObject.maxcount_02 = maxcount_02;

    LootPoolObjects.push_back(std::move(NewLootObject));

    // Debug output
    std::cout << "[LootManager] Added Loot Table ID: " << NewLootObject.LootTableID
              << " | \"" << NewLootObject.LootTableDescription
              << "\" (" << LootPoolObjects.size() << " total)\n";
}


LootObject& LootManager::GetLootObjectByID(const uint32_t LootTableID) {

    //auto - LootStructReference in LootPoolObjects
    for (auto& loot : LootPoolObjects)
    {
        if (loot.LootTableID == LootTableID)
        {
            return loot;        // Return reference to the actual object in the vector
        }
    }

    // Not found - Handle gracefully
    static LootObject emptyLoot;           // Static so it lives forever
    emptyLoot = LootObject{};              // Reset to default values

    std::cout << "[LootManager] Warning: LootTableID " << LootTableID
              << " not found!\n";

    return emptyLoot;

}

void LootManager::ShutdownLootManager() {

}

void LootManager::StartLootRoll() {
}

void LootManager::EndLootRoll() {
}

