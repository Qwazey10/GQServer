#pragma once
#include <vector>
#include <string>
#include <cstdint>

struct DBField
{
    std::string value;

    int32_t GetInt() const { return std::stoi(value); }
    int64_t GetInt64() const { return std::stoll(value); }
    float GetFloat() const { return std::stof(value); }
    const std::string& GetString() const { return value; }
};

struct DBRow
{
    std::vector<DBField> fields;

    const DBField& operator[](size_t i) const { return fields[i]; }
};

struct DBResult
{
    std::vector<DBRow> rows;

    bool Empty() const { return rows.empty(); }
};