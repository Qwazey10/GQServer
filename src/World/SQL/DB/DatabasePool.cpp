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
/*void DatabasePool::PrepareStatements()
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

    // Character
    p(Stmt::CHAR_GET_ALL_INVENTORY,      "SELECT item_id, quantity FROM inventory WHERE character_name=?");

    // TODO: Add more prepared statements here
}*/
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
    p(Stmt::CHAR_GET_ALL_INVENTORY, "SELECT item_id, quantity FROM inventory WHERE character_name=?");
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
    "FROM creature_template "
    "ORDER BY creature_id");

    p(Stmt::WORLD_GET_CREATURE_ID,   // ← Consider renaming the enum too
   "SELECT creature_id, name, race_id, hp, mp, model_id, level, "
   "       attack, defense, speed, scale, flags, script_name "
   "FROM creature_template "
   "WHERE creature_id = ?");
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
    if (!stmt) {
        std::cout << "[DB] Statement not prepared: " << (int)q.stmt << "\n";
        return result;
    }

    // Bind parameters
    if (q.params.Size() > 0) {
        if (mysql_stmt_bind_param(stmt, q.params.Get()) != 0) {
            std::cout << "[DB] Bind param error: " << mysql_stmt_error(stmt) << "\n";
            return result;
        }
    }

    if (mysql_stmt_execute(stmt) != 0) {
        std::cout << "[DB] Execute error: " << mysql_stmt_error(stmt) << "\n";
        return result;
    }

    MYSQL_RES* meta = mysql_stmt_result_metadata(stmt);
    if (!meta) {
        // No result set (INSERT, UPDATE, etc.)
        mysql_stmt_free_result(stmt);
        return result;
    }

    int numFields = mysql_num_fields(meta);
    std::vector<MYSQL_BIND> binds(numFields);
    std::vector<std::vector<char>> buffers(numFields);
    std::vector<unsigned long> lengths(numFields);
    std::vector<my_bool> isNull(numFields);

    memset(binds.data(), 0, sizeof(MYSQL_BIND) * numFields);

    MYSQL_FIELD* fields = mysql_fetch_fields(meta);

    for (int i = 0; i < numFields; i++) {
        unsigned long size = fields[i].length;
        if (size < 1) size = 1;
        if (size > 4096) size = 4096;

        buffers[i].resize(size);

        binds[i].buffer_type = fields[i].type;
        binds[i].buffer = buffers[i].data();
        binds[i].buffer_length = size;
        binds[i].length = &lengths[i];
        binds[i].is_null = &isNull[i];
    }

    mysql_stmt_bind_result(stmt, binds.data());
    mysql_stmt_store_result(stmt);

    while (true) {
        int status = mysql_stmt_fetch(stmt);
        if (status == MYSQL_NO_DATA) break;
        if (status != 0) {
            std::cout << "[DB] Fetch error: " << mysql_stmt_error(stmt) << "\n";
            break;
        }

        std::vector<DBField> row;
        row.reserve(numFields);

        for (int i = 0; i < numFields; i++) {
            DBField field;
            if (isNull[i]) {
                field.value = "";
            } else {
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
