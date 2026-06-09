#pragma once

#include <vector>
#include <string>
#include <functional>
#include <mysql.h>
#include <cstdint>

struct DBField
{
    std::string value;

    [[nodiscard]] int32_t GetInt() const
    {
        return std::stoi(value);
    }

    [[nodiscard]] int64_t GetInt64() const
    {
        return std::stoll(value);
    }

    [[nodiscard]] float GetFloat() const
    {
        return std::stof(value);
    }

    [[nodiscard]] const std::string& GetString() const
    {
        return value;
    }
};

struct QueryResult
{
    std::vector<std::vector<DBField>> rows;
    size_t index = 0;

    void AddRow(std::vector<DBField>&& row)
    {
        rows.emplace_back(std::move(row));
    }

    bool NextRow()
    {
        return ++index < rows.size();
    }

    [[nodiscard]] bool Empty() const
    {
        return rows.empty();
    }

    [[nodiscard]] size_t RowCount() const
    {
        return rows.size();
    }

    const DBField& operator[](size_t i) const
    {
        return rows[index][i];
    }
};

struct PreparedStatement
{
    void ensure(uint32_t i)
    {
        if (bind.size() <= i)
        {
            bind.resize(i + 1);
            storage.resize(i + 1);
        }
    }

    void SetString(uint32_t i, const std::string& v)
    {
        ensure(i);

        storage[i].assign(v.begin(), v.end());

        MYSQL_BIND& b = bind[i];
        memset(&b, 0, sizeof(MYSQL_BIND));

        b.buffer_type = MYSQL_TYPE_STRING;
        b.buffer = storage[i].data();
        b.buffer_length =
            static_cast<unsigned long>(storage[i].size());
    }

    void SetInt32(uint32_t i, int32_t v)
    {
        ensure(i);

        storage[i].resize(sizeof(int32_t));
        memcpy(storage[i].data(), &v, sizeof(int32_t));

        MYSQL_BIND& b = bind[i];
        memset(&b, 0, sizeof(MYSQL_BIND));

        b.buffer_type = MYSQL_TYPE_LONG;
        b.buffer = storage[i].data();
    }

    void SetInt64(uint32_t i, int64_t v)
    {
        ensure(i);

        storage[i].resize(sizeof(int64_t));
        memcpy(storage[i].data(), &v, sizeof(int64_t));

        MYSQL_BIND& b = bind[i];
        memset(&b, 0, sizeof(MYSQL_BIND));

        b.buffer_type = MYSQL_TYPE_LONGLONG;
        b.buffer = storage[i].data();
    }

    MYSQL_BIND* Get()
    {
        return bind.data();
    }

    [[nodiscard]] const MYSQL_BIND* Get() const
    {
        return bind.data();
    }

    [[nodiscard]] size_t Size() const
    {
        return bind.size();
    }

    std::vector<MYSQL_BIND> bind;
    std::vector<std::vector<uint8_t>> storage;
};


enum class DatabaseClassification : uint32_t {
    AUTH_DATABASE,
    CHAR_DATABASE,
    WRLD_DATABASE,
};

enum class Stmt : uint32_t
{
    AUTH_SEL_ACCOUNT_EXISTS,
    AUTH_INS_ACCOUNT, //Statement

    CHAR_SEL_CHARACTER_EXISTS, //Statement to Check if a Character Exists
    CHAR_INS_CHARACTER, // Statement to CREATE a New Character if none exist
    CHAR_DEL_CHARACTER, // Statement to DELETE a Character if
    CHAR_SAV_CHARACTER, // Statement to SAVE the character to the database

    CHAR_GET_ALL_INVENTORY, // Statement to get the entirety of the Character Inventory
    CHAR_INS_INVENTORY, // Statement to add an Item to Character Inventory
    CHAR_DEL_INVENTORY, // Statement to remove an item from Character Inventory

    WORLD_GET_ALL_CREATURE, // Statement to Query the Creature Database and Construct all Creature Pointers
    WORLD_GET_CREATURE_ID,

    MAX
};

struct DBJob
{
    //Set this to true to fire the callback. Not all Queries will require a callback i.e. Character Save
    bool bHasCallback = false;

    //StatementReference
    Stmt stmt;

    //Prepared Statement
    PreparedStatement params;

    //Origination Timestamp - Server time for the creation of the job request.
    uint64_t OriginationTimestamp = 0;

    //Request Handled Timestamp - Time which the Query was completed.
    uint64_t RequestHandledTimestamp = 0;

    //Character ID - CharacterID for the origination of the Request
    uint32_t CharacterID = 0;

    //Account ID - AccountID for the origination of the Request
    uint32_t AccountID = 0;

    //ZoneID - Which ZoneID was the database job created from
    uint32_t ZoneID = 0;

    //Description of Job, used for random jobs or for debug descriptions or parse string filters
    std::string Description;

    //Storage of QueryResult
    QueryResult result;

    //Callback reference
    std::function<void(DBJob&)> callback;

    //Constructor
    DBJob() = default;

    DBJob(DBJob&&) noexcept = default;
    DBJob& operator=(DBJob&&) noexcept = default;

    DBJob(const DBJob&) = delete;
    DBJob& operator=(const DBJob&) = delete;
};