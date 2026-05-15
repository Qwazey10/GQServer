#include "DBWorker.h"
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
        std::cerr << "[DBWorker] mysql_init() failed!\n";
        return;
    }

    // ====================== FORCE DISABLE SSL ======================
    my_bool ssl_disable = 1;
    mysql_options(m_conn, MYSQL_OPT_SSL_ENFORCE, &ssl_disable);

    // Additional helpful options
    unsigned int timeout = 10;
    mysql_options(m_conn, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);

    mysql_options(m_conn, MYSQL_SET_CHARSET_NAME, "utf8mb4");

    // Optional: Try to disable reconnect if you get weird issues
    // my_bool reconnect = 0;
    // mysql_options(m_conn, MYSQL_OPT_RECONNECT, &reconnect);

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
}