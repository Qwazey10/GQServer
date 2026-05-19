#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class SafeQueue
{
public:
    void push(T v)
    {
        {
            std::lock_guard<std::mutex> l(m);
            q.push(std::move(v));
        }
        cv.notify_one();
    }

    bool try_pop(T& out)
    {
        std::lock_guard<std::mutex> l(m);
        if (q.empty()) return false;
        out = std::move(q.front());
        q.pop();
        return true;
    }

    bool pop(T& out)
    {
        std::unique_lock<std::mutex> l(m);
        cv.wait(l, [&] { return shutdown || !q.empty(); });

        if (shutdown && q.empty()) return false;

        out = std::move(q.front());
        q.pop();
        return true;
    }

    void drain(std::vector<T>& out)
    {
        std::lock_guard<std::mutex> l(m);
        while (!q.empty())
        {
            out.push_back(std::move(q.front()));
            q.pop();
        }
    }

    void stop()
    {
        {
            std::lock_guard<std::mutex> l(m);
            shutdown = true;
        }
        cv.notify_all();
    }

private:
    std::queue<T> q;
    std::mutex m;
    std::condition_variable cv;
    bool shutdown = false;
};

/*
#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class SafeQueue
{
public:
    void push(T value)
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_queue.push(std::move(value));
        }
        m_cv.notify_one();
    }

    bool pop(T& out)
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        m_cv.wait(lock, [&]() { return !m_queue.empty() || m_shutdown; });

        if (m_shutdown)
            return false;

        out = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }

    void shutdown()
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_shutdown = true;
        }
        m_cv.notify_all();
    }

private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    bool m_shutdown = false;
};*/