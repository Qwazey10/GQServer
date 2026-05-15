#pragma once

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
};