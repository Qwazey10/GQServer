#include <future>
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
};