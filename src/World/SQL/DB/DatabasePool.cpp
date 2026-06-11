#include "DatabasePool.h"
#include <iostream>

#include "World/TimeManager/TimeManager.h"

void DatabasePool::Start(
    DatabaseClassification type,
    int numThreads,
    const std::string& host,
    const std::string& user,
    const std::string& pass,
    const std::string& db)
{
    m_host = host;
    m_user = user;
    m_pass = pass;
    m_db = db;

    if (!Connect())
        return;

    switch (type)
    {
        case DatabaseClassification::AUTH_DATABASE:
            AUTH_PrepareStatements();
            break;
        case DatabaseClassification::CHAR_DATABASE:
            CHAR_PrepareStatements();
            break;
        case DatabaseClassification::WRLD_DATABASE:
            WRLD_PrepareStatements();
            break;
        default:
            // Log error: "Unknown database classification"
            return;
    }

    Run();
}

bool DatabasePool::Connect()
{
    std::cout << mysql_get_client_info() << std::endl; // Prints 3.4.8
    if (m_conn) {
        mysql_close(m_conn);
        m_conn = nullptr;
    }

    m_conn = mysql_init(nullptr);
    if (!m_conn) {
        std::cout << "[DatabasePool] mysql_init failed\n";
        return false;
    }

    // Disable SSL enforcement - Maria DB Implementation
    my_bool ssl_enforce = 0;
    mysql_options(m_conn, MYSQL_OPT_SSL_ENFORCE, &ssl_enforce);

    // Disable cert verification too - Only safe when server is paired with the DB
    my_bool verify = 0;
    mysql_options(m_conn, MYSQL_OPT_SSL_VERIFY_SERVER_CERT, &verify);

    unsigned int timeout = 10;
    mysql_options(m_conn, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);

    mysql_options(m_conn, MYSQL_SET_CHARSET_NAME, "utf8mb4");

    if (!mysql_real_connect(m_conn, m_host.c_str(), m_user.c_str(), m_pass.c_str(),
                           m_db.c_str(), 3306, nullptr, 0))
    {
        std::cout << "[DatabasePool] " << m_db << " Connection failed: " << mysql_error(m_conn) << std::endl;
        return false;
    }

    std::cout << "[DatabasePool] Successfully connected to " << m_db << std::endl;
    return true;
}

void DatabasePool::AUTH_PrepareStatements()
{
    m_stmts.resize((size_t)Stmt::MAX, nullptr);

    auto p = [&](Stmt id, const char* sql) {
        MYSQL_STMT* stmt = mysql_stmt_init(m_conn);
        if (!stmt) {
            std::cout << "[DB] Failed to init statement " << (int)id << "\n";
            return;
        }

        if (mysql_stmt_prepare(stmt, sql, std::strlen(sql)) != 0) {
            std::cout << "[DB] Prepare failed for stmt " << (int)id
                      << ": " << mysql_stmt_error(stmt) << "\n";
            mysql_stmt_close(stmt);
            return;
        }

        m_stmts[(size_t)id] = stmt;
        std::cout << "[DB] Prepared statement " << (int)id << "\n";
    };

    // Auth
    p(Stmt::AUTH_SEL_ACCOUNT_EXISTS, "SELECT username FROM accounts WHERE username=?");
    p(Stmt::AUTH_INS_ACCOUNT,        "INSERT INTO accounts(username) VALUES(?)");
}

