//
// Created by mg on 9/19/26.
//

#ifndef GQUESTSERVER_COMBATMANAGER_H
#define GQUESTSERVER_COMBATMANAGER_H


class CombatManager {
public:
    CombatManager() = default;
    ~CombatManager() = default;
    // Delete copy
    static CombatManager& Instance();

    //Init the loot manager singleton
    void Initialize();
    void ShutdownCombatManager();

    void Update(float Diff);
};


#endif //GQUESTSERVER_COMBATMANAGER_H
