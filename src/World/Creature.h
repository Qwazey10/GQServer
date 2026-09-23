#pragma once
#include "Player.h"   // reuse Position and basic types

class Creature {

        public:

        Creature() = default;
        ~Creature();

        uint64_t GUID = -1;           // unique monster GUID
        uint32_t entry{};        // template ID (for model, name, stats later)
        Vector3 position;
        uint32_t zoneId{};
        float health = 100.0f;
        float maxHealth = 100.0f;

        //appearence data
        int RaceID = 1;
        int ClassID = 1;
        int SubClassID = 1;
        int Gender = 1;
        int Level = 1;
        int Skin = 1;
        int Face = 1;
        int HairStyle = 1;
        int HairColor = 1;
        int FacialHair = 1;


        [[nodiscard]] Vector3 GetPosition() const { return position; }
        [[nodiscard]] uint64_t GetId() const { return GUID; }
        [[nodiscard]] uint32_t GetZoneId() const { return zoneId; }
        [[nodiscard]] uint32_t GetEntry() const { return entry; }

        void UpdateAI(float deltaTime);   // simple AI stub for now
};