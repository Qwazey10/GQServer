//
// Created by michael on 4/6/26.
//

#ifndef GQUESTSERVER_PLAYER_H
#define GQUESTSERVER_PLAYER_H
#include <string>
#include <vector>


class Player {

public:
    int id_;
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