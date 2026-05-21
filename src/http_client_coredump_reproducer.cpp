// HTTP Client Coredump Reproducer
// Standalone program to reproduce HTTP client coredump issues
// Can be installed and run directly on BMC

#include "http/http_client.hpp"
#include "ssl_key_handler.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/beast/http/field.hpp>
#include <boost/beast/http/verb.hpp>
#include <boost/url/url_view.hpp>

#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

void printUsage(const char* programName)
{
    std::cout << "Usage: " << programName << " [OPTIONS]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --test <number>     Run specific test (1-4)\n";
    std::cout << "  --iterations <n>    Number of iterations (default: 50)\n";
    std::cout << "  --clients <n>       Number of concurrent clients (default: 10)\n";
    std::cout << "  --help              Show this help message\n\n";
    std::cout << "Tests:\n";
    std::cout << "  1: Rapid client destruction\n";
    std::cout << "  2: Concurrent clients with rapid destruction\n";
    std::cout << "  3: Client destruction during timeout\n";
    std::cout << "  4: Stress test with rapid iterations\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << programName << " --test 1 --iterations 100\n";
    std::cout << "  " << programName << " --test 2 --clients 20\n";
    std::cout << "  " << programName << " --test 4\n";
}

// Test 1: Rapid client destruction
void testRapidDestruction(boost::asio::io_context& ioc, int iterations)
{
    std::cout << "\n[TEST 1] Rapid Client Destruction (" << iterations
              << " iterations)\n";
    std::cout << std::string(70, '=') << "\n";

    auto policy = std::make_shared<crow::ConnectionPolicy>();
    policy->maxRetryAttempts = 3;
    policy->maxConnections = 4;

    int successfulCreations = 0;

    for (int i = 0; i < iterations; i++)
    {
        auto client = std::make_unique<crow::HttpClient>(ioc, policy);

        boost::beast::http::fields headers;
        headers.set(boost::beast::http::field::host, "192.0.2.1");

        try
        {
            client->sendData(
                "test data " + std::to_string(i),
                boost::urls::url_view("http://192.0.2.1:8080/test"),
                ensuressl::VerifyCertificate::NoVerify, headers,
                boost::beast::http::verb::post);

            successfulCreations++;
        }
        catch (const std::exception& e)
        {
            std::cout << "  Iteration " << i << " failed: " << e.what()
                      << "\n";
        }

        // Destroy immediately to trigger race condition
        client.reset();

        // Brief pause between iterations
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        if ((i + 1) % 10 == 0)
        {
            std::cout << "  Progress: " << (i + 1) << "/" << iterations
                      << " iterations\n";
        }
    }

    std::cout << "\n  Completed " << successfulCreations
              << " iterations without crash\n";
    std::cout << "✓ Test 1 complete\n";
}

// Test 2: Concurrent clients with rapid destruction
void testConcurrentClients(boost::asio::io_context& ioc, int numClients)
{
    std::cout << "\n[TEST 2] Concurrent Clients (" << numClients
              << " clients)\n";
    std::cout << std::string(70, '=') << "\n";

    auto policy = std::make_shared<crow::ConnectionPolicy>();
    policy->maxRetryAttempts = 3;
    policy->maxConnections = 4;

    std::vector<std::unique_ptr<crow::HttpClient>> clients;

    for (int i = 0; i < numClients; i++)
    {
        clients.push_back(std::make_unique<crow::HttpClient>(ioc, policy));
    }

    for (size_t i = 0; i < clients.size(); i++)
    {
        boost::beast::http::fields headers;
        headers.set(boost::beast::http::field::host,
                    "192.0.2." + std::to_string(i + 1));

        try
        {
            clients[i]->sendData(
                "concurrent test " + std::to_string(i),
                boost::urls::url_view("http://192.0.2." +
                                      std::to_string(i + 1) + ":8080/test"),
                ensuressl::VerifyCertificate::NoVerify, headers,
                boost::beast::http::verb::post);
        }
        catch (const std::exception& e)
        {
            std::cout << "  Client " << i << " send failed: " << e.what()
                      << "\n";
        }
    }

    // Destroy immediately after all sends to trigger race condition
    std::cout << "  Destroying all " << clients.size() << " clients immediately...\n";
    clients.clear();

    std::cout << "✓ Test 2 complete\n";
}

