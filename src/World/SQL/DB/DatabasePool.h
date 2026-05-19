#pragma once
#include "DBWorker.h"
#include <thread>
#include <memory>

class DatabasePool
{
public:
    void Start(int n,
        SafeQueue<std::function<void()>>& dispatch,
        const std::string& host,
        const std::string& user,
        const std::string& pass,
        const std::string& db)
    {
        for (int i = 0; i < n; i++)
            SpawnWorker(dispatch, host, user, pass, db);
    }

    void Stop()
    {
        queue.stop();

        for (auto& w : workers) w->RequestStop();
        for (auto& t : threads) t.join();
    }

    void Enqueue(DBOp op) { queue.push(std::move(op)); }

    void HealthCheck(SafeQueue<std::function<void()>>& dispatch,
        const std::string& host,
        const std::string& user,
        const std::string& pass,
        const std::string& db)
    {
        for (size_t i = 0; i < workers.size(); i++)
        {
            if (!workers[i]->IsHealthy())
            {
                std::cout << "[DBPool] Worker stalled, replacing...\n";

                workers[i]->RequestStop();
                threads[i].join();

                SpawnWorker(dispatch, host, user, pass, db, i);
            }
        }
    }

private:
    void SpawnWorker(SafeQueue<std::function<void()>>& dispatch,
        const std::string& host,
        const std::string& user,
        const std::string& pass,
        const std::string& db,
        size_t replaceIndex = SIZE_MAX)
    {
        auto w = std::make_shared<DBWorker>(queue, dispatch, host, user, pass, db);

        std::thread t([w]() { w->run(); });

        if (replaceIndex == SIZE_MAX)
        {
            workers.push_back(w);
            threads.push_back(std::move(t));
        }
        else
        {
            workers[replaceIndex] = w;
            threads[replaceIndex] = std::move(t);
        };

/*#include <future>
#include "DBWorker.h"


class DatabasePool
{
public:
    void Start(int numThreads,
        const std::string& host,
        const std::string& user,
        const std::string& pass,
        const std::string& db)
    {
        for (int i = 0; i < numThreads; i++)
        {
            m_threads.emplace_back([=]() {
                DBWorker worker(m_queue, host, user, pass, db);
                worker.run();
                });
        }
    }

    void Stop()
    {
        m_queue.shutdown();

        for (auto& t : m_threads)
            t.join();
    }

    // Fire and forget
    void Execute(const std::string& query)
    {
        DBJob job;
        job.query = query;
        m_queue.push(std::move(job));
    }

    // Future-based
    std::future<DBResult> Query(const std::string& query)
    {
        DBJob job;
        job.query = query;

        auto promise = std::make_shared<std::promise<DBResult>>();
        job.promise = promise;

        m_queue.push(std::move(job));
        return promise->get_future();
    }

    // Callback-based
    void QueryAsync(const std::string& query,
        std::function<void(DBResult)> callback)
    {
        DBJob job;
        job.query = query;
        job.callback = callback;

        m_queue.push(std::move(job));
    }

private:
    SafeQueue<DBJob> m_queue;
    std::vector<std::thread> m_threads;
};*/