#pragma once

#include <mysql.h>
#include <functional>
#include <future>
#include <string>

struct DBResult
{
    std::vector<std::vector<std::string>> rows;
};

struct DBJob
{
    std::string query;

    // Optional callback (async result)
    std::function<void(DBResult)> callback;

    // Optional promise (future-based)
    std::shared_ptr<std::promise<DBResult>> promise;
};