/*
 * HTTP Client Async Race Condition Test
 * 
 * This test simulates the actual async race condition in a single-threaded
 * environment (like bmcweb). The issue is NOT multi-threading, but rather
 * the order of async callback execution when objects are destroyed.
 */

#include "http/http_client.hpp"
#include "logging.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>

#include <chrono>
#include <iostream>
#include <memory>

using namespace std::chrono_literals;

// Simple HTTP server simulator that responds after a delay
class MockServer
{
  public:
    explicit MockServer(boost::asio::io_context& ioc) : timer(ioc) {}

    void simulateResponse(std::function<void()> callback)
    {
        // Simulate network delay
        timer.expires_after(10ms);
        timer.async_wait([callback](const boost::system::error_code& ec) {
            if (!ec)
            {
                callback();
            }
        });
    }

  private:
    boost::asio::steady_timer timer;
};

// Test scenario: Create HttpClient, send request, destroy HttpClient
// before response arrives
void testAsyncRaceCondition()
{
    std::cout << "\n=== Test: Async Race Condition ===\n";
    
    boost::asio::io_context ioc;
    
    // Create connection policy
    auto policy = std::make_shared<crow::ConnectionPolicy>();
    policy->maxRetryAttempts = 1;
    policy->retryIntervalSecs = std::chrono::seconds(0);
    
    int iteration = 0;
    int maxIterations = 1000;
    
    std::function<void()> runTest;
    runTest = [&]() {
        if (iteration >= maxIterations)
        {
            std::cout << "Test completed successfully after " << iteration 
                      << " iterations\n";
            return;
        }
        
        iteration++;
        if (iteration % 100 == 0)
        {
            std::cout << "Iteration " << iteration << "...\n";
        }
        
        // Create HttpClient
        auto client = std::make_unique<crow::HttpClient>(ioc, policy);
        
        // Send a request
        boost::beast::http::fields headers;
        headers.set(boost::beast::http::field::content_type, "application/json");
        
        try
        {
            client->sendData(
                R"({"test": "data"})",
                boost::urls::url_view("http://localhost:8080/test"),
                crow::ensuressl::VerifyCertificate::NoVerify,
                headers,
                boost::beast::http::verb::post
            );
        }
        catch (const std::exception& e)
        {
            std::cerr << "Exception in sendData: " << e.what() << "\n";
        }
        
        // Immediately destroy the client (before response arrives)
        // This simulates the race condition
        client.reset();
        
        // Schedule next iteration
        auto nextTimer = std::make_shared<boost::asio::steady_timer>(ioc);
        nextTimer->expires_after(1ms);
        nextTimer->async_wait([&runTest, nextTimer](const boost::system::error_code&) {
            runTest();
        });
    };
    
    // Start the test
    runTest();
    
    // Run the io_context
    try
    {
        ioc.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception in io_context: " << e.what() << "\n";
        throw;
    }
}

// Test scenario: Multiple rapid create/destroy cycles
void testRapidCreateDestroy()
{
    std::cout << "\n=== Test: Rapid Create/Destroy ===\n";
    
    boost::asio::io_context ioc;
    auto policy = std::make_shared<crow::ConnectionPolicy>();
    
    int iteration = 0;
    int maxIterations = 500;
    
    std::function<void()> runTest;
    runTest = [&]() {
        if (iteration >= maxIterations)
        {
            std::cout << "Test completed successfully after " << iteration 
                      << " iterations\n";
            return;
        }
        
        iteration++;
        if (iteration % 50 == 0)
        {
            std::cout << "Iteration " << iteration << "...\n";
        }
        
        // Create multiple clients and destroy them immediately
        for (int i = 0; i < 5; i++)
        {
            auto client = std::make_unique<crow::HttpClient>(ioc, policy);
            
            boost::beast::http::fields headers;
            try
            {
                client->sendData(
                    R"({"test": "data"})",
                    boost::urls::url_view("http://localhost:8080/test"),
                    crow::ensuressl::VerifyCertificate::NoVerify,
                    headers,
                    boost::beast::http::verb::post
                );
            }
            catch (const std::exception& e)
            {
                // Ignore exceptions during shutdown
            }
            
            // Destroy immediately
            client.reset();
        }
        
        // Schedule next iteration
        auto nextTimer = std::make_shared<boost::asio::steady_timer>(ioc);
        nextTimer->expires_after(5ms);
        nextTimer->async_wait([&runTest, nextTimer](const boost::system::error_code&) {
            runTest();
        });
    };
    
    runTest();
    
    try
    {
        ioc.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception in io_context: " << e.what() << "\n";
        throw;
    }
}

// Test scenario: Destroy client while async operations are pending
void testDestroyDuringAsyncOps()
{
    std::cout << "\n=== Test: Destroy During Async Operations ===\n";
    
    boost::asio::io_context ioc;
    auto policy = std::make_shared<crow::ConnectionPolicy>();
    
    int iteration = 0;
    int maxIterations = 200;
    
    std::function<void()> runTest;
    runTest = [&]() {
        if (iteration >= maxIterations)
        {
            std::cout << "Test completed successfully after " << iteration 
                      << " iterations\n";
            return;
        }
        
        iteration++;
        if (iteration % 20 == 0)
        {
            std::cout << "Iteration " << iteration << "...\n";
        }
        
        auto client = std::make_shared<crow::HttpClient>(ioc, policy);
        
        // Send multiple requests
        boost::beast::http::fields headers;
        for (int i = 0; i < 10; i++)
        {
            try
            {
                client->sendDataWithCallback(
                    R"({"test": "data"})",
                    boost::urls::url_view("http://localhost:8080/test"),
                    crow::ensuressl::VerifyCertificate::NoVerify,
                    headers,
                    boost::beast::http::verb::post,
                    [](crow::Response& res) {
                        // Response handler
                        BMCWEB_LOG_DEBUG("Response received: {}", res.resultInt());
                    }
                );
            }
            catch (const std::exception& e)
            {
                // Ignore
            }
        }
        
        // Destroy client while requests are pending
        auto destroyTimer = std::make_shared<boost::asio::steady_timer>(ioc);
        destroyTimer->expires_after(2ms);
        destroyTimer->async_wait([client, destroyTimer](const boost::system::error_code&) mutable {
            // Client will be destroyed here when shared_ptr goes out of scope
            client.reset();
        });
        
        // Schedule next iteration
        auto nextTimer = std::make_shared<boost::asio::steady_timer>(ioc);
        nextTimer->expires_after(10ms);
        nextTimer->async_wait([&runTest, nextTimer](const boost::system::error_code&) {
            runTest();
        });
    };
    
    runTest();
    
    try
    {
        ioc.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception in io_context: " << e.what() << "\n";
        throw;
    }
}

int main()
{
    std::cout << "HTTP Client Async Race Condition Test\n";
    std::cout << "======================================\n";
    std::cout << "This test simulates async race conditions in a single-threaded\n";
    std::cout << "environment (like bmcweb) where objects are destroyed while\n";
    std::cout << "async operations are still pending.\n";
    
    try
    {
        testAsyncRaceCondition();
        testRapidCreateDestroy();
        testDestroyDuringAsyncOps();
        
        std::cout << "\n=== All Tests Passed ===\n";
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "\n=== Test Failed ===\n";
        std::cerr << "Exception: " << e.what() << "\n";
        return 1;
    }
}

// Made with Bob
