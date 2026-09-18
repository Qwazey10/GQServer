//
// Created by michael on 4/6/26.
//

#ifndef GQUESTSERVER_PLAYER_H
#define GQUESTSERVER_PLAYER_H


#pragma once
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct Position {
    float x = 0.0f, y = 0.0f, z = 0.0f;
};


class Player {
public:

    //Character Attributes
    int64_t GUID = -1;
    
    //Attack States
    // 0 - Not Auto Attacking.
    // 1 - Auto Attack is Active.

    int32_t AttackState;

    Position m_position;
    float CharacterYaw;

    std::string characterName_;


    int level_;
    int exp_;
    int hp_;
    int mp_;
    int gold_;

    int max_hp_;
    int max_mp_;

    int str_;
    int dex_;
    int int_;
    int vit_;
    int agi_;


    int classID_;
    int raceID_;
    int gender_;
    int title_;


    float WorldX_;
    float  WorldY_;
    float  WorldZ_;

    // Grid coordinates
    int32_t gridX_ = -1;
    int32_t gridY_ = -1;
    int32_t zoneID_ = -1;

    // Visibility caching
    std::vector<int32_t> visibleGrids_;

    // Store last tick and current tick to calculate delta (enter/leave)
    std::unordered_set<int32_t> previousVisiblePlayers_;
    std::unordered_set<int32_t> currentVisiblePlayers_;

    //
    std::vector<int> inventory_;
    std::vector<int> equipment_;

    const std::string GetName(){ return characterName_; }





    const Position& GetPosition() const { return m_position; }
    void SetPosition(float x, float y, float z) {
        m_position.x = x; m_position.y = y; m_position.z = z;
    }

    const float& GetRotation() const { return CharacterYaw; }
    void SetRotation(float yaw) {
        CharacterYaw = yaw;
    }

    void SetCacheInVisibilityRange(const std::unordered_set<int>& NewCacheSet)
    {
        std::lock_guard<std::mutex> lock(m_cache_inRangePlayers);
        cache_inRangePlayers = NewCacheSet;
    }

    void SetInVisibilityRange(const std::unordered_set<int>& NewVisSet)
    {
        std::lock_guard<std::mutex> lock(m_inRangeMutex);
        inRangePlayers = NewVisSet;
    }

    std::unordered_set<int> GetCacheInVisibilityRange()
    {
        std::lock_guard<std::mutex> lock(m_cache_inRangePlayers);
        return cache_inRangePlayers;
    }

    std::unordered_set<int> GetInVisibilityRange()
    {
        std::lock_guard<std::mutex> lock(m_inRangeMutex);
        return inRangePlayers;
    }

    int64_t GetGUID() const {
        return GUID;
    }

private:

    std::mutex m_inRangeMutex;
    std::unordered_set<int> inRangePlayers;

    std::mutex m_cache_inRangePlayers;
    std::unordered_set<int> cache_inRangePlayers;

};


#endif //GQUESTSERVER_PLAYER_H