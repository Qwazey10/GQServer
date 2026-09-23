

#include "CombatManager.h"

#include <iostream>

#include "World/WorldSessionMgr.h"
#include "World/Opcodes/Opcodes.h"


CombatManager& CombatManager::Instance()
{
    static CombatManager instance;
    return instance;
}

void CombatManager::Initialize() {

}

void CombatManager::ShutdownCombatManager() {

}
/*void CombatManager::Update(float Diff)
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

            //Test Function
            std::cout << "Player " << player->GUID
                      << " swung at " << target->GUID << std::endl;

            //Create Packet to send to the player
            WorldPacket PlayerPacket(SMSG_PLAYER_AUTOATTACK_DAMAGE);

            //Damage Calculators will go below for
            int32_t TotalDmg = 0;

            TotalDmg = player->DmgMax;

            PlayerPacket << TotalDmg << target->TargetGUID << target->characterName_;

            session->SendPacket(PlayerPacket);


            if (targetSession) {
                WorldPacket TargetPacket(SMSG_PLAYER_ENTITY_ATTACK_DAMAGE);

                TargetPacket << TotalDmg << player->GUID << player->characterName_;
                targetSession->SendPacket(TargetPacket);
            }




            // Reset the timer for the next swing
            player->AttackTimer = player->AttackDelay;
        }
    }
}*/

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

        // --- Get Target ---
        auto targetSession = WorldSessionMgr::Instance().GetSessionByPlayerGUID(player->TargetGUID);
        if (!targetSession)
            continue;

        auto target = targetSession->GetPlayer();
        if (!target)
            continue;

        // --- Range Check ---
        constexpr float TempCombatRange = 400.0f;
        constexpr float TempCombatRangeSq = TempCombatRange * TempCombatRange;

        const float dx = player->m_position.x - target->m_position.x;
        const float dy = player->m_position.y - target->m_position.y;
        const float dz = player->m_position.z - target->m_position.z;
        const float distanceSq = dx * dx + dy * dy + dz * dz;

        if (distanceSq > TempCombatRangeSq)
        {
            // Out of range
            // player->StopAttacking(); // optional
            continue;
        }

        // --- Facing Check ---
        // 70 degrees is a good default (quite forgiving). Change to 90.0f for a half-circle, 45.0f for stricter.
        if (!player->IsFacing(target->m_position, 70.0f))
        {

            // Not facing the target → skip this swing
            WorldPacket pkt (SMSG_DEBUG_MESSAGE);

            std::string missString = "You are not facing your target";

            pkt << missString;

            session->SendPacket(pkt);

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
            // Test output
            std::cout << "Player " << player->GUID
                      << " swung at " << target->GUID << std::endl;

            // ----- Damage -----
            float s = player->str_ / 1024.0f;           // 0.0 → 1.0
            float r = (float)rand() / RAND_MAX;     // 0.0 → 1.0
            float skew = 1.0f / (1.0f + s * 4.0f);  // exponent: 1.0 at s=0, 0.2 at s=1
            int result = player->dmgMin + (int)((player->dmgMax - player->dmgMin) * powf(r, skew));
            int32_t TotalDmg = result;   // Replace with real damage calculation later

            // Packet to the attacker (you hit someone)
            WorldPacket PlayerPacket(SMSG_PLAYER_AUTOATTACK_DAMAGE);
            PlayerPacket << TotalDmg << target->GUID << target->characterName_;
            session->SendPacket(PlayerPacket);

            // Packet to the target (someone hit you)
            WorldPacket TargetPacket(SMSG_PLAYER_ENTITY_ATTACK_DAMAGE);
            TargetPacket << TotalDmg << player->GUID << player->characterName_;
            targetSession->SendPacket(TargetPacket);

            // Reset timer for next swing
            player->AttackTimer = player->AttackDelay;
        }
    }
}

