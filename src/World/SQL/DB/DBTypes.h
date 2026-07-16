#pragma once

#include <vector>
#include <string>
#include <functional>
#include <mysql.h>
#include <cstdint>
#include <cstring>

struct DBField
{
    std::string value;

    // ====================== Integer Getters ======================
    [[nodiscard]] int8_t   GetInt8()   const { return SafeToInt8(); }
    [[nodiscard]] uint8_t  GetUInt8()  const { return SafeToUInt8(); }
    [[nodiscard]] int16_t  GetInt16()  const { return SafeToInt16(); }
    [[nodiscard]] uint16_t GetUInt16() const { return SafeToUInt16(); }
    [[nodiscard]] int32_t  GetInt32()  const { return SafeToInt32(); }
    [[nodiscard]] uint32_t GetUInt32() const { return SafeToUInt32(); }
    [[nodiscard]] int64_t  GetInt64()  const { return SafeToInt64(); }
    [[nodiscard]] uint64_t GetUInt64() const { return SafeToUInt64(); }

    // Legacy compatibility
    [[nodiscard]] int32_t  GetInt()    const { return GetInt32(); }
    [[nodiscard]] uint32_t GetUInt()   const { return GetUInt32(); }

    // ====================== Floating Point ======================
    [[nodiscard]] float    GetFloat()  const { return SafeToFloat(); }
    [[nodiscard]] double   GetDouble() const { return SafeToDouble(); }

    // ====================== Boolean ======================
    [[nodiscard]] bool GetBool() const
    {
        if (IsNull()) return false;
        if (value == "1" || value == "true" || value == "TRUE" || value == "yes" || value == "YES")
            return true;
        return false;
    }

    // ====================== String ======================
    [[nodiscard]] const std::string& GetString() const { return value; }
    [[nodiscard]] std::string GetStringCopy() const { return value; }

    // ====================== Safe with Default ======================
    template<typename T>
    [[nodiscard]] T GetOrDefault(T defaultValue) const
    {
        if (IsNull())
            return defaultValue;

        try
        {
            if constexpr (std::is_same_v<T, bool>)      return GetBool();
            else if constexpr (std::is_same_v<T, int8_t>)   return GetInt8();
            else if constexpr (std::is_same_v<T, uint8_t>)  return GetUInt8();
            else if constexpr (std::is_same_v<T, int16_t>)  return GetInt16();
            else if constexpr (std::is_same_v<T, uint16_t>) return GetUInt16();
            else if constexpr (std::is_same_v<T, int32_t>)  return GetInt32();
            else if constexpr (std::is_same_v<T, uint32_t>) return GetUInt32();
            else if constexpr (std::is_same_v<T, int64_t>)  return GetInt64();
            else if constexpr (std::is_same_v<T, uint64_t>) return GetUInt64();
            else if constexpr (std::is_same_v<T, float>)    return GetFloat();
            else if constexpr (std::is_same_v<T, double>)   return GetDouble();
        }
        catch (...) {}

        return defaultValue;
    }

    [[nodiscard]] bool IsNull() const
    {
        return value.empty();
    }

private:
    // Safe conversion helpers
    [[nodiscard]] int8_t   SafeToInt8()   const { if (IsNull()) return 0; try { return static_cast<int8_t>(std::stoi(value)); }   catch(...) { return 0; } }
    [[nodiscard]] uint8_t  SafeToUInt8()  const { if (IsNull()) return 0; try { return static_cast<uint8_t>(std::stoul(value)); }  catch(...) { return 0; } }
    [[nodiscard]] int16_t  SafeToInt16()  const { if (IsNull()) return 0; try { return static_cast<int16_t>(std::stoi(value)); }   catch(...) { return 0; } }
    [[nodiscard]] uint16_t SafeToUInt16() const { if (IsNull()) return 0; try { return static_cast<uint16_t>(std::stoul(value)); } catch(...) { return 0; } }
    [[nodiscard]] int32_t  SafeToInt32()  const { if (IsNull()) return 0; try { return std::stoi(value); }   catch(...) { return 0; } }
    [[nodiscard]] uint32_t SafeToUInt32() const { if (IsNull()) return 0; try { return std::stoul(value); }  catch(...) { return 0; } }
    [[nodiscard]] int64_t  SafeToInt64()  const { if (IsNull()) return 0; try { return std::stoll(value); }  catch(...) { return 0; } }
    [[nodiscard]] uint64_t SafeToUInt64() const { if (IsNull()) return 0; try { return std::stoull(value); } catch(...) { return 0; } }

