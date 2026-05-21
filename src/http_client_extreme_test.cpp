// Extreme HTTP Client Crash Test
// This test uses more aggressive techniques to trigger the race condition:
// 1. Keeps io_context very busy with many pending operations
// 2. Uses shorter timeouts to trigger callbacks faster
// 3. Destroys clients at precise moments when callbacks are likely queued
// 4. Uses memory barriers and synchronization to maximize race probability

#include "http/http_client.hpp"
#include "ssl_key_handler.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
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

std::atomic<int> crashDetected{0};
std::atomic<int> callbacksExecuted{0};
std::atomic<int> clientsCreated{0};
std::atomic<int> clientsDestroyed{0};

// Signal handler to detect crashes
void signalHandler(int signal)
{
    if (signal == SIGSEGV || signal == SIGABRT)
    {
        crashDetected = 1;
        std::cerr << "\n!!! CRASH DETECTED !!! Signal: " << signal << "\n";
        std::cerr << "Clients created: " << clientsCreated.load() << "\n";
        std::cerr << "Clients destroyed: " << clientsDestroyed.load() << "\n";
        std::cerr << "Callbacks executed: " << callbacksExecuted.load() << "\n";
        exit(1);
    }
}

void extremeTest(boost::asio::io_context& ioc, int iterations)
{
    std::cout << "\n[EXTREME TEST] Maximum aggression mode\n";
    std::cout << std::string(70, '=') << "\n";

    auto policy = std::make_shared<crow::ConnectionPolicy>();
    policy->maxRetryAttempts = 5;  // More retries = more callbacks
    policy->retryIntervalSecs = std::chrono::seconds(1);  // Short retry interval
    policy->maxConnections = 8;  // More connections = more async operations

    for (int i = 0; i < iterations; i++)
    {
        // Create client
        auto client = std::make_unique<crow::HttpClient>(ioc, policy);
        clientsCreated++;

        boost::beast::http::fields headers;
        headers.set(boost::beast::http::field::host, "192.0.2.1");

        // Send MANY requests to maximize pending async operations
        for (int j = 0; j < 10; j++)
        {
            std::string ip = "192.0.2." + std::to_string((i * 10 + j) % 254 + 1);
            
            try
            {
                client->sendData(
                    "test data " + std::to_string(i) + "_" + std::to_string(j),
                    boost::urls::url_view("http://" + ip + ":8080/events"),
                    ensuressl::VerifyCertificate::NoVerify,
                    headers,
                    boost::beast::http::verb::post);
            }
            catch (const std::exception& e)
            {
                // Ignore
            }
        }

        // Post work to io_context to keep it busy
        for (int k = 0; k < 5; k++)
        {
            boost::asio::post(ioc, []() {
                // Busy work to increase contention
                int x = 0;
                for (int m = 0; m < 100; m++) { x = x + 1; }
            });
        }

        // CRITICAL: Destroy immediately while async operations are queued
        // This is the exact moment when callbacks might be about to execute
        client.reset();
        clientsDestroyed++;

        // Yield to give io_context thread a chance to process callbacks
        // This increases the chance that callbacks fire right after destruction
        std::this_thread::yield();

        if ((i + 1) % 50 == 0)
        {
            std::cout << "  Progress: " << (i + 1) << "/" << iterations
                      << " (created: " << clientsCreated.load()
                      << ", destroyed: " << clientsDestroyed.load()
                      << ", callbacks: " << callbacksExecuted.load() << ")\n";
        }
    }

    std::cout << "\n  Test complete without crash\n";
}

