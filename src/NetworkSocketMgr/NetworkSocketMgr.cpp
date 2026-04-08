#include "NetworkSocketMgr.h"
#include <iostream>

void NetworkSocketMgr::StartListening(const std::string& ip, int port, asio::io_context& io_context) {
    //Create Context Endpoint

    asio::error_code ec;

    try
    {
        // 1. Create the endpoint
        auto address = asio::ip::make_address(ip, ec);
        if (ec)
        {
            std::cerr << "Invalid IP address '" << ip << "': " << ec.message() << std::endl;
            return;
        }

        endpoint_ = asio::ip::tcp::endpoint(address, port);

        // 2. Create the acceptor
        acceptor_ = std::make_unique<asio::ip::tcp::acceptor>(io_context);

        // 3. Open, set options, bind and listen
        acceptor_->open(endpoint_.protocol(), ec);
        if (ec) throw std::system_error(ec);

        acceptor_->set_option(asio::ip::tcp::acceptor::reuse_address(true), ec);
        if (ec) throw std::system_error(ec);

        acceptor_->bind(endpoint_, ec);
        if (ec) throw std::system_error(ec);

        acceptor_->listen(asio::socket_base::max_listen_connections, ec);
        if (ec) throw std::system_error(ec);

        std::cout << "Server started successfully - Listening on "
                  << ip << ":" << port << std::endl;

        // Start the first asynchronous accept
        DoAccept();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to start server on " << ip << ":" << port
                  << " -> " << e.what() << std::endl;
    }


}

void NetworkSocketMgr::DoAccept() {
    if (!acceptor_ || !acceptor_->is_open())
        return;

    auto new_socket = std::make_unique<asio::ip::tcp::socket>(acceptor_->get_executor());

    acceptor_->async_accept(*new_socket,
        [this, socket = std::move(new_socket)](const asio::error_code& ec) mutable
        {
            if (!ec)
            {
                std::cout << "New connection accepted from "
                          << socket->remote_endpoint() << std::endl;

                std::cout << "Authenticating connection... (0xDEADBEEF for now)" << std::endl;

                // TODO: Move the socket to a Session / Connection object
                // CreateSession(std::move(*socket));   // or std::move(socket)
            }
            else if (ec != asio::error::operation_aborted)
            {
                std::cerr << "Accept error: " << ec.message() << std::endl;
            }

            // Restart accept for the next client
            DoAccept();
        });
}
void NetworkSocketMgr::Stop()
{
    asio::error_code ec;
    if (acceptor_ && acceptor_->is_open())
    {
        acceptor_->cancel(ec);   // Cancels pending async_accept
        acceptor_->close(ec);
    }
    // You can also close all active sessions here later
}