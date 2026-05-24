// HTTP Client Async Race Condition Test (True Single-Threaded)
// This test simulates bmcweb's actual single-threaded environment where
// objects are destroyed while async operations are still pending.
// Everything runs in the main thread, just like bmcweb.

#include "boost_formatters.hpp"
#include "dbus_singleton.hpp"
#include "http/http_client.hpp"
#include "ssl_key_handler.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/beast/http/field.hpp>
#include <boost/beast/http/verb.hpp>
#include <boost/url/url_view.hpp>
#include <sdbusplus/asio/connection.hpp>

#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

using namespace std::chrono_literals;

std::atomic<int> clientsCreated{0};
std::atomic<int> clientsDestroyed{0};
std::atomic<int> requestsSent{0};

// Test 1: Create client, send requests, destroy immediately
// This simulates the most common race: destruction before async ops complete
void testImmediateDestruction(boost::asio::io_context& ioc, int iterations)
{
    std::cout << "\n[TEST 1] Immediate Destruction After Send\n";
    std::cout << std::string(70, '=') << "\n";
    std::cout << "Creating clients, sending requests, destroying immediately\n";
    std::cout << "This tests if callbacks handle destroyed ConnectionPool correctly\n\n";

    auto policy = std::make_shared<crow::ConnectionPolicy>();
    policy->maxRetryAttempts = 0;  // No retries - fail fast
    policy->retryIntervalSecs = std::chrono::seconds(0);
    policy->maxConnections = 3;
    // Note: 1110 branch has hardcoded 30s timeout in http_client.hpp

    for (int i = 0; i < iterations; i++)
    {
        // Create client
        auto client = std::make_unique<crow::HttpClient>(ioc, policy);
        clientsCreated++;

        boost::beast::http::fields headers;
        headers.set(boost::beast::http::field::host, "192.0.2.1");

        // Send multiple requests to unreachable IPs
        // These will trigger async resolve operations
        for (int j = 0; j < 3; j++)
        {
            std::string ip = "192.0.2." + std::to_string((i * 3 + j) % 254 + 1);
            try
            {
                client->sendData(
                    "test data",
                    boost::urls::url_view("http://" + ip + ":8080/test"),
                    ensuressl::VerifyCertificate::NoVerify,
                    headers,
                    boost::beast::http::verb::post);
                requestsSent++;
            }
            catch (const std::exception& e)
            {
                // Ignore exceptions
            }
        }

        // Destroy immediately - async operations still pending
        client.reset();
        clientsDestroyed++;

        if ((i + 1) % 100 == 0)
        {
            std::cout << "  Progress: " << (i + 1) << "/" << iterations
                      << " (requests: " << requestsSent.load() << ")\n";
        }
    }

    std::cout << "\n  Test complete: " << clientsCreated.load() << " clients, "
              << requestsSent.load() << " requests\n";
}

// Test 2: Destroy client while io_context is processing callbacks
// This tests the actual async callback execution race
void testDestroyDuringCallbackProcessing(boost::asio::io_context& ioc, int iterations)
{
    std::cout << "\n[TEST 2] Destroy During Callback Processing\n";
    std::cout << std::string(70, '=') << "\n";
    std::cout << "Destroying clients while io_context processes pending callbacks\n";
    std::cout << "This tests if callbacks can handle mid-execution destruction\n\n";

    auto policy = std::make_shared<crow::ConnectionPolicy>();
    policy->maxRetryAttempts = 0;  // No retries - fail fast
    policy->retryIntervalSecs = std::chrono::seconds(0);
    policy->maxConnections = 2;
    // Note: 1110 branch has hardcoded 30s timeout in http_client.hpp

    int completed = 0;

    for (int i = 0; i < iterations; i++)
    {
        auto client = std::make_shared<crow::HttpClient>(ioc, policy);
        clientsCreated++;

        boost::beast::http::fields headers;
        
        // Send requests
        for (int j = 0; j < 5; j++)
        {
            std::string ip = "192.0.2." + std::to_string((i * 5 + j) % 254 + 1);
            try
            {
                client->sendDataWithCallback(
                    "data",
                    boost::urls::url_view("http://" + ip + ":8080/test"),
                    ensuressl::VerifyCertificate::NoVerify,
                    headers,
                    boost::beast::http::verb::post,
                    [](crow::Response&) {
                        // Response callback - may never be called
                    });
                requestsSent++;
            }
            catch (...) {}
        }

        // Schedule destruction after a tiny delay
        // This allows some async operations to start
        auto timer = std::make_shared<boost::asio::steady_timer>(ioc);
        timer->expires_after(1ms);
        timer->async_wait([client, timer, &completed](const boost::system::error_code&) mutable {
            // Client destroyed here when shared_ptr goes out of scope
            client.reset();
            completed++;
        });

        clientsDestroyed++;

        if ((i + 1) % 50 == 0)
        {
            std::cout << "  Progress: " << (i + 1) << "/" << iterations
                      << " (completed: " << completed << ")\n";
        }
    }

    std::cout << "\n  Test complete: " << clientsCreated.load() << " clients, "
              << requestsSent.load() << " requests\n";
}

