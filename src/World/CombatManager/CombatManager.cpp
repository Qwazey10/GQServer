

#include "CombatManager.h"

#include <iostream>

#include "World/WorldSessionMgr.h"


CombatManager& CombatManager::Instance()
{
    static CombatManager instance;
    return instance;
}

void CombatManager::Initialize() {

}

void CombatManager::ShutdownCombatManager() {

}
void CombatManager::Update(float Diff)
{
    auto sessions = WorldSessionMgr::Instance().GetSessions();

    for (auto& session : sessions)
    {
        auto player = session->GetPlayer();
        if (!player)
            continue;

        // Only process players who are actively auto-attacking
        if (!player->IsPlayerAttacking())
            continue;

        // --- Range Check ---
        constexpr float TempCombatRange = 400.0f;
        constexpr float TempCombatRangeSq = TempCombatRange * TempCombatRange;

        auto targetSession = WorldSessionMgr::Instance().GetSessionByPlayerGUID(player->TargetGUID);
        if (!targetSession)
            continue;

        auto target = targetSession->GetPlayer();
        if (!target)
            continue;

        const float dx = player->m_position.x - target->m_position.x;
        const float dy = player->m_position.y - target->m_position.y;
        const float dz = player->m_position.z - target->m_position.z;
        const float distanceSq = dx * dx + dy * dy + dz * dz;

        if (distanceSq > TempCombatRangeSq)
        {
            // Out of range – optionally stop attacking or just skip this frame
            // player->StopAttacking();
            continue;
        }

        // --- Swing Timer ---
        if (player->AttackTimer > 0.0f)
        {
            player->AttackTimer -= Diff;
        }

        // Ready to swing?
        if (player->AttackTimer <= 0.0f)
        {
            // Perform the attack here
            // Example:
            // DealDamage(player, target, CalculateDamage(player));
            // Send attack animation / combat log packet, etc.

            std::cout << "Player " << player->GUID
                      << " swung at " << target->GUID << std::endl;

            // Reset the timer for the next swing
            player->AttackTimer = player->AttackDelay;
        }
    }
}

