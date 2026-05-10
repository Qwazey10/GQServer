//
// Created by michael on 4/6/26.
//

#ifndef GQUESTSERVER_PLAYER_H
#define GQUESTSERVER_PLAYER_H


#pragma once
#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_set>
#include <vector>
struct Position {
    float x = 0.0f, y = 0.0f, z = 0.0f;
};


class Player {

public:

    //Character Attributes
    int m_id = -1;
    int zoneId_ = 1;
    std::string characterName_;
    Position m_position;
    float CharacterYaw;


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

    int zoneID_;
    float WorldX_;
    float  WorldY_;
    float  WorldZ_;
    std::vector<int> inventory_;
    std::vector<int> equipment_;

    Player(int id) : m_id(id) {}
    const std::string GetName(){ return characterName_; }

    int GetId() const { return m_id; }
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

private:

    std::mutex m_inRangeMutex;
    std::unordered_set<int> inRangePlayers;

    std::mutex m_cache_inRangePlayers;
    std::unordered_set<int> cache_inRangePlayers;

};


#endif //GQUESTSERVER_PLAYER_H