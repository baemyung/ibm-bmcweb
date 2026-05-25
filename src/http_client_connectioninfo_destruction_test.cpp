// Test to reproduce ConnectionInfo coredump by destroying connection during async callback
#include "boost_formatters.hpp"
#include "dbus_singleton.hpp"
#include "http_client.hpp"
#include "ssl_key_handler.hpp"

#include <boost/asio/io_context.hpp>
#include <sdbusplus/asio/connection.hpp>

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

// Global pointer to ConnectionInfo to simulate it being destroyed during callback
std::shared_ptr<crow::ConnectionInfo> globalConnection;

void testConnectionInfoDestructionDuringCallback()
{
    boost::asio::io_context ioc;
    
    // Initialize systemBus for DNS resolution
    sdbusplus::asio::connection systemBus(ioc);
    crow::connections::systemBus = &systemBus;
    
    std::cout << "Creating ConnectionInfo directly..." << std::endl;
    
    // Create a ConnectionPolicy
    auto policy = std::make_shared<crow::ConnectionPolicy>();
    policy->maxRetryAttempts = 1;
    
    // Create ConnectionInfo directly (bypassing the pool)
    // This simulates a connection that could be destroyed independently
    globalConnection = std::make_shared<crow::ConnectionInfo>(
        ioc, "test-sub", policy,
        boost::urls::url_view("http://localhost:9999/"),
        ensuressl::VerifyCertificate::Verify, 0);
    
    std::cout << "Starting async operation..." << std::endl;
    
    // Start an async operation that will take some time
    boost::beast::http::request<boost::beast::http::string_body> req;
    req.method(boost::beast::http::verb::get);
    req.target("/");
    req.set(boost::beast::http::field::host, "localhost:9999");
    req.keep_alive(true);
    
    // Set up a callback that will be invoked when response is received
    globalConnection->callback = [](bool keepAlive, uint32_t connId, crow::Response& res) {
        std::cout << "Callback invoked!" << std::endl;
        std::cout << "Response code: " << res.resultInt() << std::endl;
        
        // At this point, if ConnectionInfo was destroyed, we'd crash
        // when trying to access connPolicy->invalidResp in afterRead
    };
    
    globalConnection->req = std::move(req);
    
    // Start the connection
    globalConnection->doResolve();
    
    // Process a few events to let DNS resolution start
    for (int i = 0; i < 5; ++i)
    {
        ioc.poll();
        if (ioc.stopped())
        {
            ioc.restart();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    std::cout << "Destroying ConnectionInfo while async operation is pending..." << std::endl;
    
    // This is the critical moment: destroy the ConnectionInfo while
    // async operations are still pending
    globalConnection.reset();
    
    std::cout << "ConnectionInfo destroyed. Processing remaining events..." << std::endl;
    
    // Continue processing events - if there's a bug, the callback will
    // fire with a dangling 'this' pointer and crash
    for (int i = 0; i < 100; ++i)
    {
        ioc.poll();
        if (ioc.stopped())
        {
            ioc.restart();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    // Cleanup
    crow::connections::systemBus = nullptr;
    
    std::cout << "Test completed" << std::endl;
}

int main()
{
    std::cout << "=== ConnectionInfo Destruction During Async Operation Test ===" << std::endl;
    std::cout << "This test attempts to reproduce the std::bad_function_call crash" << std::endl;
    std::cout << "by destroying ConnectionInfo while async operations are pending." << std::endl;
    std::cout << std::endl;
    
    try
    {
        testConnectionInfoDestructionDuringCallback();
        std::cout << "\nTest PASSED - No crash occurred" << std::endl;
        std::cout << "This means the connection is protected during callbacks" << std::endl;
        std::cout << "(or the timing didn't trigger the race condition)" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "\nTest FAILED with exception: " << e.what() << std::endl;
        std::cout << "This reproduces the production crash!" << std::endl;
        return 1;
    }
    
    return 0;
}

// Made with Bob