#include <iostream>
#include <thread>
#include "src/NetworkSocketMgr/NetworkSocketMgr.h"
#include "src/World/World.h"

int main() {
    try {
        asio::io_context io_context;
        auto work = asio::make_work_guard(io_context);

        std::cout << "=== GQServer Starting ===\n";

        std::cout << " MAIN -- Starting NetworkSocketMgr...\n";
        NetworkSocketMgr::Instance().StartListening("127.0.0.1", 12345, io_context);
        std::cout << "MAIN -- NetworkSocketMgr started successfully.\n";

        std::cout << "MAIN -- Starting World thread...\n";
        World::Instance().Start();
        std::cout << "MAIN -- World thread started.\n";

        std::cout << "MAIN -- Starting io_context...\n";
        std::thread networkThread([&]() {
            std::cout << " Network Thread -- io_context running...\n";
            io_context.run();
        });


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