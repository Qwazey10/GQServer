
#include "DatabaseManager.h"

DatabaseManager& DatabaseManager::Instance()
{
    static DatabaseManager i;
    return i;
}

void DatabaseManager::Init()
{
    auth.Start(2, dispatch, "127.0.0.1", "devgquest", "Apple1", "auth_gquest");
    chr.Start(2, dispatch, "127.0.0.1", "devgquest", "Apple1", "character_gquest");
    content.Start(2, dispatch, "127.0.0.1", "devgquest", "Apple1", "content_gquest");
}

void DatabaseManager::Shutdown()
{
    auth.Stop();
    chr.Stop();
    content.Stop();
}

void DatabaseManager::Pump()
{
    std::function<void()> fn;
    while (dispatch.try_pop(fn)) fn();
}

std::future<QueryResult> DatabaseManager::Query(Stmt stmt, std::function<void(PreparedStatement&)> binder)
{
    DBQuery q;
    q.stmt = stmt;

    binder(q.params);

    auto p = std::make_shared<std::promise<QueryResult>>();
    q.promise = p;

    auth.Enqueue(q);
    return p->get_future();
}

void DatabaseManager::ExecuteTransaction(DBTransaction tx)
{
    auth.Enqueue(tx);
}

void DatabaseManager::HealthCheck()
{
    auth.HealthCheck(dispatch, "127.0.0.1", "devgquest", "Apple1", "auth_gquest");
    chr.HealthCheck(dispatch, "127.0.0.1", "devgquest", "Apple1", "character_gquest");
    content.HealthCheck(dispatch, "127.0.0.1", "devgquest", "Apple1", "content_gquest");
}


/*#include "DatabaseManager.h"
#include <memory>
#include <string>
#include <iostream>

void DatabaseManager::InitDB()
{
    m_authPool.Start(2, "192.168.1.13", "devgquest", "Apple1", "auth_gquest");
    m_characterPool.Start(2, "192.168.1.13", "devgquest", "Apple1", "character_gquest");
    m_contentPool.Start(2, "192.168.1.13", "devgquest", "Apple1", "content_gquest");
}

void DatabaseManager::ShutdownDB()
{
    m_authPool.Stop();
    m_characterPool.Stop();
    m_contentPool.Stop();
}*/
