//
// Created by michael on 4/11/26.
//

#include "Creature.h"

Creature::Creature(uint64_t id, uint32_t entry, float x, float y, float z, uint32_t zone) {
    : id(id), entry(entry), position{x, y, z}, zoneId(zone) {}
}


void Creature::UpdateAI(float deltaTime) {
    // Very basic AI: stand still for now
    // Later: move toward nearest player, attack, etc.
}