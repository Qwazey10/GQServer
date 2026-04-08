#include <iostream>
#include <NetworkSocketMgr/NetworkSocketMgr.h>
#include <asio.hpp>

// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
int main() {
    // Create Asio io_context
    asio::io_context io_context;
    std::cout << "Main.cpp -- Created ASIO Context" << std::endl;

    // Keep io_context alive even when there's no pending work
    auto work_guard = asio::make_work_guard(io_context);

    // Initialize the singleton early (this triggers construction safely)
    NetworkSocketMgr& networkMgr = NetworkSocketMgr::Instance();
    std::cout << "NetworkSocketMgr singleton initialized" << std::endl;

    // Example: Start your network thread(s)
    std::thread networkThread([&io_context, &networkMgr]() {
        // You can access the singleton safely from here

        //
        networkMgr.StartListening("127.0.0.1", 1234, io_context);
        // Run the io_context in this thread (common pattern)
        io_context.run();
    });

    // You can now access the singleton from any thread, e.g.:
    // NetworkSocketMgr::Instance().SomeOtherMethod();

    // ... rest of your program ...

    // Clean shutdown example
    io_context.stop();
    if (networkThread.joinable()) {
        networkThread.join();
    }

    return 0;

}