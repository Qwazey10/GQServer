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

struct Rotation {
    float x = 0.0f, y = 0.0f, z = 0.0f;
};

class Player {

public:

    Player(int id) : m_id(id) {}

    int GetId() const { return m_id; }
    const Position& GetPosition() const { return m_position; }
    void SetPosition(float x, float y, float z) {
        m_position.x = x; m_position.y = y; m_position.z = z;
    }

    const Rotation& GetRotation() const { return m_rotation; }
    void SetRotation(float pitch, float yaw, float roll) {
        m_rotation.x = pitch; m_rotation.y = yaw; m_rotation.z = roll;
    }

    int m_id = -1;
    int zoneId_ = 1;
    Position m_position;
    Rotation m_rotation;

    std::unordered_set<int> m_inRangePlayers;
    std::mutex m_inRangeMutex;

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
    int luk_;


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
};


#endif //GQUESTSERVER_PLAYER_H