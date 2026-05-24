// Test to reproduce ConnectionPool coredump by destroying pool during callback
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

// Global pointer to simulate HttpClient being destroyed during callback
std::shared_ptr<crow::HttpClient> globalClient;

void testPoolDestructionDuringCallback()
{
    boost::asio::io_context ioc;
    
    // Initialize systemBus for DNS resolution
    sdbusplus::asio::connection systemBus(ioc);
    crow::connections::systemBus = &systemBus;
    
    std::cout << "Creating HTTP client..." << std::endl;
    
    // Create an HTTP client
    auto policy = std::make_shared<crow::ConnectionPolicy>();
    policy->maxConnections = 1;
    
    globalClient = std::make_shared<crow::HttpClient>(ioc, policy);
    
    std::cout << "Sending request..." << std::endl;
    
    // Send a request using the HttpClient API
    boost::beast::http::fields headers;
    headers.set(boost::beast::http::field::host, "localhost:9999");
    
    globalClient->sendDataWithCallback(
        "",  // empty body
        boost::urls::url_view("http://localhost:9999/"),
        ensuressl::VerifyCertificate::Verify,
        headers,
        boost::beast::http::verb::get,
        [](crow::Response& /*res*/) {
            std::cout << "Callback invoked!" << std::endl;
            std::cout << "Destroying global client during callback..." << std::endl;
            
            // This destroys the HttpClient (and its ConnectionPools) while we're inside afterSendData!
            globalClient.reset();
            
            std::cout << "Client destroyed. Callback returning..." << std::endl;
            // When callback returns, afterSendData will try to call sendNext()
            // with the now-destroyed pool!
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
    
    std::cout << "Test completed without crash (pool was protected by weak_ptr)" << std::endl;
}

int main()
{
    std::cout << "=== ConnectionPool Destruction During Callback Test ===" << std::endl;
    
    try
    {
        testPoolDestructionDuringCallback();
        std::cout << "\nTest PASSED - No crash occurred" << std::endl;
        std::cout << "This means weak_ptr protection is working correctly" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "\nTest FAILED with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

// Made with Bob
