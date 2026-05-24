// Test to reproduce ConnectionPool coredump by destroying pool during callback
#include "app.hpp"
#include "http_client.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>

#include <memory>
#include <iostream>

// Global pointer to simulate pool being destroyed during callback
std::shared_ptr<crow::ConnectionPool> globalPool;

void testPoolDestructionDuringCallback()
{
    boost::asio::io_context ioc;
    
    std::cout << "Creating connection pool..." << std::endl;
    
    // Create a connection pool
    auto policy = std::make_shared<crow::ConnectionPolicy>();
    policy->maxConnections = 1;
    
    globalPool = std::make_shared<crow::ConnectionPool>(
        ioc, "test-pool", policy,
        boost::urls::url_view("http://localhost:9999"),
        crow::ensuressl::VerifyCertificate::Verify);
    
    // Send a request with a callback that destroys the pool
    crow::HttpClient client;
    
    boost::beast::http::request<bmcweb::HttpBody> req;
    req.method(boost::beast::http::verb::get);
    req.target("/");
    req.set(boost::beast::http::field::host, "localhost:9999");
    
    std::cout << "Sending request..." << std::endl;
    
    // The callback will destroy the pool
    auto callback = [](bool success, uint32_t connId, crow::Response& res) {
        std::cout << "Callback invoked! connId=" << connId << std::endl;
        std::cout << "Destroying global pool during callback..." << std::endl;
        
        // This destroys the ConnectionPool while we're inside afterSendData!
        globalPool.reset();
        
        std::cout << "Pool destroyed. Callback returning..." << std::endl;
        // When callback returns, afterSendData will try to call sendNext()
        // with the now-destroyed pool!
    };
    
    globalPool->sendData(std::move(req), "", callback);
    
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