// Test 3: Client destruction during timeout
void testDestructionDuringTimeout(boost::asio::io_context& ioc)
{
    std::cout << "\n[TEST 3] Destruction During Timeout\n";
    std::cout << std::string(70, '=') << "\n";

    auto shortTimeoutPolicy = std::make_shared<crow::ConnectionPolicy>();
    shortTimeoutPolicy->maxRetryAttempts = 1;

    auto client = std::make_unique<crow::HttpClient>(ioc, shortTimeoutPolicy);

    boost::beast::http::fields headers;
    headers.set(boost::beast::http::field::host, "192.0.2.1");

    try
    {
        client->sendData(
            "timeout test", boost::urls::url_view("http://192.0.2.1:8080/test"),
            ensuressl::VerifyCertificate::NoVerify, headers,
            boost::beast::http::verb::post);
    }
    catch (const std::exception& e)
    {
        std::cout << "  Send failed: " << e.what() << "\n";
    }

    // Destroy immediately to trigger race condition
    std::cout << "  Destroying client immediately...\n";
    client.reset();

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    std::cout << "✓ Test 3 complete\n";
}

// Test 4: Stress test
void testStressTest(boost::asio::io_context& ioc, int iterations)
{
    std::cout << "\n[TEST 4] Stress Test (" << iterations << " iterations)\n";
    std::cout << std::string(70, '=') << "\n";

    auto policy = std::make_shared<crow::ConnectionPolicy>();
    policy->maxRetryAttempts = 3;
    policy->maxConnections = 4;

    int completedIterations = 0;

    for (int i = 0; i < iterations; i++)
    {
        auto client = std::make_unique<crow::HttpClient>(ioc, policy);

        boost::beast::http::fields headers;
        headers.set(boost::beast::http::field::host,
                    "192.0.2." + std::to_string((i % 254) + 1));

        try
        {
            client->sendData(
                "stress test " + std::to_string(i),
                boost::urls::url_view("http://192.0.2." +
                                      std::to_string((i % 254) + 1) +
                                      ":8080/test"),
                ensuressl::VerifyCertificate::NoVerify, headers,
                boost::beast::http::verb::post);
        }
        catch (const std::exception& e)
        {
            // Expected failures
        }

        // Destroy immediately to maximize race condition
        client.reset();

        completedIterations++;

        if ((i + 1) % 20 == 0)
        {
            std::cout << "  Progress: " << (i + 1) << "/" << iterations
                      << " iterations\n";
        }
    }

    std::cout << "\n  Completed " << completedIterations
              << " iterations without crash\n";
    std::cout << "✓ Test 4 complete\n";
}

int main(int argc, char* argv[])
{
    int testNumber = 0;
    int iterations = 50;
    int numClients = 10;

    // Parse command line arguments
    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h")
        {
            printUsage(argv[0]);
            return 0;
        }
        else if (arg == "--test" && i + 1 < argc)
        {
            testNumber = std::stoi(argv[++i]);
        }
        else if (arg == "--iterations" && i + 1 < argc)
        {
            iterations = std::stoi(argv[++i]);
        }
        else if (arg == "--clients" && i + 1 < argc)
        {
            numClients = std::stoi(argv[++i]);
        }
        else
        {
            std::cerr << "Unknown option: " << arg << "\n";
            printUsage(argv[0]);
            return 1;
        }
    }

    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "HTTP Client Coredump Reproducer\n";
    std::cout << std::string(70, '=') << "\n";

    boost::asio::io_context ioc;
    std::thread ioThread([&ioc]() { ioc.run(); });

    try
    {
        if (testNumber == 0)
        {
            // Run all tests
            testRapidDestruction(ioc, iterations);
            std::this_thread::sleep_for(std::chrono::seconds(1));

            testConcurrentClients(ioc, numClients);
            std::this_thread::sleep_for(std::chrono::seconds(1));

            testDestructionDuringTimeout(ioc);
            std::this_thread::sleep_for(std::chrono::seconds(1));

            testStressTest(ioc, iterations);
        }
        else
        {
            // Run specific test
            switch (testNumber)
            {
                case 1:
                    testRapidDestruction(ioc, iterations);
                    break;
                case 2:
                    testConcurrentClients(ioc, numClients);
                    break;
                case 3:
                    testDestructionDuringTimeout(ioc);
                    break;
                case 4:
                    testStressTest(ioc, iterations);
                    break;
                default:
                    std::cerr << "Invalid test number: " << testNumber << "\n";
                    printUsage(argv[0]);
                    ioc.stop();
                    if (ioThread.joinable())
                    {
                        ioThread.join();
                    }
                    return 1;
            }
        }

        std::cout << "\n" << std::string(70, '=') << "\n";
        std::cout << "All tests completed!\n";
        std::cout << "Check system logs for crashes or coredumps:\n";
        std::cout << "  dmesg | grep segfault\n";
        std::cout << "  journalctl | grep -i 'segmentation\\|core'\n";
        std::cout << std::string(70, '=') << "\n\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
        ioc.stop();
        if (ioThread.joinable())
        {
            ioThread.join();
        }
        return 1;
    }

    ioc.stop();
    if (ioThread.joinable())
    {
        ioThread.join();
    }

    return 0;
}

// Made with Bob
