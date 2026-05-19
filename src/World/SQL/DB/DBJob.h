#pragma once
#include <vector>
#include <string>
#include <functional>
#include <future>
#include "Statements.h"
#include "DBResult.h"

struct DBJob
{
    PreparedStatements stmt;

    std::vector<std::string> stringParams;
    std::vector<int32_t> intParams;

    std::function<void(DBResult)> callback;
    std::shared_ptr<std::promise<DBResult>> promise;
};