void DatabasePool::CHAR_PrepareStatements()
{
    m_stmts.resize((size_t)Stmt::MAX, nullptr);

    auto p = [&](Stmt id, const char* sql) {
        MYSQL_STMT* stmt = mysql_stmt_init(m_conn);
        if (!stmt) {
            std::cout << "[DB] Failed to init statement " << (int)id << "\n";
            return;
        }

        if (mysql_stmt_prepare(stmt, sql, std::strlen(sql)) != 0) {
            std::cout << "[DB] Prepare failed for stmt " << (int)id
                      << ": " << mysql_stmt_error(stmt) << "\n";
            mysql_stmt_close(stmt);
            return;
        }

        m_stmts[(size_t)id] = stmt;
        std::cout << "[DB] Prepared statement " << (int)id << "\n";
    };

    // Character
    // Check if character name already exists
    p(Stmt::CHAR_SEL_CHARACTER_EXISTS,
        "SELECT 1 FROM `character` WHERE character_name = ? LIMIT 1");

    // Create new character
    p(Stmt::CHAR_INS_CHARACTER,
        "INSERT INTO `character` "
        "(account_id, character_name, race_id, class_id, gender, level, xp, money, "
        "zone_id, position_x, position_y, position_z, position_o, "
        "transport_x, transport_y, transport_z, transport_o) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

    // Delete character
    p(Stmt::CHAR_DEL_CHARACTER,
        "DELETE FROM `character` WHERE guid = ? AND account_id = ?");

    // Save / Update character (full update)
    p(Stmt::CHAR_SAV_CHARACTER,
        "UPDATE `character` SET "
        "level = ?, xp = ?, money = ?, zone_id = ?, "
        "position_x = ?, position_y = ?, position_z = ?, position_o = ?, "
        "transport_x = ?, transport_y = ?, transport_z = ?, transport_o = ? "
        "WHERE guid = ?");

    // Get all characters for an account (Character Selection Screen)
    p(Stmt::CHAR_GET_CHARACTER_ACCOUNT,
        "SELECT guid, character_name, race_id, class_id, gender, level, xp, money, "
        "zone_id, position_x, position_y, position_z, position_o, "
        "transport_x, transport_y, transport_z, transport_o "
        "FROM `character` WHERE account_id = ? ORDER BY character_name");

    // Get one character by name // login request etc
    p(Stmt::CHAR_GET_CHARACTER_NAME,
        "SELECT guid, account_id, character_name, race_id, class_id, gender, level, xp, money, "
        "zone_id, position_x, position_y, position_z, position_o, "
        "transport_x, transport_y, transport_z, transport_o "
        "FROM `character` WHERE character_name = ? LIMIT 1");
}

void DatabasePool::WRLD_PrepareStatements()
{
    m_stmts.resize((size_t)Stmt::MAX, nullptr);

    auto p = [&](Stmt id, const char* sql) {
        MYSQL_STMT* stmt = mysql_stmt_init(m_conn);
        if (!stmt) {
            std::cout << "[DB] Failed to init statement " << (int)id << "\n";
            return;
        }

        if (mysql_stmt_prepare(stmt, sql, std::strlen(sql)) != 0) {
            std::cout << "[DB] Prepare failed for stmt " << (int)id
                      << ": " << mysql_stmt_error(stmt) << "\n";
            mysql_stmt_close(stmt);
            return;
        }

        m_stmts[(size_t)id] = stmt;
        std::cout << "[DB] Prepared statement " << (int)id << "\n";
    };

    // World
    // TODO: Add world-related prepared statements here
    p(Stmt::WORLD_GET_ALL_CREATURE,
    "SELECT creature_id, name, race_id, hp, mp, model_id, level, "
    "       attack, defense, speed, scale, flags, script_name "
    "FROM creature "
    "ORDER BY creature_id");

    p(Stmt::WORLD_GET_CREATURE_ID,   // ← Consider renaming the enum too
    "SELECT creature_id, name, race_id, hp, mp, model_id, level, "
    "       attack, defense, speed, scale, flags, script_name "
    "FROM creature "
   "WHERE creature_id = ?");

    // Loot System
    p(Stmt::WORLD_GET_ALL_LOOTTABLES,
        "SELECT lootpool_id, description, "
        "loot_id_00, lootdroprate_00, mincount_00, maxcount_00, "
        "loot_id_01, lootdroprate_01, mincount_01, maxcount_01, "
        "loot_id_02, lootdroprate_02, mincount_02, maxcount_02 "
        "FROM lootpool_template "
        "ORDER BY lootpool_id ASC");
}

void DatabasePool::Run()
{
    m_workerThread = std::thread([this]()
    {
        DBJob job;

        while (m_running)
        {
            DBJob job;

            if (!DB_Request_Queue.pop(job))
                break;

            job.result = Execute(job);

            job.RequestHandledTimestamp =
                TimeManager::Instance().GetCurrentTimeMs();

            if (job.bHasCallback)
                DB_Callback_Queue.push(std::move(job));
        }
    });
}


void DatabasePool::Stop()
{
    m_running = false;

    DB_Request_Queue.stop();

    if (m_workerThread.joinable())
    {
        m_workerThread.join();
    }

    for (MYSQL_STMT* stmt : m_stmts)
    {
        if (stmt)
        {
            mysql_stmt_close(stmt);
        }
    }

    m_stmts.clear();

    if (m_conn)
    {
        mysql_close(m_conn);
        m_conn = nullptr;
    }

    std::cout << "[DatabasePool] "
              << m_db
              << " stopped\n";
}

void DatabasePool::Submit(DBJob&& job)
{
    DB_Request_Queue.push(std::move(job));
}

QueryResult DatabasePool::FetchResult(MYSQL_STMT *stmt) {
        QueryResult result;

    if (!stmt)
        return result;

    MYSQL_RES* meta = mysql_stmt_result_metadata(stmt);
    if (!meta)
    {
        // Not a SELECT query (INSERT/UPDATE/DELETE)
        mysql_stmt_free_result(stmt);
        return result;
    }

    int numFields = mysql_num_fields(meta);
    std::vector<MYSQL_BIND> binds(numFields);
    std::vector<std::vector<char>> buffers(numFields);
    std::vector<unsigned long> lengths(numFields, 0);
    std::vector<my_bool> isNull(numFields, 0);

    MYSQL_FIELD* fields = mysql_fetch_fields(meta);

    // Force all fields to STRING type - most reliable for mixed INT/FLOAT/VARCHAR
    for (int i = 0; i < numFields; ++i)
    {
        unsigned long bufferSize = 64; // enough for most numbers

        if (fields[i].type == MYSQL_TYPE_STRING ||
            fields[i].type == MYSQL_TYPE_VAR_STRING ||
            fields[i].type == MYSQL_TYPE_BLOB)
        {
            bufferSize = fields[i].length > 0 ? fields[i].length + 1 : 512;
        }

        if (bufferSize > 16384) bufferSize = 16384;

        buffers[i].resize(bufferSize, '\0');

        binds[i].buffer_type   = MYSQL_TYPE_STRING;        // Key fix
        binds[i].buffer        = buffers[i].data();
        binds[i].buffer_length = bufferSize - 1;
        binds[i].length        = &lengths[i];
        binds[i].is_null       = &isNull[i];
    }

    if (mysql_stmt_bind_result(stmt, binds.data()) != 0)
    {
        std::cout << "[DB] Bind result error: " << mysql_stmt_error(stmt) << "\n";
        mysql_free_result(meta);
        mysql_stmt_free_result(stmt);
        return result;
    }

    mysql_stmt_store_result(stmt);

    while (true)
    {
        int status = mysql_stmt_fetch(stmt);
        if (status == MYSQL_NO_DATA) break;
        if (status != 0)
        {
            std::cout << "[DB] Fetch error: " << mysql_stmt_error(stmt) << "\n";
            break;
        }

        std::vector<DBField> row;
        row.reserve(numFields);

        for (int i = 0; i < numFields; ++i)
        {
            DBField field;
            if (isNull[i] || lengths[i] == 0)
            {
                field.value = "";
            }
            else
            {
                field.value.assign(buffers[i].data(), lengths[i]);
            }
            row.push_back(std::move(field));
        }

        result.AddRow(std::move(row));
    }

    mysql_free_result(meta);
    mysql_stmt_free_result(stmt);

    return result;
}

void DatabasePool::PumpCallbacks()
{
    DBJob job;

    while (DB_Callback_Queue.try_pop(job))
    {
        if (job.bHasCallback == true)
        {
            job.callback(job);
        }
    }
}


QueryResult DatabasePool::Execute(DBJob& q) {
    QueryResult result;

    MYSQL_STMT* stmt = m_stmts[(size_t)q.stmt];
    if (!stmt)
    {
        std::cout << "[DB] Statement not prepared: " << (int)q.stmt << "\n";
        return result;
    }

    // Bind parameters
    if (q.params.Size() > 0)
    {
        if (mysql_stmt_bind_param(stmt, q.params.Get()) != 0)
        {
            std::cout << "[DB] Bind param error: " << mysql_stmt_error(stmt) << "\n";
            return result;
        }
    }

    if (mysql_stmt_execute(stmt) != 0)
    {
        std::cout << "[DB] Execute error: " << mysql_stmt_error(stmt) << "\n";
        return result;
    }

    return FetchResult(stmt);
}

QueryResult DatabasePool::ForceQuery_NoParams(Stmt stmt)
{
    PreparedStatement emptyParams;
    return ForceQuery(stmt, emptyParams);
}

QueryResult DatabasePool::ForceQuery(Stmt stmt,PreparedStatement& params)
{
    QueryResult result;

    if (!m_conn)
    {
        std::cout << "[DB] ForceQuery failed - No connection\n";
        return result;
    }

    MYSQL_STMT* mysqlStmt = m_stmts[(size_t)stmt];
    if (!mysqlStmt)
    {
        std::cout << "[DB] ForceQuery failed - Statement not prepared: "
                  << (int)stmt << "\n";
        return result;
    }

    // Bind parameters if any
    if (params.Size() > 0)
    {
        if (mysql_stmt_bind_param(mysqlStmt, params.Get()) != 0)
        {
            std::cout << "[DB] ForceQuery bind error: "
                      << mysql_stmt_error(mysqlStmt) << "\n";
            return result;
        }
    }

    if (mysql_stmt_execute(mysqlStmt) != 0)
    {
        std::cout << "[DB] ForceQuery execute error: "
                  << mysql_stmt_error(mysqlStmt) << "\n";
        return result;
    }

    return FetchResult(mysqlStmt);
}