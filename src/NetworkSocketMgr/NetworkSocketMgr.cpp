#include "NetworkSocketMgr.h"
#include "../World/WorldSession.h"
#include <iostream>

#include "World/WorldSessionMgr.h"

void NetworkSocketMgr::StartListening(const std::string& ip, uint16_t port, asio::io_context& io_context)
{
    io_context_ = &io_context;
    try
    {
        // Use the most common, reliable constructor style for standalone Asio
        asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), port);   // start with IPv4 for simplicity

        // If user wants IPv6, we can extend later
        if (ip.find(':') != std::string::npos && ip != "0.0.0.0")
            endpoint = asio::ip::tcp::endpoint(asio::ip::tcp::v6(), port);

        acceptor_ = std::make_unique<asio::ip::tcp::acceptor>(io_context, endpoint);  // constructor that does open+bind+listen internally

        // Extra safety: explicitly enable reuse (some systems need this)
        acceptor_->set_option(asio::ip::tcp::acceptor::reuse_address(true));

        std::cout << "Server started successfully - Listening on " << ip << ":" << port << std::endl;

        DoAccept();
    }
    catch (const std::exception& e)
    {
        std::cerr << "=== StartListening FAILED ===\n";
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "IP: " << ip << " Port: " << port << std::endl;
        acceptor_.reset();
    }
}

void NetworkSocketMgr::DoAccept()
{
    if (!acceptor_)
    {
        std::cerr << "DoAccept: acceptor_ is nullptr!\n";
        return;
    }

    if (!acceptor_->is_open())
    {
        std::cerr << "DoAccept: acceptor is not open! (is_open() returned false)\n";
        return;
    }

    std::cout << "Starting async_accept...\n";   // temporary debug line - remove later

    auto socket = std::make_shared<asio::ip::tcp::socket>(*io_context_);

    acceptor_->async_accept(*socket,
      [this, socket](const asio::error_code& ec) mutable
      {
          if (!ec)
          {
              static int nextPlayerId = 1;
              int playerId = nextPlayerId++;

              try
              {
                  auto session = std::make_shared<WorldSession>(std::move(*socket), playerId);
                  session->Start();
                  WorldSessionMgr::Instance().AddSession(session);

                  std::cout << "New player connected! ID: " << playerId << "\n";
              }
              catch (const std::exception& e)
              {
                  std::cerr << "Failed to create session: " << e.what() << "\n";
              }
          }
          else if (ec != asio::error::operation_aborted)
          {
              std::cerr << "Accept error: " << ec.message() << "\n";
          }

          DoAccept();
      });
}

void NetworkSocketMgr::Stop() {
    if (acceptor_ && acceptor_->is_open()) {
        asio::error_code ec;
        acceptor_->close(ec);
        std::cout << "Acceptor closed.\n";
    }
}