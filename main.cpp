#include <iostream>
#include <thread>
#include "src/NetworkSocketMgr/NetworkSocketMgr.h"
#include "src/World/World.h"

int main() {
    try {
        asio::io_context io_context;
        auto work = asio::make_work_guard(io_context);

        std::cout << "=== GQServer Starting ===\n";

        std::cout << "Starting NetworkSocketMgr...\n";
        NetworkSocketMgr::Instance().StartListening("0.0.0.0", 12345, io_context);
        std::cout << "NetworkSocketMgr started successfully.\n";

        std::cout << "Starting World thread...\n";
        World::Instance().Start();
        std::cout << "World thread started.\n";

        std::thread networkThread([&]() {
            std::cout << "io_context running...\n";
            io_context.run();
        });

        std::cout << "Server listening on port 12345\nPress Enter to stop...\n";

        std::cin.get();

        std::cout << "Shutting down...\n";
        World::Instance().Stop();
        io_context.stop();
        if (networkThread.joinable()) networkThread.join();

        std::cout << "Server stopped.\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown exception caught!\n";
    }
    return 0;
}