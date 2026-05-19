#include "DBWorker.h"
#include <thread>
#include <iostream>

DBWorker::DBWorker(SafeQueue<DBOp>& q,
    SafeQueue<std::function<void()>>& d,
    const std::string& h,
    const std::string& u,
    const std::string& p,
    const std::string& database)
    : queue(q), dispatch(d), host(h), user(u), pass(p), db(database)
{
    Connect();
}

uint64_t DBWorker::NowMs() const
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}

bool DBWorker::Connect()
{
    conn = mysql_init(nullptr);
    if (!conn) return false;

    if (!mysql_real_connect(conn, host.c_str(), user.c_str(), pass.c_str(), db.c_str(), 3306, nullptr, 0))
        return false;

    PrepareStatements();
    return true;
}

bool DBWorker::ReconnectWithBackoff()
{
    int delay = 1000;

    while (running)
    {
        std::cout << "[DB] Reconnecting...\n";

        if (Connect())
        {
            std::cout << "[DB] Reconnected\n";
            return true;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        delay = std::min(delay * 2, 30000);
    }

    return false;
}

void DBWorker::PrepareStatements()
{
    stmts.resize((size_t)Stmt::MAX);

    auto p = [&](Stmt id, const char* sql)
        {
            auto s = mysql_stmt_init(conn);
            mysql_stmt_prepare(s, sql, strlen(sql));
            stmts[(size_t)id] = s;
        };

    p(Stmt::AUTH_SEL_ACCOUNT_EXISTS, "SELECT username FROM accounts WHERE username=?");
    p(Stmt::AUTH_INS_ACCOUNT, "INSERT INTO accounts(username) VALUES(?)");



/*#include "DBWorker.h"
#include <mysql.h>
#include <iostream>

DBWorker::DBWorker(SafeQueue<DBJob>& queue,
    const std::string& host,
    const std::string& user,
    const std::string& pass,
    const std::string& db)
    : m_queue(queue)
{
    m_conn = mysql_init(nullptr);

    if (!m_conn)
    {
        std::cerr << "[DBWorker] mysql_init failed\n";
        return;
    }

    // ✅ Disable SSL enforcement (correct for MariaDB)
    my_bool ssl_enforce = 0;
    mysql_options(m_conn, MYSQL_OPT_SSL_ENFORCE, &ssl_enforce);

    // Optional: disable cert verification too (safe for local dev)
    my_bool verify = 0;
    mysql_options(m_conn, MYSQL_OPT_SSL_VERIFY_SERVER_CERT, &verify);

    unsigned int timeout = 10;
    mysql_options(m_conn, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);

    mysql_options(m_conn, MYSQL_SET_CHARSET_NAME, "utf8mb4");


    std::cout << "[DBWorker] Connecting to " << db
        << " on " << host << ":3306...\n";

    if (!mysql_real_connect(m_conn,
        host.c_str(),
        user.c_str(),
        pass.c_str(),
        db.c_str(),
        3306,
        nullptr,
        0))
    {
        std::cerr << "[DBWorker] ❌ Connection failed: "
            << mysql_error(m_conn) << "\n";
        mysql_close(m_conn);
        m_conn = nullptr;
    }
    else
    {
        std::cout << "[DBWorker] ✅ Successfully connected to " << db << "\n";
        std::cout << "[DBWorker] Server: " << mysql_get_server_info(m_conn) << "\n";
    }
}

void DBWorker::run()
{
    DBJob job;

    while (m_queue.pop(job))
    {
        if (m_conn == nullptr)
        {
            std::cerr << "[DBWorker] No active connection - skipping query\n";
            continue;
        }
        execute(job);
    }
}

void DBWorker::execute(DBJob& job)
{
    if (!m_conn) return;

    DBResult result;

    if (mysql_query(m_conn, job.query.c_str()))
    {
        std::cerr << "[DBWorker] Query failed: " << mysql_error(m_conn)
            << "\nQuery: " << job.query.substr(0, 200) << "...\n";
        return;
    }

    MYSQL_RES* res = mysql_store_result(m_conn);
    if (res)
    {
        int num_fields = mysql_num_fields(res);
        MYSQL_ROW row;

        while ((row = mysql_fetch_row(res)))
        {
            std::vector<std::string> rowData;
            for (int i = 0; i < num_fields; ++i)
            {
                rowData.emplace_back(row[i] ? row[i] : "");
            }
            result.rows.push_back(std::move(rowData));
        }
        mysql_free_result(res);
    }

    if (job.callback)
        job.callback(result);

    if (job.promise)
        job.promise->set_value(result);
}*/