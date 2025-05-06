/****************************************************************
 *                 READ THIS WARNING FIRST
 * This is an auto-generated header which contains definitions
 * for Redfish DMTF defined messages.
 * DO NOT modify this registry outside of running the
 * parse_registries.py script.  The definitions contained within
 * this file are owned by DMTF.  Any modifications to these files
 * should be first pushed to the relevant registry in the DMTF
 * github organization.
 ***************************************************************/
#include "license_messages.hpp"

#include "error_message_utils.hpp"
#include "http_response.hpp"
#include "registries.hpp"
#include "registries/license_message_registry.hpp"

#include <boost/beast/http/status.hpp>
#include <nlohmann/json.hpp>

#include <array>
#include <cstddef>
#include <span>
#include <string_view>

// Clang can't seem to decide whether this header needs to be included or not,
// and is inconsistent.  Include it for now
// NOLINTNEXTLINE(misc-include-cleaner)
#include <utility>

namespace redfish
{

namespace messages
{

static nlohmann::json::object_t getLog(redfish::registries::License::Index name,
                                       std::span<const std::string_view> args)
{
    size_t index = static_cast<size_t>(name);
    if (index >= redfish::registries::License::registry.size())
    {
        return {};
    }
    return getLogFromRegistry(redfish::registries::License::header,
                              redfish::registries::License::registry, index,
                              args);
}

/**
 * @internal
 * @brief Formats LicenseInstalled message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t licenseInstalled(std::string_view arg1)
{
    return getLog(redfish::registries::License::Index::licenseInstalled,
                  std::to_array({arg1}));
}

void licenseInstalled(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::ok);
    addMessageToErrorJson(res.jsonValue, licenseInstalled(arg1));
}

/**
 * @internal
 * @brief Formats DaysBeforeExpiration message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t daysBeforeExpiration(std::string_view arg1,
                                              std::string_view arg2)
{
    return getLog(redfish::registries::License::Index::daysBeforeExpiration,
                  std::to_array({arg1, arg2}));
}

void daysBeforeExpiration(crow::Response& res, std::string_view arg1,
                          std::string_view arg2)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, daysBeforeExpiration(arg1, arg2));
}

/**
 * @internal
 * @brief Formats GracePeriod message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t gracePeriod(std::string_view arg1,
                                     std::string_view arg2)
{
    return getLog(redfish::registries::License::Index::gracePeriod,
                  std::to_array({arg1, arg2}));
}

void gracePeriod(crow::Response& res, std::string_view arg1,
                 std::string_view arg2)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, gracePeriod(arg1, arg2));
}

/**
 * @internal
 * @brief Formats Expired message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t expired(std::string_view arg1)
{
    return getLog(redfish::registries::License::Index::expired,
                  std::to_array({arg1}));
}

void expired(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, expired(arg1));
}

/**
 * @internal
 * @brief Formats InstallFailed message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t installFailed(std::string_view arg1)
{
    return getLog(redfish::registries::License::Index::installFailed,
                  std::to_array({arg1}));
}

void installFailed(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::internal_server_error);
    addMessageToErrorJson(res.jsonValue, installFailed(arg1));
}

/**
 * @internal
 * @brief Formats InvalidLicense message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t invalidLicense()
{
    return getLog(redfish::registries::License::Index::invalidLicense, {});
}

void invalidLicense(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, invalidLicense());
}

/**
 * @internal
 * @brief Formats NotApplicableToTarget message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t notApplicableToTarget()
{
    return getLog(redfish::registries::License::Index::notApplicableToTarget,
                  {});
}

void notApplicableToTarget(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, notApplicableToTarget());
}

/**
 * @internal
 * @brief Formats TargetsRequired message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t targetsRequired()
{
    return getLog(redfish::registries::License::Index::targetsRequired, {});
}

void targetsRequired(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, targetsRequired());
}

} // namespace messages
} // namespace redfish
