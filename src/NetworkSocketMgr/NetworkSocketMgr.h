#pragma once
#include <asio.hpp>
#include <memory>
#include <string>

class NetworkSocketMgr {
private:
    NetworkSocketMgr() = default;
    NetworkSocketMgr(const NetworkSocketMgr&) = delete;
    NetworkSocketMgr& operator=(const NetworkSocketMgr&) = delete;

public:
    static NetworkSocketMgr& Instance() {
        static NetworkSocketMgr instance;
        return instance;
    }
    void StartListening(const std::string& ip, uint16_t port, asio::io_context& io_context);

    void DoAccept();
    void Stop();

    bool IsListening() const { return acceptor_ && acceptor_->is_open(); }

private:
    asio::io_context* io_context_ = nullptr;
    std::unique_ptr<asio::ip::tcp::acceptor> acceptor_;
};