    [[nodiscard]] float    SafeToFloat()  const { if (IsNull()) return 0.0f; try { return std::stof(value); }  catch(...) { return 0.0f; } }
    [[nodiscard]] double   SafeToDouble() const { if (IsNull()) return 0.0;  try { return std::stod(value); }  catch(...) { return 0.0; } }
};
/*struct DBField
{
    std::string value;

    // ====================== Integer Getters ======================
    [[nodiscard]] int8_t   GetInt8()   const { return static_cast<int8_t>(std::stoi(value)); }
    [[nodiscard]] uint8_t  GetUInt8()  const { return static_cast<uint8_t>(std::stoul(value)); }
    [[nodiscard]] int16_t  GetInt16()  const { return static_cast<int16_t>(std::stoi(value)); }
    [[nodiscard]] uint16_t GetUInt16() const { return static_cast<uint16_t>(std::stoul(value)); }
    [[nodiscard]] int32_t  GetInt32()  const { return std::stoi(value); }
    [[nodiscard]] uint32_t GetUInt32() const { return std::stoul(value); }
    [[nodiscard]] int64_t  GetInt64()  const { return std::stoll(value); }
    [[nodiscard]] uint64_t GetUInt64() const { return std::stoull(value); }

    // Legacy aliases (for backwards compatibility)
    [[nodiscard]] int32_t GetInt()     const { return GetInt32(); }
    [[nodiscard]] uint32_t GetUInt()   const { return GetUInt32(); }

    // ====================== Floating Point ======================
    [[nodiscard]] float    GetFloat()  const { return std::stof(value); }
    [[nodiscard]] double   GetDouble() const { return std::stod(value); }

    // ====================== Boolean ======================
    [[nodiscard]] bool GetBool() const
    {
        if (value.empty()) return false;

        // Support common database boolean representations
        if (value == "1" || value == "true" || value == "TRUE" || value == "yes" || value == "YES")
            return true;

        return false;
    }

    // ====================== String ======================
    [[nodiscard]] const std::string& GetString() const { return value; }

    [[nodiscard]] std::string GetStringCopy() const { return value; }

    // ====================== Safe Getters with Default ======================
    template<typename T>
    [[nodiscard]] T GetOrDefault(T defaultValue) const
    {
        try
        {
            if constexpr (std::is_same_v<T, bool>)
                return GetBool();
            else if constexpr (std::is_same_v<T, int8_t>)   return GetInt8();
            else if constexpr (std::is_same_v<T, uint8_t>)  return GetUInt8();
            else if constexpr (std::is_same_v<T, int16_t>)  return GetInt16();
            else if constexpr (std::is_same_v<T, uint16_t>) return GetUInt16();
            else if constexpr (std::is_same_v<T, int32_t>)  return GetInt32();
            else if constexpr (std::is_same_v<T, uint32_t>) return GetUInt32();
            else if constexpr (std::is_same_v<T, int64_t>)  return GetInt64();
            else if constexpr (std::is_same_v<T, uint64_t>) return GetUInt64();
            else if constexpr (std::is_same_v<T, float>)    return GetFloat();
            else if constexpr (std::is_same_v<T, double>)   return GetDouble();
            else
                return defaultValue;
        }
        catch (...)
        {
            return defaultValue;
        }
    }

    // Check if field is NULL / empty
    [[nodiscard]] bool IsNull() const
    {
        return value.empty();
    }
};*/

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

    // === String ===
    void SetString(uint32_t i, const std::string& v)
    {
        ensure(i);
        storage[i].assign(v.begin(), v.end());

        MYSQL_BIND& b = bind[i];
        memset(&b, 0, sizeof(MYSQL_BIND));

        b.buffer_type   = MYSQL_TYPE_STRING;
        b.buffer        = storage[i].data();
        b.buffer_length = static_cast<unsigned long>(storage[i].size());
        b.is_null       = nullptr;
    }

    // === Integer types ===
    void SetInt8(uint32_t i, int8_t v)   { SetInteger(i, MYSQL_TYPE_TINY, &v, sizeof(v)); }
    void SetUInt8(uint32_t i, uint8_t v) { SetInteger(i, MYSQL_TYPE_TINY, &v, sizeof(v)); }

    void SetInt16(uint32_t i, int16_t v) { SetInteger(i, MYSQL_TYPE_SHORT, &v, sizeof(v)); }
    void SetUInt16(uint32_t i, uint16_t v){ SetInteger(i, MYSQL_TYPE_SHORT, &v, sizeof(v)); }

    void SetInt32(uint32_t i, int32_t v) { SetInteger(i, MYSQL_TYPE_LONG, &v, sizeof(v)); }
    void SetUInt32(uint32_t i, uint32_t v){ SetInteger(i, MYSQL_TYPE_LONG, &v, sizeof(v)); }

    void SetInt64(uint32_t i, int64_t v) { SetInteger(i, MYSQL_TYPE_LONGLONG, &v, sizeof(v)); }
    void SetUInt64(uint32_t i, uint64_t v){ SetInteger(i, MYSQL_TYPE_LONGLONG, &v, sizeof(v)); }

    // === Float / Double ===
    void SetFloat(uint32_t i, float v)
    {
        ensure(i);
        storage[i].resize(sizeof(float));
        memcpy(storage[i].data(), &v, sizeof(float));

        MYSQL_BIND& b = bind[i];
        memset(&b, 0, sizeof(MYSQL_BIND));

        b.buffer_type = MYSQL_TYPE_FLOAT;
        b.buffer      = storage[i].data();
        b.is_null     = nullptr;
    }

    void SetDouble(uint32_t i, double v)
    {
        ensure(i);
        storage[i].resize(sizeof(double));
        memcpy(storage[i].data(), &v, sizeof(double));

        MYSQL_BIND& b = bind[i];
        memset(&b, 0, sizeof(MYSQL_BIND));

        b.buffer_type = MYSQL_TYPE_DOUBLE;
        b.buffer      = storage[i].data();
        b.is_null     = nullptr;
    }

    // === Boolean ===
    void SetBool(uint32_t i, bool v)
    {
        uint8_t val = v ? 1 : 0;
        SetInteger(i, MYSQL_TYPE_TINY, &val, sizeof(val));
    }