// Test 3: Rapid create/destroy cycles
// This maximizes the chance of callbacks executing on destroyed objects
void testRapidCycles(boost::asio::io_context& ioc, int cycles)
{
    std::cout << "\n[TEST 3] Rapid Create/Destroy Cycles\n";
    std::cout << std::string(70, '=') << "\n";
    std::cout << "Creating and destroying multiple clients in rapid succession\n";
    std::cout << "This tests if the io_context queue handles destruction correctly\n\n";

    auto policy = std::make_shared<crow::ConnectionPolicy>();
    policy->maxRetryAttempts = 0;  // No retries - fail fast
    policy->maxConnections = 2;
    // Note: 1110 branch has hardcoded 30s timeout in http_client.hpp

    for (int cycle = 0; cycle < cycles; cycle++)
    {
        // Create multiple clients in one cycle
        for (int i = 0; i < 5; i++)
        {
            auto client = std::make_unique<crow::HttpClient>(ioc, policy);
            clientsCreated++;

            boost::beast::http::fields headers;
            
            // Send a few requests
            for (int j = 0; j < 2; j++)
            {
                std::string ip = "192.0.2." + std::to_string((cycle * 10 + i * 2 + j) % 254 + 1);
                try
                {
                    client->sendData(
                        "test",
                        boost::urls::url_view("http://" + ip + ":8080/test"),
                        ensuressl::VerifyCertificate::NoVerify,
                        headers,
                        boost::beast::http::verb::post);
                    requestsSent++;
                }
                catch (...) {}
            }

            // Destroy immediately
            client.reset();
            clientsDestroyed++;
        }

        if ((cycle + 1) % 20 == 0)
        {
            std::cout << "  Cycle: " << (cycle + 1) << "/" << cycles
                      << " (total requests: " << requestsSent.load() << ")\n";
        }
    }

    std::cout << "\n  Test complete: " << clientsCreated.load() << " clients, "
              << requestsSent.load() << " requests\n";
}

int main(int argc, char* argv[])
{
    int iterations = 500;

    if (argc > 1)
    {
        iterations = std::stoi(argv[1]);
    }

    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "HTTP Client Async Race Condition Test (Single-Threaded)\n";
    std::cout << std::string(70, '=') << "\n";
    std::cout << "This test simulates bmcweb's single-threaded async environment\n";
    std::cout << "where objects are destroyed while async operations are pending.\n";
    std::cout << "\nKey characteristics:\n";
    std::cout << "  - True single-threaded (main thread only)\n";
    std::cout << "  - All operations in one thread (like bmcweb)\n";
    std::cout << "  - Tests async callback execution order\n";
    std::cout << "  - Focuses on destruction timing, not concurrency\n";
    std::cout << std::string(70, '=') << "\n";

    boost::asio::io_context ioc;
    
    // Initialize D-Bus connection (required for async_resolve)
    // IMPORTANT: Must be created in the same thread that runs io_context
    std::cout << "\nInitializing D-Bus connection...\n";
    sdbusplus::asio::connection systemBusConn(ioc);
    crow::connections::systemBus = &systemBusConn;
    std::cout << "D-Bus connection initialized\n";

    try
    {
        // Run tests - they will post work to io_context
        std::cout << "\nRunning tests (single-threaded like bmcweb)...\n";
        testImmediateDestruction(ioc, iterations);
        
        // Process pending async operations with timeout
        std::cout << "\nProcessing async operations (max 3 seconds)...\n";
        auto deadline = std::chrono::steady_clock::now() + 3s;
        while (std::chrono::steady_clock::now() < deadline && !ioc.stopped())
        {
            ioc.poll_one();
        }
        ioc.restart();
        
        clientsCreated = 0;
        clientsDestroyed = 0;
        requestsSent = 0;
        testDestroyDuringCallbackProcessing(ioc, iterations / 2);
        
        // Process pending async operations with timeout
        std::cout << "\nProcessing async operations (max 3 seconds)...\n";
        deadline = std::chrono::steady_clock::now() + 3s;
        while (std::chrono::steady_clock::now() < deadline && !ioc.stopped())
        {
            ioc.poll_one();
        }
        ioc.restart();
        
        clientsCreated = 0;
        clientsDestroyed = 0;
        requestsSent = 0;
        testRapidCycles(ioc, iterations / 5);
        
        // Process pending async operations with timeout
        std::cout << "\nProcessing async operations (max 3 seconds)...\n";
        deadline = std::chrono::steady_clock::now() + 3s;
        while (std::chrono::steady_clock::now() < deadline && !ioc.stopped())
        {
            ioc.poll_one();
        }

        // Final processing with timeout
        std::cout << "\nFinal processing (max 5 seconds)...\n";
        deadline = std::chrono::steady_clock::now() + 5s;
        while (std::chrono::steady_clock::now() < deadline && !ioc.stopped())
        {
            ioc.poll_one();
        }
        
        // Stop any remaining operations
        ioc.stop();
        std::cout << "Stopped io_context\n";

        std::cout << "\n" << std::string(70, '=') << "\n";
        std::cout << "All tests completed!\n";
        std::cout << "\nIf no crash occurred:\n";
        std::cout << "  1. The fix may be working correctly\n";
        std::cout << "  2. The race condition is very timing-sensitive\n";
        std::cout << "  3. Try running with AddressSanitizer:\n";
        std::cout << "     meson configure -Db_sanitize=address && ninja\n";
        std::cout << "\nIf crash occurred:\n";
        std::cout << "  Check dmesg or journalctl for crash details\n";
        std::cout << "  Run with gdb to get backtrace:\n";
        std::cout << "     gdb ./http_client_async_race_test\n";
        std::cout << std::string(70, '=') << "\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "\nException caught: " << e.what() << "\n";
        crow::connections::systemBus = nullptr;
        return 1;
    }

    // Clean up D-Bus connection
    crow::connections::systemBus = nullptr;
    std::cout << "D-Bus connection cleaned up\n";

    std::cout << "\nTest program completed successfully\n";
    return 0;
}

// Made with Bob
