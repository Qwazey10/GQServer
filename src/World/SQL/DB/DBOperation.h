#pragma once
#include <mysql.h>
#include <vector>
#include <string>

class PreparedStatement
{
public:
    void SetString(uint32_t i, const std::string& v)
    {
        ensure(i);
        storage[i] = std::vector<uint8_t>(v.begin(), v.end());
        storage[i].push_back(0);

        bind[i].buffer_type = MYSQL_TYPE_STRING;
        bind[i].buffer = storage[i].data();
        bind[i].buffer_length = storage[i].size();
    }

    void SetInt32(uint32_t i, int32_t v)
    {
        ensure(i);
        storage[i].resize(sizeof(v));
        memcpy(storage[i].data(), &v, sizeof(v));

        bind[i].buffer_type = MYSQL_TYPE_LONG;
        bind[i].buffer = storage[i].data();
    }

    MYSQL_BIND* Get() { return bind.data(); }
    size_t Size() const { return bind.size(); }

private:
    void ensure(uint32_t i)
    {
        if (bind.size() <= i)
        {
            bind.resize(i + 1);
            storage.resize(i + 1);
        }
    }

    std::vector<MYSQL_BIND> bind;
    std::vector<std::vector<uint8_t>> storage;
};