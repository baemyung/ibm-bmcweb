#include "http/http_client.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/beast/http/field.hpp>
#include <boost/beast/http/verb.hpp>
#include <boost/url/url_view.hpp>

#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace crow
{

// Test fixture for HTTP client coredump reproduction
class HttpClientCoredumpTest : public ::testing::Test
{
  protected:
    boost::asio::io_context ioc;
    std::shared_ptr<ConnectionPolicy> policy;

    void SetUp() override
    {
        policy = std::make_shared<ConnectionPolicy>();
        policy->maxRetryAttempts = 3;
        policy->requestTimeoutMs = 5000;
        policy->maxConnections = 4;
    }

    void TearDown() override
    {
        ioc.stop();
    }
};

// Test 1: Rapid client destruction while async operations are pending
TEST_F(HttpClientCoredumpTest, RapidClientDestruction)
{
    // Start io_context in a separate thread
    std::thread ioThread([this]() { ioc.run(); });

    const int iterations = 50;
    int successfulCreations = 0;

    for (int i = 0; i < iterations; i++)
    {
        // Create HttpClient
        auto client = std::make_unique<HttpClient>(
            ioc, "test-client-" + std::to_string(i), policy);

        // Send a request to a non-routable IP (triggers async operations)
        boost::beast::http::fields headers;
        headers.set(boost::beast::http::field::host, "192.0.2.1");

        try
        {
            client->sendData(
                "test data " + std::to_string(i),
                boost::urls::url_view("http://192.0.2.1:8080/test"),
                ensuressl::VerifyCertificate::VerifyNone, headers,
                boost::beast::http::verb::post,
                [i](const Response& /*res*/) {
                // Callback - may execute after client is destroyed
                // This is where the coredump would occur without fixes
            });

            successfulCreations++;
        }
        catch (const std::exception& e)
        {
            // Expected to fail for some iterations
            BMCWEB_LOG_DEBUG("Iteration {} failed: {}", i, e.what());
        }

        // Destroy client immediately while async operations are pending
        // This is the critical moment that triggers the coredump
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        client.reset();

        // Small delay between iterations
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    // Stop io_context and wait for thread
    ioc.stop();
    if (ioThread.joinable())
    {
        ioThread.join();
    }

    // Test passes if we didn't crash
    EXPECT_GT(successfulCreations, 0);
    BMCWEB_LOG_INFO("Test completed {} iterations without crash",
                    successfulCreations);
}

// Test 2: Multiple concurrent clients with rapid destruction
TEST_F(HttpClientCoredumpTest, ConcurrentClientsRapidDestruction)
{
    std::thread ioThread([this]() { ioc.run(); });

    const int numClients = 10;
    std::vector<std::unique_ptr<HttpClient>> clients;

    // Create multiple clients
    for (int i = 0; i < numClients; i++)
    {
        clients.push_back(std::make_unique<HttpClient>(
            ioc, "concurrent-client-" + std::to_string(i), policy));
    }

    // Send requests from all clients
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
                ensuressl::VerifyCertificate::VerifyNone, headers,
                boost::beast::http::verb::post, [i](const Response& /*res*/) {
                // Callback
            });
        }
        catch (const std::exception& e)
        {
            BMCWEB_LOG_DEBUG("Client {} send failed: {}", i, e.what());
        }
    }

    // Wait briefly for async operations to start
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Destroy all clients rapidly
    clients.clear();

    // Stop io_context
    ioc.stop();
    if (ioThread.joinable())
    {
        ioThread.join();
    }

    // Test passes if we didn't crash
    SUCCEED();
}

// Test 3: Client destruction during connection timeout
TEST_F(HttpClientCoredumpTest, DestructionDuringTimeout)
{
    std::thread ioThread([this]() { ioc.run(); });

    // Create client with short timeout
    auto shortTimeoutPolicy = std::make_shared<ConnectionPolicy>();
    shortTimeoutPolicy->requestTimeoutMs = 100; // 100ms timeout
    shortTimeoutPolicy->maxRetryAttempts = 1;

    auto client =
        std::make_unique<HttpClient>(ioc, "timeout-test-client",
                                     shortTimeoutPolicy);

    boost::beast::http::fields headers;
    headers.set(boost::beast::http::field::host, "192.0.2.1");

    bool callbackExecuted = false;

    try
    {
        client->sendData(
            "timeout test", boost::urls::url_view("http://192.0.2.1:8080/test"),
            ensuressl::VerifyCertificate::VerifyNone, headers,
            boost::beast::http::verb::post, [&callbackExecuted](const Response&
                                                                     /*res*/) {
            callbackExecuted = true;
        });
    }
    catch (const std::exception& e)
    {
        BMCWEB_LOG_DEBUG("Send failed: {}", e.what());
    }

    // Wait for timeout to be close to expiring
    std::this_thread::sleep_for(std::chrono::milliseconds(90));

    // Destroy client just before/during timeout
    client.reset();

    // Wait a bit more to ensure timeout would have fired
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    ioc.stop();
    if (ioThread.joinable())
    {
        ioThread.join();
    }

    // Test passes if we didn't crash
    // Callback may or may not have executed depending on timing
    SUCCEED();
}

// Test 4: Stress test with many rapid iterations
TEST_F(HttpClientCoredumpTest, StressTestRapidIterations)
{
    std::thread ioThread([this]() { ioc.run(); });

    const int iterations = 100;
    int completedIterations = 0;

    for (int i = 0; i < iterations; i++)
    {
        auto client = std::make_unique<HttpClient>(
            ioc, "stress-client-" + std::to_string(i), policy);

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
                ensuressl::VerifyCertificate::VerifyNone, headers,
                boost::beast::http::verb::post, [i](const Response& /*res*/) {
                // Callback
            });
        }
        catch (const std::exception& e)
        {
            // Expected failures
        }

        // Very short delay before destruction
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        client.reset();

        completedIterations++;

        // Progress indicator every 20 iterations
        if ((i + 1) % 20 == 0)
        {
            BMCWEB_LOG_DEBUG("Completed {} iterations", i + 1);
        }
    }

    ioc.stop();
    if (ioThread.joinable())
    {
        ioThread.join();
    }

    EXPECT_EQ(completedIterations, iterations);
    BMCWEB_LOG_INFO("Stress test completed {} iterations without crash",
                    completedIterations);
}

} // namespace crow

// Made with Bob
