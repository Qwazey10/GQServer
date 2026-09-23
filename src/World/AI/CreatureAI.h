//
// Created by michael on 4/12/26.
//

#ifndef GQUESTSERVER_CREATUREAI_H
#define GQUESTSERVER_CREATUREAI_H
#include "World/Player.h"
#include "World/World.h"

class WorldSessionMgr;

class CreatureAI {

public:

    void UpdateAI(float deltaTime);

    void GetPathTo(Vector3& pos);

    void MoveTo(Vector3& pos);




};

enum class CreatureAIState {
    IDLE,
    WANDERING,
    FLEEING,
    ATTACKING,
    CHASING,
    CASTING,
};


#endif //GQUESTSERVER_CREATUREAI_H