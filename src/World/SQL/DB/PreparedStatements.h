#pragma once
#include <cstdint>

// Shared registry across pools

enum class PreparedStatement : uint32_t
{
    AUTH_SEL_ACCOUNT_EXISTS,
    AUTH_INS_ACCOUNT,

    CHAR_SEL_CHARACTER_INFO,

    MAX
};