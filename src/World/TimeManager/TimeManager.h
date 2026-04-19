#ifndef GQUESTSERVER_TIMEMANAGER_H
#define GQUESTSERVER_TIMEMANAGER_H

#include <chrono>
#include <cstdint>
#include <vector>
#include <functional>
#include <algorithm>

class TimeManager {
public:
    using TimerID = uint32_t;
    using TimerCallback = std::function<void()>;

    // Delete copy
    TimeManager(const TimeManager&) = delete;
    TimeManager& operator=(const TimeManager&) = delete;

    // Instance Declaration
    static TimeManager& Instance() {
        static TimeManager instance;
        return instance;
    }

    // ====================== Initialization ======================
    void Initialize() {
        startTime = std::chrono::steady_clock::now();
        lastUpdateTime = startTime;
    }

    // ====================== Update (call every tick/frame) ======================
    void Update() {
        auto now = std::chrono::steady_clock::now();
        lastUpdateTime = now;

        // Process timers
        ProcessTimers();
    }

    // ====================== General Server Time ======================

    // Current server time in milliseconds since epoch (wall-clock time)
    uint64_t GetCurrentTimeMs() const {
        auto now = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()
        ).count();
    }

    // Current server time in microseconds since epoch
    uint64_t GetCurrentTimeUs() const {
        auto now = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(
            now.time_since_epoch()
        ).count();
    }

    // ====================== Server Uptime ======================

    // Uptime in milliseconds since server started
    uint64_t GetUptimeMs() const {
        if (startTime.time_since_epoch().count() == 0) return 0;
        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            now - startTime
        ).count();
    }

    // Uptime in seconds (more readable)
    double GetUptimeSeconds() const {
        return GetUptimeMs() / 1000.0;
    }

    // ====================== Delta Time (for game logic) ======================

    // Time since last Update() call in milliseconds
    uint64_t GetDeltaTimeMs() const {
        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            now - lastUpdateTime
        ).count();
    }

    // ====================== Ping / Pong ======================

    uint64_t StartPing() {
        lastPingTime = GetCurrentTimeUs();
        return lastPingTime;
    }

    uint64_t EndPing(uint64_t pingTimestamp) const {
        return GetCurrentTimeUs() - pingTimestamp;
    }

    uint64_t GetLastPingTime() const {
        return lastPingTime;
    }

    // ====================== Timers / Scheduled Events ======================

    // Schedule a one-time callback after delayMs milliseconds
    TimerID Schedule(uint64_t delayMs, TimerCallback callback) {
        auto now = std::chrono::steady_clock::now();
        Timer timer{
            nextTimerID++,
            now + std::chrono::milliseconds(delayMs),
            std::move(callback),
            0,           // repeat = 0 means one-shot
            false
        };
        timers.push_back(std::move(timer));
        return timer.id;
    }

    // Schedule a repeating timer
    TimerID ScheduleRepeating(uint64_t intervalMs, TimerCallback callback) {
        auto now = std::chrono::steady_clock::now();
        Timer timer{
            nextTimerID++,
            now + std::chrono::milliseconds(intervalMs),
            std::move(callback),
            intervalMs,
            true
        };
        timers.push_back(std::move(timer));
        return timer.id;
    }

    // Cancel a timer
    bool CancelTimer(TimerID id) {
        auto it = std::find_if(timers.begin(), timers.end(),
            [id](const Timer& t) { return t.id == id; });
        if (it != timers.end()) {
            timers.erase(it);
            return true;
        }
        return false;
    }

private:
    TimeManager() = default;

    struct Timer {
        TimerID id;
        std::chrono::steady_clock::time_point expiry;
        TimerCallback callback;
        uint64_t intervalMs;     // 0 = one-shot
        bool repeating;
    };

    void ProcessTimers() {
        auto now = std::chrono::steady_clock::now();

        for (size_t i = 0; i < timers.size(); ) {
            if (now >= timers[i].expiry) {
                timers[i].callback();

                if (timers[i].repeating && timers[i].intervalMs > 0) {
                    // Reschedule
                    timers[i].expiry = now + std::chrono::milliseconds(timers[i].intervalMs);
                    ++i;
                } else {
                    // Remove one-shot timer
                    timers.erase(timers.begin() + i);
                }
            } else {
                ++i;
            }
        }
    }

    // Member variables
    std::chrono::steady_clock::time_point startTime{};
    std::chrono::steady_clock::time_point lastUpdateTime{};

    uint64_t lastPingTime = 0;

    std::vector<Timer> timers;
    TimerID nextTimerID = 1;
};

#endif // GQUESTSERVER_TIMEMANAGER_H