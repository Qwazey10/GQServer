#pragma once
#include <cstdint>

enum class Stmt : uint32_t
{
    AUTH_SEL_ACCOUNT_EXISTS,
    AUTH_INS_ACCOUNT,

    CHAR_SEL_CHARACTER,

    CONTENT_SEL_ALL_CREATURES,

    MAX
};