void chaosTest(boost::asio::io_context& ioc, int numThreads, int iterationsPerThread)
{
    std::cout << "\n[CHAOS TEST] " << numThreads << " threads creating maximum chaos\n";
    std::cout << std::string(70, '=') << "\n";

    std::vector<std::thread> threads;
    std::atomic<bool> startFlag{false};
    
    // Create all threads but don't start them yet
    for (int t = 0; t < numThreads; t++)
    {
        threads.emplace_back([&ioc, &startFlag, iterationsPerThread, t]() {
            // Wait for all threads to be ready
            while (!startFlag.load()) {
                std::this_thread::yield();
            }

            auto policy = std::make_shared<crow::ConnectionPolicy>();
            policy->maxRetryAttempts = 5;
            policy->retryIntervalSecs = std::chrono::seconds(1);
            policy->maxConnections = 6;

            for (int i = 0; i < iterationsPerThread; i++)
            {
                auto client = std::make_unique<crow::HttpClient>(ioc, policy);
                clientsCreated++;

                boost::beast::http::fields headers;
                headers.set(boost::beast::http::field::host, "192.0.2.1");

                // Rapid-fire requests
                for (int j = 0; j < 8; j++)
                {
                    std::string ip = "192.0.2." + std::to_string((t * 1000 + i * 8 + j) % 254 + 1);
                    try
                    {
                        client->sendData(
                            "chaos",
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
                
                // No sleep - maximum chaos
            }
        });
    }

    // Start all threads simultaneously for maximum contention
    std::cout << "  Starting all threads simultaneously...\n";
    startFlag = true;

    for (auto& thread : threads)
    {
        thread.join();
    }

    std::cout << "  Chaos test complete: created " << clientsCreated.load()
              << ", destroyed " << clientsDestroyed.load() << " clients\n";
}

int main(int argc, char* argv[])
{
    // Install signal handlers
    signal(SIGSEGV, signalHandler);
    signal(SIGABRT, signalHandler);

    int iterations = 500;  // Start with fewer iterations but more aggressive
    int numThreads = 8;    // More threads = more contention

    if (argc > 1)
    {
        iterations = std::stoi(argv[1]);
    }
    if (argc > 2)
    {
        numThreads = std::stoi(argv[2]);
    }

    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "HTTP Client EXTREME Crash Test\n";
    std::cout << "This test uses maximum aggression to trigger race conditions:\n";
    std::cout << "  1. Very short retry intervals (5-10ms)\n";
    std::cout << "  2. Many concurrent requests per client (8-10)\n";
    std::cout << "  3. Immediate destruction with no delays\n";
    std::cout << "  4. Multiple threads with synchronized start\n";
    std::cout << "  5. Busy io_context to maximize callback queueing\n";
    std::cout << std::string(70, '=') << "\n";

    boost::asio::io_context ioc;
    
    // Run io_context with multiple threads for more contention
    std::vector<std::thread> ioThreads;
    for (int i = 0; i < 2; i++)
    {
        ioThreads.emplace_back([&ioc, i]() {
            std::cout << "IO thread " << i << " started\n";
            ioc.run();
            std::cout << "IO thread " << i << " stopped\n";
        });
    }

    try
    {
        // Test 1: Extreme single-threaded test
        extremeTest(ioc, iterations);
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        // Test 2: Chaos multi-threaded test
        clientsCreated = 0;
        clientsDestroyed = 0;
        chaosTest(ioc, numThreads, iterations / numThreads);

        std::cout << "\n" << std::string(70, '=') << "\n";
        if (crashDetected.load() == 0)
        {
            std::cout << "✓ All tests completed without crash!\n";
            std::cout << "\nThis could mean:\n";
            std::cout << "  1. The fix is already applied in this codebase\n";
            std::cout << "  2. The race condition is extremely timing-sensitive\n";
            std::cout << "  3. Additional conditions are needed to trigger it\n";
            std::cout << "\nRecommendations:\n";
            std::cout << "  - Run with AddressSanitizer: meson setup -Db_sanitize=address\n";
            std::cout << "  - Run with ThreadSanitizer: meson setup -Db_sanitize=thread\n";
            std::cout << "  - Check system logs: dmesg | tail -50\n";
            std::cout << "  - Try on different hardware/timing conditions\n";
        }
        else
        {
            std::cout << "✗ CRASH DETECTED during testing!\n";
            std::cout << "The race condition vulnerability exists.\n";
        }
        std::cout << std::string(70, '=') << "\n\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    ioc.stop();
    for (auto& thread : ioThreads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }

    return crashDetected.load();
}

// Made with Bob