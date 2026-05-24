#pragma once
#include "dbus_singleton.hpp"
#include "logging.hpp"

#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/basic_endpoint.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <sdbusplus/message.hpp>

#include <charconv>
#include <memory>

namespace async_resolve
{

inline bool endpointFromResolveTuple(const std::vector<uint8_t>& ipAddress,
                                     boost::asio::ip::tcp::endpoint& endpoint)
{
    if (ipAddress.size() == 4) // ipv4 address
    {
        BMCWEB_LOG_DEBUG("ipv4 address");
        boost::asio::ip::address_v4 ipv4Addr(
            {ipAddress[0], ipAddress[1], ipAddress[2], ipAddress[3]});
        endpoint.address(ipv4Addr);
    }
    else if (ipAddress.size() == 16) // ipv6 address
    {
        BMCWEB_LOG_DEBUG("ipv6 address");
        boost::asio::ip::address_v6 ipv6Addr(
            {ipAddress[0], ipAddress[1], ipAddress[2], ipAddress[3],
             ipAddress[4], ipAddress[5], ipAddress[6], ipAddress[7],
             ipAddress[8], ipAddress[9], ipAddress[10], ipAddress[11],
             ipAddress[12], ipAddress[13], ipAddress[14], ipAddress[15]});
        endpoint.address(ipv6Addr);
    }
    else
    {
        BMCWEB_LOG_ERROR("Resolve failed to fetch the IP address");
        return false;
    }
    return true;
}

class Resolver
{
  public:
    // unused io param used to keep interface identical to
    // boost::asio::tcp:::resolver
    explicit Resolver(boost::asio::io_context& /*io*/) {}

    ~Resolver() = default;

    Resolver(const Resolver&) = delete;
    Resolver(Resolver&&) = delete;
    Resolver& operator=(const Resolver&) = delete;
    Resolver& operator=(Resolver&&) = delete;

    using results_type = std::vector<boost::asio::ip::tcp::endpoint>;

    template <typename ResolveHandler>
    // This function is kept using snake case so that it is interoperable with
    // boost::asio::ip::tcp::resolver
    // NOLINTNEXTLINE(readability-identifier-naming)
    void async_resolve(std::string_view host, std::string_view port,
                       ResolveHandler&& handler)
    {
        BMCWEB_LOG_DEBUG("[TRACE 1] async_resolve called with host.size()={}, port.size()={}",
                         host.size(), port.size());
        
        // Convert string_views to strings immediately to avoid dangling references
        // when the caller's temporary objects (like boost::urls::url members) are destroyed
        std::string hostStr(host);
        BMCWEB_LOG_DEBUG("[TRACE 2] hostStr created: '{}'", hostStr);
        
        std::string portStr(port);
        BMCWEB_LOG_DEBUG("[TRACE 3] portStr created: '{}'", portStr);
        
        BMCWEB_LOG_DEBUG("[TRACE 4] Trying to resolve: {}:{}", hostStr, portStr);

        uint16_t portNum = 0;

        BMCWEB_LOG_DEBUG("[TRACE 5] About to parse port number");
        auto it = std::from_chars(portStr.data(), portStr.data() + portStr.size(), portNum);
        BMCWEB_LOG_DEBUG("[TRACE 6] Port parsed: {}", portNum);
        if (it.ec != std::errc())
        {
            BMCWEB_LOG_ERROR("[TRACE 7] Failed to get the Port");
            handler(std::make_error_code(std::errc::invalid_argument),
                    results_type{});

            return;
        }

        BMCWEB_LOG_DEBUG("[TRACE 8] About to call systemBus->async_method_call");
        
        // Check if systemBus is valid
        if (crow::connections::systemBus == nullptr)
        {
            BMCWEB_LOG_ERROR("[TRACE 8.1] ERROR: systemBus is NULL!");
            handler(std::make_error_code(std::errc::not_connected),
                    results_type{});
            return;
        }
        
        BMCWEB_LOG_DEBUG("[TRACE 8.2] systemBus pointer is valid: {}",
                         static_cast<void*>(crow::connections::systemBus));
        
        uint64_t flag = 0;
        crow::connections::systemBus->async_method_call(
            [hostStr, portNum,
             handler = std::forward<ResolveHandler>(handler)](
                const boost::system::error_code& ec,
                const std::vector<
                    std::tuple<int32_t, int32_t, std::vector<uint8_t>>>& resp,
                const std::string& hostName, const uint64_t flagNum) {
                BMCWEB_LOG_DEBUG("[TRACE 9] D-Bus callback invoked, ec={}", ec.message());
                results_type endpointList;
                if (ec)
                {
                    BMCWEB_LOG_ERROR("[TRACE 10] Resolve failed: {}", ec.message());
                    handler(ec, endpointList);
                    return;
                }
                BMCWEB_LOG_DEBUG("[TRACE 11] ResolveHostname returned: {}:{}", hostName,
                                 flagNum);
                // Extract the IP address from the response
                for (const std::tuple<int32_t, int32_t, std::vector<uint8_t>>&
                         resolveList : resp)
                {
                    boost::asio::ip::tcp::endpoint endpoint;
                    endpoint.port(portNum);
                    if (!endpointFromResolveTuple(std::get<2>(resolveList),
                                                  endpoint))
                    {
                        boost::system::error_code ecErr = make_error_code(
                            boost::system::errc::address_not_available);
                        handler(ecErr, endpointList);
                    }
                    BMCWEB_LOG_DEBUG("resolved endpoint is : {}",
                                     endpoint.address().to_string());
                    endpointList.push_back(endpoint);
                }
                // All the resolved data is filled in the endpointList
                handler(ec, endpointList);
            },
            "org.freedesktop.resolve1", "/org/freedesktop/resolve1",
            "org.freedesktop.resolve1.Manager", "ResolveHostname", 0, hostStr,
            AF_UNSPEC, flag);
        BMCWEB_LOG_DEBUG("[TRACE 12] async_method_call setup complete, returning from async_resolve");
    }
};

} // namespace async_resolve
