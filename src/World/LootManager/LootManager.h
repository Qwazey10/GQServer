//
// Created by Michael on 6/10/2026.
//

#ifndef GQUESTSERVER_LOOTMANAGER_H
#define GQUESTSERVER_LOOTMANAGER_H
#include <string>
#include <vector>


struct LootObject {
    uint32_t LootTableID;

    std::string LootTableDescription;

    uint32_t LootID_00;
    float LootDropRate_00;
    uint32_t mincount_00;
    uint32_t maxcount_00;

    uint32_t LootID_01;
    float LootDropRate_01;
    uint32_t mincount_01;
    uint32_t maxcount_01;

    uint32_t LootID_02;
    float LootDropRate_02;
    uint32_t mincount_02;
    uint32_t maxcount_02;

};
class LootManager {

public:
    LootManager() = default;
    ~LootManager() = default;
    // Delete copy
    static LootManager& Instance();

    //Init the loot manager singleton
    void Initialize();
    void ShutdownLootManager();

    void AddLootTableStruct(
        uint32_t LootTableID,
        std::string LootTableDescription,
        uint32_t LootID_00,
        float LootDropRate_00,
        uint32_t mincount_00,
        uint32_t maxcount_00,

        uint32_t LootID_01,
        float LootDropRate_01,
        uint32_t mincount_01,
        uint32_t maxcount_01,

        uint32_t LootID_02,
        float LootDropRate_02,
        uint32_t mincount_02,
        uint32_t maxcount_02);
        /*uint32_t LootID_03,
        float LootDropRate_03);*/

    void StartLootRoll();
    void EndLootRoll();

    LootObject& GetLootObjectByID(const uint32_t LootTableID);

private:
    //Request loot from the world/content database
    void RequestLootDataFromDatabase();

    std::vector<LootObject> LootPoolObjects;

};


#endif //GQUESTSERVER_LOOTMANAGER_H
