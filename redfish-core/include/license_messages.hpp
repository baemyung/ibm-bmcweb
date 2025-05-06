#pragma once
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
// These generated headers are a superset of what is needed.
// clang sees them as an error, so ignore
// NOLINTBEGIN(misc-include-cleaner)
#include "http_response.hpp"

#include <boost/url/url_view_base.hpp>
#include <nlohmann/json.hpp>

#include <cstdint>
#include <source_location>
#include <string_view>
// NOLINTEND(misc-include-cleaner)

namespace redfish
{

namespace messages
{
/**
 * @brief Formats LicenseInstalled message into JSON
 * Message body: "The license <arg1> was installed."
 *
 * @param[in] arg1 Parameter of message that will replace %1 in its body.
 *
 * @returns Message LicenseInstalled formatted to JSON */
nlohmann::json::object_t licenseInstalled(std::string_view arg1);

void licenseInstalled(crow::Response& res, std::string_view arg1);

/**
 * @brief Formats DaysBeforeExpiration message into JSON
 * Message body: "The license <arg1> will expire in <arg2> days."
 *
 * @param[in] arg1 Parameter of message that will replace %1 in its body.
 * @param[in] arg2 Parameter of message that will replace %2 in its body.
 *
 * @returns Message DaysBeforeExpiration formatted to JSON */
nlohmann::json::object_t daysBeforeExpiration(std::string_view arg1,
                                              std::string_view arg2);

void daysBeforeExpiration(crow::Response& res, std::string_view arg1,
                          std::string_view arg2);

/**
 * @brief Formats GracePeriod message into JSON
 * Message body: "The license <arg1> has expired, <arg2> day grace period before
 * licensed functionality is disabled."
 *
 * @param[in] arg1 Parameter of message that will replace %1 in its body.
 * @param[in] arg2 Parameter of message that will replace %2 in its body.
 *
 * @returns Message GracePeriod formatted to JSON */
nlohmann::json::object_t gracePeriod(std::string_view arg1,
                                     std::string_view arg2);

void gracePeriod(crow::Response& res, std::string_view arg1,
                 std::string_view arg2);

/**
 * @brief Formats Expired message into JSON
 * Message body: "The license <arg1> has expired."
 *
 * @param[in] arg1 Parameter of message that will replace %1 in its body.
 *
 * @returns Message Expired formatted to JSON */
nlohmann::json::object_t expired(std::string_view arg1);

void expired(crow::Response& res, std::string_view arg1);

/**
 * @brief Formats InstallFailed message into JSON
 * Message body: "Failed to install the license.  Reason: <arg1>."
 *
 * @param[in] arg1 Parameter of message that will replace %1 in its body.
 *
 * @returns Message InstallFailed formatted to JSON */
nlohmann::json::object_t installFailed(std::string_view arg1);

void installFailed(crow::Response& res, std::string_view arg1);

/**
 * @brief Formats InvalidLicense message into JSON
 * Message body: "The content of the license was not recognized, is corrupted,
 * or is invalid."
 *
 *
 * @returns Message InvalidLicense formatted to JSON */
nlohmann::json::object_t invalidLicense();

void invalidLicense(crow::Response& res);

/**
 * @brief Formats NotApplicableToTarget message into JSON
 * Message body: "The license is not applicable to the target."
 *
 *
 * @returns Message NotApplicableToTarget formatted to JSON */
nlohmann::json::object_t notApplicableToTarget();

void notApplicableToTarget(crow::Response& res);

/**
 * @brief Formats TargetsRequired message into JSON
 * Message body: "The license requires targets to be specified."
 *
 *
 * @returns Message TargetsRequired formatted to JSON */
nlohmann::json::object_t targetsRequired();

void targetsRequired(crow::Response& res);

} // namespace messages
} // namespace redfish
