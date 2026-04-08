#ifndef GQUESTSERVER_NETWORKSOCKETMGR_H
#define GQUESTSERVER_NETWORKSOCKETMGR_H
#include <memory>

#include <asio.hpp>
//#include "asio/ip/tcp.hpp"


class NetworkSocketMgr {
private:
    // Private constructor - prevents direct instantiation
    NetworkSocketMgr();

    // Delete copy constructor and assignment operator
    NetworkSocketMgr(const NetworkSocketMgr&) = delete;
    NetworkSocketMgr& operator=(const NetworkSocketMgr&) = delete;

public:
    // Static access method - this is the singleton getter
    static NetworkSocketMgr& Instance() {
        static NetworkSocketMgr instance;  // Created only once, thread-safe
        return instance;
    }

    void StartListening(const std::string& ip, int port, asio::io_context& io_context);
    void DoAccept();

    void Stop();

    //Asio TCP Acceptor
    std::unique_ptr<asio::ip::tcp::acceptor> acceptor_;
    //Asio TCP Socket
    std::unique_ptr<asio::ip::tcp::socket> socket_;
    //Asio TCP Endpoint
    asio::ip::tcp::endpoint endpoint_;
};
    // Add your network-related methods here, e.g.:
    // void StartListening(...);
    // void SendPacket(...);
    // etc.


#endif //GQUESTSERVER_NETWORKSOCKETMGR_H