private:
    void SetInteger(uint32_t i, enum enum_field_types type, const void* value, size_t size)
    {
        ensure(i);
        storage[i].resize(size);
        memcpy(storage[i].data(), value, size);

        MYSQL_BIND& b = bind[i];
        memset(&b, 0, sizeof(MYSQL_BIND));

        b.buffer_type = type;
        b.buffer      = storage[i].data();
        b.is_null     = nullptr;
    }

public:
    MYSQL_BIND* Get() { return bind.data(); }
    [[nodiscard]] const MYSQL_BIND* Get() const { return bind.data(); }
    [[nodiscard]] size_t Size() const { return bind.size(); }

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
    // ======== ACCOUNT STATEMENTS ==========
    AUTH_SEL_ACCOUNT_EXISTS, // Statement to check if an account exists.
    AUTH_INS_ACCOUNT, //Statement to Insert a new account.
    AUTH_GET_ACCOUNT_USERNAME, //Statement to get an account via USERNAME.

    //========= CHARACTER STATEMENTS ========

    //Create/Delete a Character
    CHAR_INS_CHARACTER, // Statement to CREATE a New Character if none exist.
    CHAR_SEL_CHARACTER_EXISTS, // Statement to Check if a Character Exists.
    CHAR_DEL_CHARACTER_GUID_ACCOUNT, // Statement to DELETE a Character via GUID.

    //Save a character
    CHAR_SAV_CHARACTER_GUID, // Statement to SAVE the Character to the DB via GUID.
    CHAR_SAV_CHARACTER_CHARACTER_NAME, // Statement to SAVE the Character to the DB via Character Name.

    //Retrieve Character Information
    CHAR_GET_CHARACTER_GUID, // Statement to GET Character Information
    CHAR_GET_CHARACTER_NAME, //Statement to GET the character by character NAME.
    CHAR_GET_CHARACTER_ALL_ACCOUNT, // Statement to GET the character by ACCOUNT ID.



    //Get Character Inventory
    CHAR_GET_ALL_INVENTORY, // Statement to get the entirety of the Character Inventory
    CHAR_INS_INVENTORY, // Statement to add an Item to Character Inventory
    CHAR_DEL_INVENTORY, // Statement to remove an item from Character Inventory via GUID

    //======= WORLD STATEMENTS ============
    WORLD_GET_ALL_CREATURE, // Statement to Query the Creature Database and Construct all Creature Pointers
    WORLD_GET_CREATURE_ID,

    WORLD_GET_ALL_LOOTTABLES, // Statement to get all loottable information and create LootTable Pointers.

    MAX
};


