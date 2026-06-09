#pragma once

#include "DBTypes.h"
#include "SafeQueue.h"

#include <atomic>
#include <thread>
#include <vector>
#include <mysql.h>

class DatabasePool
{
public:

    void Start(
        DatabaseClassification type,
        int numThreads,
        const std::string& host,
        const std::string& user,
        const std::string& pass,
        const std::string& db);

    void Stop();

    bool Connect();

    void AUTH_PrepareStatements();
    void CHAR_PrepareStatements();
    void WRLD_PrepareStatements();

    void Run();

    void PumpCallbacks();

    void Submit(DBJob&& job);

    QueryResult Execute(DBJob& q);

    void RequestStop()
    {
        m_running = false;
    }

private:

    SafeQueue<DBJob> DB_Request_Queue;
    SafeQueue<DBJob> DB_Callback_Queue;

    std::thread m_workerThread;

    std::string m_host;
    std::string m_user;
    std::string m_pass;
    std::string m_db;

    MYSQL* m_conn = nullptr;

    std::vector<MYSQL_STMT*> m_stmts;

    std::atomic<bool> m_running{ true };
};