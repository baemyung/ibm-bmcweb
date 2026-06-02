// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright OpenBMC Authors

#include "http/http_client.hpp"
#include "http/http_response.hpp"
#include "ssl_key_handler.hpp"

#include <boost/asio/buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/http/field.hpp>
#include <boost/beast/http/verb.hpp>
#include <boost/url/url_view.hpp>

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace
{

class SimpleHttpServer
{
  public:
    explicit SimpleHttpServer(boost::asio::io_context& ioc) :
        acceptor(ioc,
                 boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 0)),
        socket(ioc)
    {
        port = acceptor.local_endpoint().port();
    }

    void acceptAndRespondNTimes(std::size_t remainingResponses)
    {
        acceptor.async_accept(
            socket, [this, remainingResponses](
                        const boost::system::error_code& ec) mutable {
                if (ec)
                {
                    return;
                }

                static constexpr std::string_view response =
                    "HTTP/1.1 500 Internal Server Error\r\n"
                    "Content-Length: 0\r\n"
                    "Connection: close\r\n"
                    "\r\n";

                boost::asio::async_write(
                    socket, boost::asio::buffer(response),
                    [this, remainingResponses](const boost::system::error_code&,
                                               std::size_t) mutable {
                        boost::system::error_code ignoredEc;
                        socket.shutdown(
                            boost::asio::ip::tcp::socket::shutdown_both,
                            ignoredEc);
                        socket.close(ignoredEc);

                        if (remainingResponses > 1)
                        {
                            acceptAndRespondNTimes(remainingResponses - 1);
                        }
                    });
            });
    }

    uint16_t getPort() const
    {
        return port;
    }

  private:
    boost::asio::ip::tcp::acceptor acceptor;
    boost::asio::ip::tcp::socket socket;
    uint16_t port{};
};

class HttpClientCoredumpTest : public ::testing::Test
{
  protected:
    boost::asio::io_context ioc;
};

TEST_F(HttpClientCoredumpTest,
       QueuedRequestHitsEmptyInvalidRespAfterFirstCallbackMutatesState)
{
    SimpleHttpServer server(ioc);
    server.acceptAndRespondNTimes(2);

    auto policy = std::make_shared<crow::ConnectionPolicy>();
    policy->maxRetryAttempts = 0;
    policy->maxConnections = 1;

    auto client = std::make_unique<crow::HttpClient>(ioc, policy);

    boost::beast::http::fields headers;
    headers.set(boost::beast::http::field::host, "127.0.0.1");

    const std::string url =
        "http://127.0.0.1:" + std::to_string(server.getPort()) + "/";

    std::unique_ptr<crow::HttpClient>* clientPtr = &client;
    std::atomic<bool> firstCallbackInvoked{false};

    client->sendDataWithCallback(
        "", boost::urls::url_view(url), ensuressl::VerifyCertificate::NoVerify,
        headers, boost::beast::http::verb::get,
        [clientPtr, policy, &firstCallbackInvoked](crow::Response&) {
            firstCallbackInvoked.store(true);
            policy->invalidResp = {};
            clientPtr->reset();
        });

    client->sendDataWithCallback(
        "", boost::urls::url_view(url), ensuressl::VerifyCertificate::NoVerify,
        headers, boost::beast::http::verb::get, [](crow::Response&) {});

    EXPECT_THROW(
        {
            try
            {
                ioc.run();
            }
            catch (const std::bad_function_call&)
            {
                throw std::runtime_error(
                    "queued request hit empty invalidResp");
            }
        },
        std::runtime_error);
    EXPECT_TRUE(firstCallbackInvoked.load());
    EXPECT_EQ(client, nullptr);
}

} // namespace

// Made with Bob