#define MAX_CHAR_NAME_LEN 50
//Struct used to save the character
struct SaveCharacterStruct {
    // Database: guid (int 11)
    uint32_t guid;

    // Database: account_id (int 11)
    uint32_t account_id;

    // Database: character_name (varchar 50)
    // Use a fixed array, NOT a pointer (char*), so the data is inside the struct
    char character_name[MAX_CHAR_NAME_LEN];

    // Database: race_id, class_id, gender (tinyint 4)
    uint8_t race_id;
    uint8_t class_id;
    uint8_t gender;

    // Database: level (tinyint 4)
    uint8_t level;

    // Database: xp, money, zone_id (int 11)
    // Using 32-bit integers to match SQL 'int'
    uint32_t xp;
    uint32_t money;
    uint32_t zone_id;

    // Database: position_x, y, z, o (float)
    float position_x;
    float position_y;
    float position_z;
    float position_o;

    // Database: transport_x, y, z, o (float)
    float transport_x;
    float transport_y;
    float transport_z;
    float transport_o;

};

/*-- Dumping structure for table character_gquest.character
CREATE TABLE IF NOT EXISTS `character` (
  `guid` int(11) DEFAULT NULL COMMENT 'global unique ID',
  `account_id` int(11) DEFAULT NULL COMMENT 'Account ID',
  `character_name` varchar(50) DEFAULT NULL COMMENT 'Character Name',
  `race_id` tinyint(4) DEFAULT NULL COMMENT 'RaceID',
  `class_id` tinyint(4) DEFAULT NULL COMMENT 'ClassID',
  `gender` tinyint(4) DEFAULT NULL COMMENT 'GenderID 0=male 1=female',
  `level` tinyint(4) DEFAULT NULL,
  `xp` int(11) DEFAULT NULL COMMENT 'Experience Points',
  `money` int(11) DEFAULT NULL COMMENT 'Represented in Copper',
  `zone_id` int(11) DEFAULT NULL COMMENT 'zone ID of the character',
  `position_x` float DEFAULT NULL COMMENT 'the X position of the character',
  `position_y` float DEFAULT NULL COMMENT 'The Y Pos of the Character',
  `position_z` float DEFAULT NULL COMMENT 'the z position of the character',
  `position_o` float DEFAULT NULL COMMENT 'Rotation of the character, we only rotate on the yaw axis',
  `transport_x` float DEFAULT NULL COMMENT 'transport local position',
  `transport_y` float DEFAULT NULL COMMENT 'transport local y pos',
  `transport_z` float DEFAULT NULL COMMENT 'transport local z pos',
  `transport_o` float DEFAULT NULL COMMENT 'transport local rot'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci COMMENT='PrimaryCharacterDatabase\r\n';*/

struct CreateCharacterStruct {

    // Database: character_name (varchar 50)
    // Use a fixed array, NOT a pointer (char*), so the data is inside the struct
    std::string CharacterName;

    //Class Identification
    uint32_t prim_class_id;
    uint32_t second_class_id;
    uint32_t third_class_id;

    //Appearence Information
    uint32_t race_id;
    uint32_t gender;
    uint32_t hair_id;
    uint32_t face_id;
    uint32_t skin_id;

    // Database: level
    uint32_t level;

    // Database: xp, money, zone_id (int 11)
    // Using 32-bit integers to match SQL 'int'
    uint32_t xp;
    uint32_t money;
    uint32_t zone_id;

    // Database: position_x, y, z, o (float)
    float position_x;
    float position_y;
    float position_z;
    float position_o;

    // Database: transport_x, y, z, o (float)
    float transport_x;
    float transport_y;
    float transport_z;
    float transport_o;
};

struct DBJob
{
    //Set this to true to fire the callback. Not all Queries will require a callback i.e. Character Save
    bool bHasCallback = false;

    //Struct References
    CreateCharacterStruct CharCreateStruct;
    SaveCharacterStruct SaveStruct;
    //StatementReference
    Stmt stmt;

    //Prepared Statement
    PreparedStatement params;

    uint32_t SessionID;

    //Origination Timestamp - Server time for the creation of the job request.
    uint64_t OriginationTimestamp = 0;

    //Request Handled Timestamp - Time which the Query was completed.
    uint64_t RequestHandledTimestamp = 0;

    //Character ID - CharacterID for the origination of the Request
    uint64_t GUID = 0;

    //Account ID - AccountID for the origination of the Request
    uint32_t AccountID = 0;

    //Character Name - Character Name associated with Request
    std::string CharacterName;

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