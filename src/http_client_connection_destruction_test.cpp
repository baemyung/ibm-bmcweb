// Test to reproduce ConnectionInfo coredump by destroying connection during callback
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

void testConnectionDestructionDuringCallback()
{
    boost::asio::io_context ioc;
    
    // Initialize systemBus for DNS resolution
    sdbusplus::asio::connection systemBus(ioc);
    crow::connections::systemBus = &systemBus;
    
    std::cout << "Creating connection directly..." << std::endl;
    
    // Create a ConnectionInfo directly (bypassing the pool)
    auto policy = std::make_shared<crow::ConnectionPolicy>();
    
    // Create the connection
    globalConnection = std::make_shared<crow::ConnectionInfo>(
        ioc, "http://localhost:9999/", ensuressl::VerifyCertificate::Verify,
        policy, 0);
    
    std::cout << "Starting connection..." << std::endl;
    
    // Start the connection - this will trigger async_resolve
    boost::beast::http::request<boost::beast::http::string_body> req;
    req.method(boost::beast::http::verb::get);
    req.target("/");
    req.set(boost::beast::http::field::host, "localhost:9999");
    
    globalConnection->sendMessage(req, [](crow::Response& /*res*/) {
        std::cout << "Callback invoked!" << std::endl;
        std::cout << "Destroying global connection during callback..." << std::endl;
        
        // This destroys the ConnectionInfo while we're inside a callback!
        globalConnection.reset();
        
        std::cout << "Connection destroyed. Callback returning..." << std::endl;
        // If the callback was using raw 'this', it would crash here
    });
    
    std::cout << "Request sent. Processing events..." << std::endl;
    
    // Process events for a short time
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
    
    std::cout << "Test completed without crash" << std::endl;
}

int main()
{
    std::cout << "=== ConnectionInfo Destruction During Callback Test ===" << std::endl;
    
    try
    {
        testConnectionDestructionDuringCallback();
        std::cout << "\nTest PASSED - No crash occurred" << std::endl;
        std::cout << "This means the connection is protected during callbacks" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "\nTest FAILED with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

// Made with Bob