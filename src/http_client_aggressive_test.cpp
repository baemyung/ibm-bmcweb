// Aggressive HTTP Client Crash Test
// This test is designed to maximize the chance of triggering race conditions
// by creating and destroying clients as fast as possible while async operations
// are in flight.

#include "http/http_client.hpp"
#include "ssl_key_handler.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/beast/http/field.hpp>
#include <boost/beast/http/verb.hpp>
#include <boost/url/url_view.hpp>

#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

std::atomic<int> clientsCreated{0};
std::atomic<int> clientsDestroyed{0};

void aggressiveTest(boost::asio::io_context& ioc, int iterations)
{
    std::cout << "\n[AGGRESSIVE TEST] Creating and destroying clients rapidly\n";
    std::cout << std::string(70, '=') << "\n";

    auto policy = std::make_shared<crow::ConnectionPolicy>();
    policy->maxRetryAttempts = 3;  // Allow some retries to keep operations alive longer
    policy->retryIntervalSecs = std::chrono::seconds(1);
    policy->maxConnections = 4;

    for (int i = 0; i < iterations; i++)
    {
        // Create client
        auto client = std::make_unique<crow::HttpClient>(ioc, policy);
        clientsCreated++;

        boost::beast::http::fields headers;
        headers.set(boost::beast::http::field::host, "192.0.2.1");

        // Send multiple requests to different unreachable IPs
        // This creates multiple ConnectionInfo objects with active async operations
        for (int j = 0; j < 5; j++)
        {
            std::string ip = "192.0.2." + std::to_string((i * 5 + j) % 254 + 1);
            try
            {
                client->sendData(
                    "test data",
                    boost::urls::url_view("http://" + ip + ":8080/events"),
                    ensuressl::VerifyCertificate::NoVerify,
                    headers,
                    boost::beast::http::verb::post);
            }
            catch (const std::exception& e)
            {
                // Ignore exceptions
            }
        }

        // Destroy immediately - this is the critical race condition trigger
        // The async operations (resolve, connect, timers) are still in flight
        client.reset();
        clientsDestroyed++;

        // NO sleep - destroy as fast as possible to maximize race condition
        
        if ((i + 1) % 100 == 0)
        {
            std::cout << "  Progress: " << (i + 1) << "/" << iterations
                      << " (created: " << clientsCreated.load()
                      << ", destroyed: " << clientsDestroyed.load() << ")\n";
        }
    }

    std::cout << "\n  Test complete: created " << clientsCreated.load()
              << ", destroyed " << clientsDestroyed.load() << " clients\n";
}

void multiThreadedTest(boost::asio::io_context& ioc, int numThreads, int iterationsPerThread)
{
    std::cout << "\n[MULTI-THREADED TEST] " << numThreads << " threads, "
              << iterationsPerThread << " iterations each\n";
    std::cout << std::string(70, '=') << "\n";

    std::vector<std::thread> threads;
    
    for (int t = 0; t < numThreads; t++)
    {
        threads.emplace_back([&ioc, iterationsPerThread, t]() {
            auto policy = std::make_shared<crow::ConnectionPolicy>();
            policy->maxRetryAttempts = 3;
            policy->retryIntervalSecs = std::chrono::seconds(1);
            policy->maxConnections = 4;

            for (int i = 0; i < iterationsPerThread; i++)
            {
                auto client = std::make_unique<crow::HttpClient>(ioc, policy);
                clientsCreated++;

                boost::beast::http::fields headers;
                headers.set(boost::beast::http::field::host, "192.0.2.1");

                // Send to multiple IPs
                for (int j = 0; j < 3; j++)
                {
                    std::string ip = "192.0.2." + std::to_string((t * 100 + i * 3 + j) % 254 + 1);
                    try
                    {
                        client->sendData(
                            "test",
                            boost::urls::url_view("http://" + ip + ":8080/events"),
                            ensuressl::VerifyCertificate::NoVerify,
                            headers,
                            boost::beast::http::verb::post);
                    }
                    catch (...) {}
                }

                // Immediate destruction
                client.reset();
                clientsDestroyed++;
            }
        });
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

    std::cout << "  All threads complete: created " << clientsCreated.load()
              << ", destroyed " << clientsDestroyed.load() << " clients\n";
}

int main(int argc, char* argv[])
{
    int iterations = 1000;
    int numThreads = 4;

    if (argc > 1)
    {
        iterations = std::stoi(argv[1]);
    }
    if (argc > 2)
    {
        numThreads = std::stoi(argv[2]);
    }

    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "HTTP Client Aggressive Crash Test\n";
    std::cout << "This test attempts to trigger race conditions by:\n";
    std::cout << "  1. Creating clients with multiple pending async operations\n";
    std::cout << "  2. Destroying them immediately (no delays)\n";
    std::cout << "  3. Running multiple threads concurrently\n";
    std::cout << std::string(70, '=') << "\n";

    boost::asio::io_context ioc;
    
    // Run io_context in background thread
    std::thread ioThread([&ioc]() {
        std::cout << "IO thread started\n";
        ioc.run();
        std::cout << "IO thread stopped\n";
    });

    try
    {
        // Test 1: Single-threaded aggressive test
        aggressiveTest(ioc, iterations);
        
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        // Test 2: Multi-threaded test
        clientsCreated = 0;
        clientsDestroyed = 0;
        multiThreadedTest(ioc, numThreads, iterations / numThreads);

        std::cout << "\n" << std::string(70, '=') << "\n";
        std::cout << "All tests completed!\n";
        std::cout << "If no crash occurred, the race condition may be:\n";
        std::cout << "  1. Already fixed in this codebase\n";
        std::cout << "  2. Very timing-sensitive and hard to reproduce\n";
        std::cout << "  3. Only reproducible under specific conditions\n";
        std::cout << "\nCheck system logs:\n";
        std::cout << "  dmesg | tail -50\n";
        std::cout << "  journalctl -n 50 | grep -i 'segfault\\|core\\|crash'\n";
        std::cout << std::string(70, '=') << "\n\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    ioc.stop();
    if (ioThread.joinable())
    {
        ioThread.join();
    }

    return 0;
}

// Made with Bob
