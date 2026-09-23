//
// Created by michael on 4/6/26.
//

#include "Player.h"


Vector3 Player::GetForwardVector() const
{
    // Convert degrees → radians
    const float YawRad = CharacterYaw * (3.1415926535f / 180.0f);

    // Flat forward vector (ignore pitch for most melee/ranged facing checks)
    return Vector3{
        std::cos(YawRad),   // X
        std::sin(YawRad),   // Y
        0.0f                // Z
    };
}
bool Player::IsFacing(const Vector3& TargetPos, float MaxAngleDegrees) const
{
    Vector3 ToTarget = TargetPos - m_position;
    ToTarget.z = 0.0f;                    // Flatten if you only care about horizontal facing

    float DistSq = ToTarget.x*ToTarget.x + ToTarget.y*ToTarget.y + ToTarget.z*ToTarget.z;
    if (DistSq < 0.0001f)
        return true;                      // Standing on top of each other → consider facing

    // Normalize
    float InvLen = 1.0f / std::sqrt(DistSq);
    ToTarget.x *= InvLen;
    ToTarget.y *= InvLen;
    ToTarget.z *= InvLen;

    Vector3 Forward = GetForwardVector();

    // Dot product
    float Dot = Forward.x * ToTarget.x + Forward.y * ToTarget.y + Forward.z * ToTarget.z;

    // Cosine of the maximum allowed angle
    float CosThreshold = std::cos(MaxAngleDegrees * (PI / 180.0f));

    return Dot >= CosThreshold;
}