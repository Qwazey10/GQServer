#pragma once
#include <string>

class Field
{
public:
    Field(const char* d, unsigned long len) : v(d, len) {}

    int32_t GetInt() const { return std::stoi(v); }
    int64_t GetInt64() const { return std::stoll(v); }
    float GetFloat() const { return std::stof(v); }
    const std::string& GetString() const { return v; }

private:
    std::string v;
};