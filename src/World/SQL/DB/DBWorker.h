#pragma once
#include "SafeQueue.h"
#include "DBOperation.h"
#include <mysql.h>
#include <atomic>

class DBWorker
{
public:
    DBWorker(SafeQueue<DBOp>& q,
        SafeQueue<std::function<void()>>& dispatch,
        const std::string& host,
        const std::string& user,
        const std::string& pass,
        const std::string& db);

    void run();

    bool IsHealthy() const;
    void RequestStop();

private:
    bool Connect();
    bool ReconnectWithBackoff();
    void PrepareStatements();
    QueryResult Execute(DBQuery& q);
    void ExecuteTransaction(DBTransaction& tx);

    uint64_t NowMs() const;

    std::atomic<bool> running{ true };
    std::atomic<uint64_t> lastWork{ 0 };

    MYSQL* conn = nullptr;
    std::vector<MYSQL_STMT*> stmts;

    SafeQueue<DBOp>& queue;
    SafeQueue<std::function<void()>>& dispatch;

    std::string host, user, pass, db;
};


/*#pragma once*/
/*
#include "DBJob.h"
#include "SafeQueue.h"
#include <mysql.h>
#include <string>
#include <vector>
#include <iostream>

class DBWorker
{
public:
    DBWorker(SafeQueue<DBJob>& queue,
        const std::string& host,
        const std::string& user,
        const std::string& pass,
        const std::string& db);

    void run();

private:
    void execute(DBJob& job);

    SafeQueue<DBJob>& m_queue;
    MYSQL* m_conn = nullptr;
};*/