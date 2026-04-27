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
#include "openbmc_messages.hpp"

#include "error_message_utils.hpp"
#include "http_response.hpp"
#include "registries.hpp"
#include "registries/openbmc_message_registry.hpp"

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

static nlohmann::json::object_t getLog(redfish::registries::Openbmc::Index name,
                                       std::span<const std::string_view> args)
{
    size_t index = static_cast<size_t>(name);
    if (index >= redfish::registries::Openbmc::registry.size())
    {
        return {};
    }
    return getLogFromRegistry(redfish::registries::Openbmc::header,
                              redfish::registries::Openbmc::registry, index,
                              args);
}

/**
 * @internal
 * @brief Formats ADDDCCorrectable message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t aDDDCCorrectable(
    std::string_view arg1, std::string_view arg2, std::string_view arg3,
    std::string_view arg4)
{
    return getLog(redfish::registries::Openbmc::Index::aDDDCCorrectable,
                  std::to_array({arg1, arg2, arg3, arg4}));
}

void aDDDCCorrectable(crow::Response& res, std::string_view arg1,
                      std::string_view arg2, std::string_view arg3,
                      std::string_view arg4)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          aDDDCCorrectable(arg1, arg2, arg3, arg4));
}

/**
 * @internal
 * @brief Formats AtScaleDebugConnected message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t atScaleDebugConnected(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::atScaleDebugConnected,
                  std::to_array({arg1}));
}

void atScaleDebugConnected(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, atScaleDebugConnected(arg1));
}

/**
 * @internal
 * @brief Formats AtScaleDebugConnectionFailed message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t atScaleDebugConnectionFailed()
{
    return getLog(
        redfish::registries::Openbmc::Index::atScaleDebugConnectionFailed, {});
}

void atScaleDebugConnectionFailed(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, atScaleDebugConnectionFailed());
}

/**
 * @internal
 * @brief Formats AtScaleDebugDisconnected message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t atScaleDebugDisconnected()
{
    return getLog(redfish::registries::Openbmc::Index::atScaleDebugDisconnected,
                  {});
}

void atScaleDebugDisconnected(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, atScaleDebugDisconnected());
}

/**
 * @internal
 * @brief Formats AtScaleDebugFeatureDisabled message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t atScaleDebugFeatureDisabled()
{
    return getLog(
        redfish::registries::Openbmc::Index::atScaleDebugFeatureDisabled, {});
}

void atScaleDebugFeatureDisabled(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, atScaleDebugFeatureDisabled());
}

/**
 * @internal
 * @brief Formats AtScaleDebugFeatureDisabledAtHardware message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t atScaleDebugFeatureDisabledAtHardware()
{
    return getLog(redfish::registries::Openbmc::Index::
                      atScaleDebugFeatureDisabledAtHardware,
                  {});
}

void atScaleDebugFeatureDisabledAtHardware(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          atScaleDebugFeatureDisabledAtHardware());
}

/**
 * @internal
 * @brief Formats AtScaleDebugFeatureEnabled message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t atScaleDebugFeatureEnabled()
{
    return getLog(
        redfish::registries::Openbmc::Index::atScaleDebugFeatureEnabled, {});
}

void atScaleDebugFeatureEnabled(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, atScaleDebugFeatureEnabled());
}

/**
 * @internal
 * @brief Formats AtScaleDebugFeatureEnabledAtHardware message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t atScaleDebugFeatureEnabledAtHardware()
{
    return getLog(redfish::registries::Openbmc::Index::
                      atScaleDebugFeatureEnabledAtHardware,
                  {});
}

void atScaleDebugFeatureEnabledAtHardware(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          atScaleDebugFeatureEnabledAtHardware());
}

/**
 * @internal
 * @brief Formats AtScaleDebugSpecialUserDisabled message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t atScaleDebugSpecialUserDisabled()
{
    return getLog(
        redfish::registries::Openbmc::Index::atScaleDebugSpecialUserDisabled,
        {});
}

void atScaleDebugSpecialUserDisabled(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, atScaleDebugSpecialUserDisabled());
}

/**
 * @internal
 * @brief Formats AtScaleDebugSpecialUserEnabled message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t atScaleDebugSpecialUserEnabled()
{
    return getLog(
        redfish::registries::Openbmc::Index::atScaleDebugSpecialUserEnabled,
        {});
}

void atScaleDebugSpecialUserEnabled(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, atScaleDebugSpecialUserEnabled());
}

/**
 * @internal
 * @brief Formats AuditLogEntry message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t auditLogEntry(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::auditLogEntry,
                  std::to_array({arg1}));
}

void auditLogEntry(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, auditLogEntry(arg1));
}

/**
 * @internal
 * @brief Formats AuditLogUsysConfig message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t auditLogUsysConfig(
    std::string_view arg1, std::string_view arg2, std::string_view arg3,
    std::string_view arg4, std::string_view arg5, std::string_view arg6,
    std::string_view arg7, std::string_view arg8)
{
    return getLog(
        redfish::registries::Openbmc::Index::auditLogUsysConfig,
        std::to_array({arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8}));
}

void auditLogUsysConfig(
    crow::Response& res, std::string_view arg1, std::string_view arg2,
    std::string_view arg3, std::string_view arg4, std::string_view arg5,
    std::string_view arg6, std::string_view arg7, std::string_view arg8)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(
        res.jsonValue,
        auditLogUsysConfig(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8));
}

/**
 * @internal
 * @brief Formats BIOSAttributesChanged message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t bIOSAttributesChanged()
{
    return getLog(redfish::registries::Openbmc::Index::bIOSAttributesChanged,
                  {});
}

void bIOSAttributesChanged(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, bIOSAttributesChanged());
}

/**
 * @internal
 * @brief Formats BIOSBoot message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t bIOSBoot()
{
    return getLog(redfish::registries::Openbmc::Index::bIOSBoot, {});
}

void bIOSBoot(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, bIOSBoot());
}

/**
 * @internal
 * @brief Formats BIOSFirmwarePanicReason message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t bIOSFirmwarePanicReason(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::bIOSFirmwarePanicReason,
                  std::to_array({arg1}));
}

void bIOSFirmwarePanicReason(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, bIOSFirmwarePanicReason(arg1));
}

/**
 * @internal
 * @brief Formats BIOSFirmwareRecoveryReason message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t bIOSFirmwareRecoveryReason(std::string_view arg1)
{
    return getLog(
        redfish::registries::Openbmc::Index::bIOSFirmwareRecoveryReason,
        std::to_array({arg1}));
}

void bIOSFirmwareRecoveryReason(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, bIOSFirmwareRecoveryReason(arg1));
}

/**
 * @internal
 * @brief Formats BIOSFirmwareResiliencyError message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t bIOSFirmwareResiliencyError(std::string_view arg1)
{
    return getLog(
        redfish::registries::Openbmc::Index::bIOSFirmwareResiliencyError,
        std::to_array({arg1}));
}

void bIOSFirmwareResiliencyError(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, bIOSFirmwareResiliencyError(arg1));
}

/**
 * @internal
 * @brief Formats BIOSPOSTCode message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t bIOSPOSTCode(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(redfish::registries::Openbmc::Index::bIOSPOSTCode,
                  std::to_array({arg1, arg2, arg3}));
}

void bIOSPOSTCode(crow::Response& res, std::string_view arg1,
                  std::string_view arg2, std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, bIOSPOSTCode(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats BIOSPOSTError message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t bIOSPOSTError(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::bIOSPOSTError,
                  std::to_array({arg1}));
}

void bIOSPOSTError(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, bIOSPOSTError(arg1));
}

/**
 * @internal
 * @brief Formats BIOSRecoveryComplete message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t bIOSRecoveryComplete()
{
    return getLog(redfish::registries::Openbmc::Index::bIOSRecoveryComplete,
                  {});
}

void bIOSRecoveryComplete(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, bIOSRecoveryComplete());
}

/**
 * @internal
 * @brief Formats BIOSRecoveryStart message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t bIOSRecoveryStart()
{
    return getLog(redfish::registries::Openbmc::Index::bIOSRecoveryStart, {});
}

void bIOSRecoveryStart(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, bIOSRecoveryStart());
}

/**
 * @internal
 * @brief Formats BMCBootReason message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t bMCBootReason(std::string_view arg1,
                                       std::string_view arg2)
{
    return getLog(redfish::registries::Openbmc::Index::bMCBootReason,
                  std::to_array({arg1, arg2}));
}

void bMCBootReason(crow::Response& res, std::string_view arg1,
                   std::string_view arg2)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, bMCBootReason(arg1, arg2));
}

/**
 * @internal
 * @brief Formats BMCFirmwarePanicReason message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t bMCFirmwarePanicReason(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::bMCFirmwarePanicReason,
                  std::to_array({arg1}));
}

void bMCFirmwarePanicReason(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, bMCFirmwarePanicReason(arg1));
}

/**
 * @internal
 * @brief Formats BMCFirmwareRecoveryReason message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t bMCFirmwareRecoveryReason(std::string_view arg1)
{
    return getLog(
        redfish::registries::Openbmc::Index::bMCFirmwareRecoveryReason,
        std::to_array({arg1}));
}

void bMCFirmwareRecoveryReason(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, bMCFirmwareRecoveryReason(arg1));
}

/**
 * @internal
 * @brief Formats BMCFirmwareResiliencyError message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t bMCFirmwareResiliencyError(std::string_view arg1)
{
    return getLog(
        redfish::registries::Openbmc::Index::bMCFirmwareResiliencyError,
        std::to_array({arg1}));
}

void bMCFirmwareResiliencyError(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, bMCFirmwareResiliencyError(arg1));
}

/**
 * @internal
 * @brief Formats BMCKernelPanic message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t bMCKernelPanic()
{
    return getLog(redfish::registries::Openbmc::Index::bMCKernelPanic, {});
}

void bMCKernelPanic(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, bMCKernelPanic());
}

/**
 * @internal
 * @brief Formats CPLDFirmwarePanicReason message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t cPLDFirmwarePanicReason(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::cPLDFirmwarePanicReason,
                  std::to_array({arg1}));
}

void cPLDFirmwarePanicReason(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, cPLDFirmwarePanicReason(arg1));
}

/**
 * @internal
 * @brief Formats CPLDFirmwareRecoveryReason message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t cPLDFirmwareRecoveryReason(std::string_view arg1)
{
    return getLog(
        redfish::registries::Openbmc::Index::cPLDFirmwareRecoveryReason,
        std::to_array({arg1}));
}

void cPLDFirmwareRecoveryReason(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, cPLDFirmwareRecoveryReason(arg1));
}

/**
 * @internal
 * @brief Formats CPLDFirmwareResiliencyError message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t cPLDFirmwareResiliencyError(std::string_view arg1)
{
    return getLog(
        redfish::registries::Openbmc::Index::cPLDFirmwareResiliencyError,
        std::to_array({arg1}));
}

void cPLDFirmwareResiliencyError(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, cPLDFirmwareResiliencyError(arg1));
}

/**
 * @internal
 * @brief Formats CPUError message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t cPUError(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::cPUError,
                  std::to_array({arg1}));
}

void cPUError(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, cPUError(arg1));
}

/**
 * @internal
 * @brief Formats CPUMismatch message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t cPUMismatch(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::cPUMismatch,
                  std::to_array({arg1}));
}

void cPUMismatch(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, cPUMismatch(arg1));
}

/**
 * @internal
 * @brief Formats CPUThermalTrip message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t cPUThermalTrip(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::cPUThermalTrip,
                  std::to_array({arg1}));
}

void cPUThermalTrip(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, cPUThermalTrip(arg1));
}

/**
 * @internal
 * @brief Formats ChassisIntrusionDetected message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t chassisIntrusionDetected()
{
    return getLog(redfish::registries::Openbmc::Index::chassisIntrusionDetected,
                  {});
}

void chassisIntrusionDetected(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, chassisIntrusionDetected());
}

/**
 * @internal
 * @brief Formats ChassisIntrusionReset message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t chassisIntrusionReset()
{
    return getLog(redfish::registries::Openbmc::Index::chassisIntrusionReset,
                  {});
}

void chassisIntrusionReset(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, chassisIntrusionReset());
}

/**
 * @internal
 * @brief Formats ComponentOverTemperature message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t componentOverTemperature(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::componentOverTemperature,
                  std::to_array({arg1}));
}

void componentOverTemperature(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, componentOverTemperature(arg1));
}

/**
 * @internal
 * @brief Formats DCPowerOff message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t dCPowerOff()
{
    return getLog(redfish::registries::Openbmc::Index::dCPowerOff, {});
}

void dCPowerOff(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, dCPowerOff());
}

/**
 * @internal
 * @brief Formats DCPowerOn message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t dCPowerOn()
{
    return getLog(redfish::registries::Openbmc::Index::dCPowerOn, {});
}

void dCPowerOn(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, dCPowerOn());
}

/**
 * @internal
 * @brief Formats DriveError message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t driveError(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::driveError,
                  std::to_array({arg1}));
}

void driveError(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, driveError(arg1));
}

/**
 * @internal
 * @brief Formats EventLogCleared message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t eventLogCleared()
{
    return getLog(redfish::registries::Openbmc::Index::eventLogCleared, {});
}

void eventLogCleared(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, eventLogCleared());
}

/**
 * @internal
 * @brief Formats FanInserted message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t fanInserted(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::fanInserted,
                  std::to_array({arg1}));
}

void fanInserted(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, fanInserted(arg1));
}

/**
 * @internal
 * @brief Formats FanRedundancyLost message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t fanRedundancyLost()
{
    return getLog(redfish::registries::Openbmc::Index::fanRedundancyLost, {});
}

void fanRedundancyLost(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, fanRedundancyLost());
}

/**
 * @internal
 * @brief Formats FanRedundancyRegained message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t fanRedundancyRegained()
{
    return getLog(redfish::registries::Openbmc::Index::fanRedundancyRegained,
                  {});
}

void fanRedundancyRegained(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, fanRedundancyRegained());
}

/**
 * @internal
 * @brief Formats FanRemoved message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t fanRemoved()
{
    return getLog(redfish::registries::Openbmc::Index::fanRemoved, {});
}

void fanRemoved(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, fanRemoved());
}

/**
 * @internal
 * @brief Formats FirmwareActivationCompleted message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t firmwareActivationCompleted(std::string_view arg1)
{
    return getLog(
        redfish::registries::Openbmc::Index::firmwareActivationCompleted,
        std::to_array({arg1}));
}

void firmwareActivationCompleted(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, firmwareActivationCompleted(arg1));
}

/**
 * @internal
 * @brief Formats FirmwareActivationFailed message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t firmwareActivationFailed(std::string_view arg1,
                                                  std::string_view arg2)
{
    return getLog(redfish::registries::Openbmc::Index::firmwareActivationFailed,
                  std::to_array({arg1, arg2}));
}

void firmwareActivationFailed(crow::Response& res, std::string_view arg1,
                              std::string_view arg2)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, firmwareActivationFailed(arg1, arg2));
}

/**
 * @internal
 * @brief Formats FirmwareActivationStarted message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t firmwareActivationStarted(std::string_view arg1)
{
    return getLog(
        redfish::registries::Openbmc::Index::firmwareActivationStarted,
        std::to_array({arg1}));
}

void firmwareActivationStarted(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, firmwareActivationStarted(arg1));
}

/**
 * @internal
 * @brief Formats FirmwareResiliencyError message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t firmwareResiliencyError(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::firmwareResiliencyError,
                  std::to_array({arg1}));
}

void firmwareResiliencyError(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, firmwareResiliencyError(arg1));
}

/**
 * @internal
 * @brief Formats FirmwareUpdateCompleted message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t firmwareUpdateCompleted(std::string_view arg1,
                                                 std::string_view arg2)
{
    return getLog(redfish::registries::Openbmc::Index::firmwareUpdateCompleted,
                  std::to_array({arg1, arg2}));
}

void firmwareUpdateCompleted(crow::Response& res, std::string_view arg1,
                             std::string_view arg2)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, firmwareUpdateCompleted(arg1, arg2));
}

/**
 * @internal
 * @brief Formats FirmwareUpdateFailed message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t firmwareUpdateFailed(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(redfish::registries::Openbmc::Index::firmwareUpdateFailed,
                  std::to_array({arg1, arg2, arg3}));
}

void firmwareUpdateFailed(crow::Response& res, std::string_view arg1,
                          std::string_view arg2, std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          firmwareUpdateFailed(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats FirmwareUpdateStaged message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t firmwareUpdateStaged(std::string_view arg1,
                                              std::string_view arg2)
{
    return getLog(redfish::registries::Openbmc::Index::firmwareUpdateStaged,
                  std::to_array({arg1, arg2}));
}

void firmwareUpdateStaged(crow::Response& res, std::string_view arg1,
                          std::string_view arg2)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, firmwareUpdateStaged(arg1, arg2));
}

/**
 * @internal
 * @brief Formats FirmwareUpdateStarted message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t firmwareUpdateStarted(std::string_view arg1,
                                               std::string_view arg2)
{
    return getLog(redfish::registries::Openbmc::Index::firmwareUpdateStarted,
                  std::to_array({arg1, arg2}));
}

void firmwareUpdateStarted(crow::Response& res, std::string_view arg1,
                           std::string_view arg2)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, firmwareUpdateStarted(arg1, arg2));
}

/**
 * @internal
 * @brief Formats GeneralFirmwareSecurityViolation message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t generalFirmwareSecurityViolation(std::string_view arg1)
{
    return getLog(
        redfish::registries::Openbmc::Index::generalFirmwareSecurityViolation,
        std::to_array({arg1}));
}

void generalFirmwareSecurityViolation(crow::Response& res,
                                      std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          generalFirmwareSecurityViolation(arg1));
}

/**
 * @internal
 * @brief Formats GuardRecord message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t guardRecord(std::string_view arg1,
                                     std::string_view arg2)
{
    return getLog(redfish::registries::Openbmc::Index::guardRecord,
                  std::to_array({arg1, arg2}));
}

void guardRecord(crow::Response& res, std::string_view arg1,
                 std::string_view arg2)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, guardRecord(arg1, arg2));
}

/**
 * @internal
 * @brief Formats HardwareIsolationReason message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t hardwareIsolationReason(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::hardwareIsolationReason,
                  std::to_array({arg1}));
}

void hardwareIsolationReason(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, hardwareIsolationReason(arg1));
}

/**
 * @internal
 * @brief Formats IPMIWatchdog message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t iPMIWatchdog(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::iPMIWatchdog,
                  std::to_array({arg1}));
}

void iPMIWatchdog(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, iPMIWatchdog(arg1));
}

/**
 * @internal
 * @brief Formats IntelUPILinkWidthReducedToHalf message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t intelUPILinkWidthReducedToHalf(std::string_view arg1)
{
    return getLog(
        redfish::registries::Openbmc::Index::intelUPILinkWidthReducedToHalf,
        std::to_array({arg1}));
}

void intelUPILinkWidthReducedToHalf(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, intelUPILinkWidthReducedToHalf(arg1));
}

/**
 * @internal
 * @brief Formats IntelUPILinkWidthReducedToQuarter message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t intelUPILinkWidthReducedToQuarter(
    std::string_view arg1)
{
    return getLog(
        redfish::registries::Openbmc::Index::intelUPILinkWidthReducedToQuarter,
        std::to_array({arg1}));
}

void intelUPILinkWidthReducedToQuarter(crow::Response& res,
                                       std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          intelUPILinkWidthReducedToQuarter(arg1));
}

/**
 * @internal
 * @brief Formats InvalidLoginAttempted message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t invalidLoginAttempted(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::invalidLoginAttempted,
                  std::to_array({arg1}));
}

void invalidLoginAttempted(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, invalidLoginAttempted(arg1));
}

/**
 * @internal
 * @brief Formats InvalidUpload message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t invalidUpload(std::string_view arg1,
                                       std::string_view arg2)
{
    return getLog(redfish::registries::Openbmc::Index::invalidUpload,
                  std::to_array({arg1, arg2}));
}

void invalidUpload(crow::Response& res, std::string_view arg1,
                   std::string_view arg2)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, invalidUpload(arg1, arg2));
}

/**
 * @internal
 * @brief Formats InventoryAdded message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t inventoryAdded(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(redfish::registries::Openbmc::Index::inventoryAdded,
                  std::to_array({arg1, arg2, arg3}));
}

void inventoryAdded(crow::Response& res, std::string_view arg1,
                    std::string_view arg2, std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, inventoryAdded(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats InventoryRemoved message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t inventoryRemoved(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(redfish::registries::Openbmc::Index::inventoryRemoved,
                  std::to_array({arg1, arg2, arg3}));
}

void inventoryRemoved(crow::Response& res, std::string_view arg1,
                      std::string_view arg2, std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, inventoryRemoved(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats LanLost message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t lanLost(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::lanLost,
                  std::to_array({arg1}));
}

void lanLost(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, lanLost(arg1));
}

/**
 * @internal
 * @brief Formats LanRegained message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t lanRegained(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::lanRegained,
                  std::to_array({arg1}));
}

void lanRegained(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, lanRegained(arg1));
}

/**
 * @internal
 * @brief Formats LegacyPCIPERR message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t legacyPCIPERR(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(redfish::registries::Openbmc::Index::legacyPCIPERR,
                  std::to_array({arg1, arg2, arg3}));
}

void legacyPCIPERR(crow::Response& res, std::string_view arg1,
                   std::string_view arg2, std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, legacyPCIPERR(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats LegacyPCISERR message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t legacyPCISERR(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(redfish::registries::Openbmc::Index::legacyPCISERR,
                  std::to_array({arg1, arg2, arg3}));
}

void legacyPCISERR(crow::Response& res, std::string_view arg1,
                   std::string_view arg2, std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, legacyPCISERR(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats MEAutoConfigFailed message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t mEAutoConfigFailed(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::mEAutoConfigFailed,
                  std::to_array({arg1}));
}

void mEAutoConfigFailed(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, mEAutoConfigFailed(arg1));
}

/**
 * @internal
 * @brief Formats MEAutoConfigSuccess message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t mEAutoConfigSuccess(
    std::string_view arg1, std::string_view arg2, std::string_view arg3,
    std::string_view arg4)
{
    return getLog(redfish::registries::Openbmc::Index::mEAutoConfigSuccess,
                  std::to_array({arg1, arg2, arg3, arg4}));
}

void mEAutoConfigSuccess(crow::Response& res, std::string_view arg1,
                         std::string_view arg2, std::string_view arg3,
                         std::string_view arg4)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          mEAutoConfigSuccess(arg1, arg2, arg3, arg4));
}

/**
 * @internal
 * @brief Formats MEBootGuardHealthEvent message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t mEBootGuardHealthEvent(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::mEBootGuardHealthEvent,
                  std::to_array({arg1}));
}

void mEBootGuardHealthEvent(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, mEBootGuardHealthEvent(arg1));
}

/**
 * @internal
 * @brief Formats MECpuDebugCapabilityDisabled message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t mECpuDebugCapabilityDisabled()
{
    return getLog(
        redfish::registries::Openbmc::Index::mECpuDebugCapabilityDisabled, {});
}

void mECpuDebugCapabilityDisabled(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, mECpuDebugCapabilityDisabled());
}

/**
 * @internal
 * @brief Formats MEDirectFlashUpdateRequested message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t mEDirectFlashUpdateRequested()
{
    return getLog(
        redfish::registries::Openbmc::Index::mEDirectFlashUpdateRequested, {});
}

void mEDirectFlashUpdateRequested(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, mEDirectFlashUpdateRequested());
}

/**
 * @internal
 * @brief Formats MEExceptionDuringShutdown message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t mEExceptionDuringShutdown()
{
    return getLog(
        redfish::registries::Openbmc::Index::mEExceptionDuringShutdown, {});
}

void mEExceptionDuringShutdown(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, mEExceptionDuringShutdown());
}

/**
 * @internal
 * @brief Formats MEFactoryResetError message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t mEFactoryResetError(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::mEFactoryResetError,
                  std::to_array({arg1}));
}

void mEFactoryResetError(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, mEFactoryResetError(arg1));
}

/**
 * @internal
 * @brief Formats MEFactoryRestore message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t mEFactoryRestore(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::mEFactoryRestore,
                  std::to_array({arg1}));
}

void mEFactoryRestore(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, mEFactoryRestore(arg1));
}

/**
 * @internal
 * @brief Formats MEFirmwareException message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t mEFirmwareException(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::mEFirmwareException,
                  std::to_array({arg1}));
}

void mEFirmwareException(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, mEFirmwareException(arg1));
}

/**
 * @internal
 * @brief Formats MEFirmwarePanicReason message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t mEFirmwarePanicReason(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::mEFirmwarePanicReason,
                  std::to_array({arg1}));
}

void mEFirmwarePanicReason(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, mEFirmwarePanicReason(arg1));
}

/**
 * @internal
 * @brief Formats MEFirmwareRecoveryReason message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t mEFirmwareRecoveryReason(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::mEFirmwareRecoveryReason,
                  std::to_array({arg1}));
}

void mEFirmwareRecoveryReason(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, mEFirmwareRecoveryReason(arg1));
}

/**
 * @internal
 * @brief Formats MEFirmwareResiliencyError message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t mEFirmwareResiliencyError(std::string_view arg1)
{
    return getLog(
        redfish::registries::Openbmc::Index::mEFirmwareResiliencyError,
        std::to_array({arg1}));
}

void mEFirmwareResiliencyError(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, mEFirmwareResiliencyError(arg1));
}

/**
 * @internal
 * @brief Formats MEFlashEraseError message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t mEFlashEraseError()
{
    return getLog(redfish::registries::Openbmc::Index::mEFlashEraseError, {});
}

void mEFlashEraseError(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, mEFlashEraseError());
}

/**
 * @internal
 * @brief Formats MEFlashStateInformation message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t mEFlashStateInformation(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::mEFlashStateInformation,
                  std::to_array({arg1}));
}

void mEFlashStateInformation(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, mEFlashStateInformation(arg1));
}

/**
 * @internal
 * @brief Formats MEFlashStateInformationWritingEnabled message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t mEFlashStateInformationWritingEnabled(
    std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::
                      mEFlashStateInformationWritingEnabled,
                  std::to_array({arg1}));
}

void mEFlashStateInformationWritingEnabled(crow::Response& res,
                                           std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          mEFlashStateInformationWritingEnabled(arg1));
}

/**
 * @internal
 * @brief Formats MEFlashVerificationError message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t mEFlashVerificationError(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::mEFlashVerificationError,
                  std::to_array({arg1}));
}

void mEFlashVerificationError(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, mEFlashVerificationError(arg1));
}

/**
 * @internal
 * @brief Formats MEFlashWearOutWarning message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t mEFlashWearOutWarning(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::mEFlashWearOutWarning,
                  std::to_array({arg1}));
}

void mEFlashWearOutWarning(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, mEFlashWearOutWarning(arg1));
}

/**
 * @internal
 * @brief Formats MEImageExecutionFailed message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t mEImageExecutionFailed()
{
    return getLog(redfish::registries::Openbmc::Index::mEImageExecutionFailed,
                  {});
}

void mEImageExecutionFailed(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, mEImageExecutionFailed());
}

/**
 * @internal
 * @brief Formats MEInternalError message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t mEInternalError()
{
    return getLog(redfish::registries::Openbmc::Index::mEInternalError, {});
}

void mEInternalError(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, mEInternalError());
}

/**
 * @internal
 * @brief Formats MEManufacturingError message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t mEManufacturingError(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::mEManufacturingError,
                  std::to_array({arg1}));
}

void mEManufacturingError(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, mEManufacturingError(arg1));
}

/**
 * @internal
 * @brief Formats MEMctpInterfaceError message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t mEMctpInterfaceError(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::mEMctpInterfaceError,
                  std::to_array({arg1}));
}

void mEMctpInterfaceError(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, mEMctpInterfaceError(arg1));
}

/**
 * @internal
 * @brief Formats MEMultiPchModeMisconfig message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t mEMultiPchModeMisconfig(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::mEMultiPchModeMisconfig,
                  std::to_array({arg1}));
}

void mEMultiPchModeMisconfig(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, mEMultiPchModeMisconfig(arg1));
}

/**
 * @internal
 * @brief Formats MEPeciOverDmiError message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t mEPeciOverDmiError(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::mEPeciOverDmiError,
                  std::to_array({arg1}));
}

void mEPeciOverDmiError(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, mEPeciOverDmiError(arg1));
}

/**
 * @internal
 * @brief Formats MEPttHealthEvent message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t mEPttHealthEvent(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::mEPttHealthEvent,
                  std::to_array({arg1}));
}

void mEPttHealthEvent(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, mEPttHealthEvent(arg1));
}

/**
 * @internal
 * @brief Formats MERecoveryGpioForced message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t mERecoveryGpioForced()
{
    return getLog(redfish::registries::Openbmc::Index::mERecoveryGpioForced,
                  {});
}

void mERecoveryGpioForced(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, mERecoveryGpioForced());
}

/**
 * @internal
 * @brief Formats MERestrictedMode message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t mERestrictedMode(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::mERestrictedMode,
                  std::to_array({arg1}));
}

void mERestrictedMode(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, mERestrictedMode(arg1));
}

/**
 * @internal
 * @brief Formats MESmbusLinkFailure message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t mESmbusLinkFailure(std::string_view arg1,
                                            std::string_view arg2)
{
    return getLog(redfish::registries::Openbmc::Index::mESmbusLinkFailure,
                  std::to_array({arg1, arg2}));
}

void mESmbusLinkFailure(crow::Response& res, std::string_view arg1,
                        std::string_view arg2)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, mESmbusLinkFailure(arg1, arg2));
}

/**
 * @internal
 * @brief Formats MEUmaError message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t mEUmaError(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::mEUmaError,
                  std::to_array({arg1}));
}

void mEUmaError(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, mEUmaError(arg1));
}

/**
 * @internal
 * @brief Formats MEUnsupportedFeature message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t mEUnsupportedFeature(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::mEUnsupportedFeature,
                  std::to_array({arg1}));
}

void mEUnsupportedFeature(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, mEUnsupportedFeature(arg1));
}

/**
 * @internal
 * @brief Formats ManufacturingModeEntered message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t manufacturingModeEntered()
{
    return getLog(redfish::registries::Openbmc::Index::manufacturingModeEntered,
                  {});
}

void manufacturingModeEntered(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, manufacturingModeEntered());
}

/**
 * @internal
 * @brief Formats ManufacturingModeExited message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t manufacturingModeExited()
{
    return getLog(redfish::registries::Openbmc::Index::manufacturingModeExited,
                  {});
}

void manufacturingModeExited(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, manufacturingModeExited());
}

/**
 * @internal
 * @brief Formats MemoryECCCorrectable message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t memoryECCCorrectable(
    std::string_view arg1, std::string_view arg2, std::string_view arg3,
    std::string_view arg4)
{
    return getLog(redfish::registries::Openbmc::Index::memoryECCCorrectable,
                  std::to_array({arg1, arg2, arg3, arg4}));
}

void memoryECCCorrectable(crow::Response& res, std::string_view arg1,
                          std::string_view arg2, std::string_view arg3,
                          std::string_view arg4)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          memoryECCCorrectable(arg1, arg2, arg3, arg4));
}

/**
 * @internal
 * @brief Formats MemoryECCUncorrectable message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t memoryECCUncorrectable(
    std::string_view arg1, std::string_view arg2, std::string_view arg3,
    std::string_view arg4)
{
    return getLog(redfish::registries::Openbmc::Index::memoryECCUncorrectable,
                  std::to_array({arg1, arg2, arg3, arg4}));
}

void memoryECCUncorrectable(crow::Response& res, std::string_view arg1,
                            std::string_view arg2, std::string_view arg3,
                            std::string_view arg4)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          memoryECCUncorrectable(arg1, arg2, arg3, arg4));
}

/**
 * @internal
 * @brief Formats MemoryParityCommandAndAddress message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t memoryParityCommandAndAddress(
    std::string_view arg1, std::string_view arg2, std::string_view arg3,
    std::string_view arg4, std::string_view arg5)
{
    return getLog(
        redfish::registries::Openbmc::Index::memoryParityCommandAndAddress,
        std::to_array({arg1, arg2, arg3, arg4, arg5}));
}

void memoryParityCommandAndAddress(crow::Response& res, std::string_view arg1,
                                   std::string_view arg2, std::string_view arg3,
                                   std::string_view arg4, std::string_view arg5)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, memoryParityCommandAndAddress(
                                             arg1, arg2, arg3, arg4, arg5));
}

/**
 * @internal
 * @brief Formats MemoryParityNotKnown message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t memoryParityNotKnown(
    std::string_view arg1, std::string_view arg2, std::string_view arg3,
    std::string_view arg4, std::string_view arg5)
{
    return getLog(redfish::registries::Openbmc::Index::memoryParityNotKnown,
                  std::to_array({arg1, arg2, arg3, arg4, arg5}));
}

void memoryParityNotKnown(crow::Response& res, std::string_view arg1,
                          std::string_view arg2, std::string_view arg3,
                          std::string_view arg4, std::string_view arg5)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          memoryParityNotKnown(arg1, arg2, arg3, arg4, arg5));
}

/**
 * @internal
 * @brief Formats MemoryRASConfigurationDisabled message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t memoryRASConfigurationDisabled(std::string_view arg1,
                                                        std::string_view arg2)
{
    return getLog(
        redfish::registries::Openbmc::Index::memoryRASConfigurationDisabled,
        std::to_array({arg1, arg2}));
}

void memoryRASConfigurationDisabled(crow::Response& res, std::string_view arg1,
                                    std::string_view arg2)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          memoryRASConfigurationDisabled(arg1, arg2));
}

/**
 * @internal
 * @brief Formats MemoryRASConfigurationEnabled message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t memoryRASConfigurationEnabled(std::string_view arg1,
                                                       std::string_view arg2)
{
    return getLog(
        redfish::registries::Openbmc::Index::memoryRASConfigurationEnabled,
        std::to_array({arg1, arg2}));
}

void memoryRASConfigurationEnabled(crow::Response& res, std::string_view arg1,
                                   std::string_view arg2)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          memoryRASConfigurationEnabled(arg1, arg2));
}

/**
 * @internal
 * @brief Formats MemoryRASModeDisabled message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t memoryRASModeDisabled(std::string_view arg1,
                                               std::string_view arg2)
{
    return getLog(redfish::registries::Openbmc::Index::memoryRASModeDisabled,
                  std::to_array({arg1, arg2}));
}

void memoryRASModeDisabled(crow::Response& res, std::string_view arg1,
                           std::string_view arg2)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, memoryRASModeDisabled(arg1, arg2));
}

/**
 * @internal
 * @brief Formats MemoryRASModeEnabled message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t memoryRASModeEnabled(std::string_view arg1,
                                              std::string_view arg2)
{
    return getLog(redfish::registries::Openbmc::Index::memoryRASModeEnabled,
                  std::to_array({arg1, arg2}));
}

void memoryRASModeEnabled(crow::Response& res, std::string_view arg1,
                          std::string_view arg2)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, memoryRASModeEnabled(arg1, arg2));
}

/**
 * @internal
 * @brief Formats MemoryThermTrip message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t memoryThermTrip(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::memoryThermTrip,
                  std::to_array({arg1}));
}

void memoryThermTrip(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, memoryThermTrip(arg1));
}

/**
 * @internal
 * @brief Formats MirroringRedundancyDegraded message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t mirroringRedundancyDegraded(
    std::string_view arg1, std::string_view arg2, std::string_view arg3,
    std::string_view arg4, std::string_view arg5)
{
    return getLog(
        redfish::registries::Openbmc::Index::mirroringRedundancyDegraded,
        std::to_array({arg1, arg2, arg3, arg4, arg5}));
}

void mirroringRedundancyDegraded(crow::Response& res, std::string_view arg1,
                                 std::string_view arg2, std::string_view arg3,
                                 std::string_view arg4, std::string_view arg5)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, mirroringRedundancyDegraded(
                                             arg1, arg2, arg3, arg4, arg5));
}

/**
 * @internal
 * @brief Formats MirroringRedundancyFull message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t mirroringRedundancyFull(
    std::string_view arg1, std::string_view arg2, std::string_view arg3,
    std::string_view arg4, std::string_view arg5)
{
    return getLog(redfish::registries::Openbmc::Index::mirroringRedundancyFull,
                  std::to_array({arg1, arg2, arg3, arg4, arg5}));
}

void mirroringRedundancyFull(crow::Response& res, std::string_view arg1,
                             std::string_view arg2, std::string_view arg3,
                             std::string_view arg4, std::string_view arg5)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(
        res.jsonValue, mirroringRedundancyFull(arg1, arg2, arg3, arg4, arg5));
}

/**
 * @internal
 * @brief Formats NMIButtonPressed message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t nMIButtonPressed()
{
    return getLog(redfish::registries::Openbmc::Index::nMIButtonPressed, {});
}

void nMIButtonPressed(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, nMIButtonPressed());
}

/**
 * @internal
 * @brief Formats NMIDiagnosticInterrupt message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t nMIDiagnosticInterrupt()
{
    return getLog(redfish::registries::Openbmc::Index::nMIDiagnosticInterrupt,
                  {});
}

void nMIDiagnosticInterrupt(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, nMIDiagnosticInterrupt());
}

/**
 * @internal
 * @brief Formats PCIeCorrectableAdvisoryNonFatal message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t pCIeCorrectableAdvisoryNonFatal(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(
        redfish::registries::Openbmc::Index::pCIeCorrectableAdvisoryNonFatal,
        std::to_array({arg1, arg2, arg3}));
}

void pCIeCorrectableAdvisoryNonFatal(crow::Response& res, std::string_view arg1,
                                     std::string_view arg2,
                                     std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          pCIeCorrectableAdvisoryNonFatal(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats PCIeCorrectableBadDLLP message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t pCIeCorrectableBadDLLP(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(redfish::registries::Openbmc::Index::pCIeCorrectableBadDLLP,
                  std::to_array({arg1, arg2, arg3}));
}

void pCIeCorrectableBadDLLP(crow::Response& res, std::string_view arg1,
                            std::string_view arg2, std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          pCIeCorrectableBadDLLP(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats PCIeCorrectableBadTLP message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t pCIeCorrectableBadTLP(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(redfish::registries::Openbmc::Index::pCIeCorrectableBadTLP,
                  std::to_array({arg1, arg2, arg3}));
}

void pCIeCorrectableBadTLP(crow::Response& res, std::string_view arg1,
                           std::string_view arg2, std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          pCIeCorrectableBadTLP(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats PCIeCorrectableHeaderLogOverflow message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t pCIeCorrectableHeaderLogOverflow(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(
        redfish::registries::Openbmc::Index::pCIeCorrectableHeaderLogOverflow,
        std::to_array({arg1, arg2, arg3}));
}

void pCIeCorrectableHeaderLogOverflow(
    crow::Response& res, std::string_view arg1, std::string_view arg2,
    std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          pCIeCorrectableHeaderLogOverflow(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats PCIeCorrectableInternal message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t pCIeCorrectableInternal(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(redfish::registries::Openbmc::Index::pCIeCorrectableInternal,
                  std::to_array({arg1, arg2, arg3}));
}

void pCIeCorrectableInternal(crow::Response& res, std::string_view arg1,
                             std::string_view arg2, std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          pCIeCorrectableInternal(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats PCIeCorrectableLinkBWChanged message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t pCIeCorrectableLinkBWChanged(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(
        redfish::registries::Openbmc::Index::pCIeCorrectableLinkBWChanged,
        std::to_array({arg1, arg2, arg3}));
}

void pCIeCorrectableLinkBWChanged(crow::Response& res, std::string_view arg1,
                                  std::string_view arg2, std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          pCIeCorrectableLinkBWChanged(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats PCIeCorrectableReceiverError message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t pCIeCorrectableReceiverError(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(
        redfish::registries::Openbmc::Index::pCIeCorrectableReceiverError,
        std::to_array({arg1, arg2, arg3}));
}

void pCIeCorrectableReceiverError(crow::Response& res, std::string_view arg1,
                                  std::string_view arg2, std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          pCIeCorrectableReceiverError(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats PCIeCorrectableReplayNumRollover message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t pCIeCorrectableReplayNumRollover(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(
        redfish::registries::Openbmc::Index::pCIeCorrectableReplayNumRollover,
        std::to_array({arg1, arg2, arg3}));
}

void pCIeCorrectableReplayNumRollover(
    crow::Response& res, std::string_view arg1, std::string_view arg2,
    std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          pCIeCorrectableReplayNumRollover(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats PCIeCorrectableReplayTimerTimeout message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t pCIeCorrectableReplayTimerTimeout(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(
        redfish::registries::Openbmc::Index::pCIeCorrectableReplayTimerTimeout,
        std::to_array({arg1, arg2, arg3}));
}

void pCIeCorrectableReplayTimerTimeout(
    crow::Response& res, std::string_view arg1, std::string_view arg2,
    std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          pCIeCorrectableReplayTimerTimeout(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats PCIeCorrectableUnspecifiedAERError message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t pCIeCorrectableUnspecifiedAERError(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(
        redfish::registries::Openbmc::Index::pCIeCorrectableUnspecifiedAERError,
        std::to_array({arg1, arg2, arg3}));
}

void pCIeCorrectableUnspecifiedAERError(
    crow::Response& res, std::string_view arg1, std::string_view arg2,
    std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          pCIeCorrectableUnspecifiedAERError(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats PCIeFatalACSViolation message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t pCIeFatalACSViolation(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(redfish::registries::Openbmc::Index::pCIeFatalACSViolation,
                  std::to_array({arg1, arg2, arg3}));
}

void pCIeFatalACSViolation(crow::Response& res, std::string_view arg1,
                           std::string_view arg2, std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          pCIeFatalACSViolation(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats PCIeFatalAtomicEgressBlocked message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t pCIeFatalAtomicEgressBlocked(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(
        redfish::registries::Openbmc::Index::pCIeFatalAtomicEgressBlocked,
        std::to_array({arg1, arg2, arg3}));
}

void pCIeFatalAtomicEgressBlocked(crow::Response& res, std::string_view arg1,
                                  std::string_view arg2, std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          pCIeFatalAtomicEgressBlocked(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats PCIeFatalCompleterAbort message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t pCIeFatalCompleterAbort(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(redfish::registries::Openbmc::Index::pCIeFatalCompleterAbort,
                  std::to_array({arg1, arg2, arg3}));
}

void pCIeFatalCompleterAbort(crow::Response& res, std::string_view arg1,
                             std::string_view arg2, std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          pCIeFatalCompleterAbort(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats PCIeFatalCompletionTimeout message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t pCIeFatalCompletionTimeout(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(
        redfish::registries::Openbmc::Index::pCIeFatalCompletionTimeout,
        std::to_array({arg1, arg2, arg3}));
}

void pCIeFatalCompletionTimeout(crow::Response& res, std::string_view arg1,
                                std::string_view arg2, std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          pCIeFatalCompletionTimeout(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats PCIeFatalDataLinkLayerProtocol message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t pCIeFatalDataLinkLayerProtocol(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(
        redfish::registries::Openbmc::Index::pCIeFatalDataLinkLayerProtocol,
        std::to_array({arg1, arg2, arg3}));
}

void pCIeFatalDataLinkLayerProtocol(crow::Response& res, std::string_view arg1,
                                    std::string_view arg2,
                                    std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          pCIeFatalDataLinkLayerProtocol(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats PCIeFatalECRCError message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t pCIeFatalECRCError(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(redfish::registries::Openbmc::Index::pCIeFatalECRCError,
                  std::to_array({arg1, arg2, arg3}));
}

void pCIeFatalECRCError(crow::Response& res, std::string_view arg1,
                        std::string_view arg2, std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, pCIeFatalECRCError(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats PCIeFatalFlowControlProtocol message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t pCIeFatalFlowControlProtocol(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(
        redfish::registries::Openbmc::Index::pCIeFatalFlowControlProtocol,
        std::to_array({arg1, arg2, arg3}));
}

void pCIeFatalFlowControlProtocol(crow::Response& res, std::string_view arg1,
                                  std::string_view arg2, std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          pCIeFatalFlowControlProtocol(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats PCIeFatalMCBlockedTLP message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t pCIeFatalMCBlockedTLP(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(redfish::registries::Openbmc::Index::pCIeFatalMCBlockedTLP,
                  std::to_array({arg1, arg2, arg3}));
}

void pCIeFatalMCBlockedTLP(crow::Response& res, std::string_view arg1,
                           std::string_view arg2, std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          pCIeFatalMCBlockedTLP(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats PCIeFatalMalformedTLP message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t pCIeFatalMalformedTLP(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(redfish::registries::Openbmc::Index::pCIeFatalMalformedTLP,
                  std::to_array({arg1, arg2, arg3}));
}

void pCIeFatalMalformedTLP(crow::Response& res, std::string_view arg1,
                           std::string_view arg2, std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          pCIeFatalMalformedTLP(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats PCIeFatalPoisonedTLP message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t pCIeFatalPoisonedTLP(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(redfish::registries::Openbmc::Index::pCIeFatalPoisonedTLP,
                  std::to_array({arg1, arg2, arg3}));
}

void pCIeFatalPoisonedTLP(crow::Response& res, std::string_view arg1,
                          std::string_view arg2, std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          pCIeFatalPoisonedTLP(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats PCIeFatalReceivedErrNonFatalMessage message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t pCIeFatalReceivedErrNonFatalMessage(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(redfish::registries::Openbmc::Index::
                      pCIeFatalReceivedErrNonFatalMessage,
                  std::to_array({arg1, arg2, arg3}));
}

void pCIeFatalReceivedErrNonFatalMessage(
    crow::Response& res, std::string_view arg1, std::string_view arg2,
    std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(
        res.jsonValue, pCIeFatalReceivedErrNonFatalMessage(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats PCIeFatalReceivedFatalMessageFromDownstream message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t pCIeFatalReceivedFatalMessageFromDownstream(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(redfish::registries::Openbmc::Index::
                      pCIeFatalReceivedFatalMessageFromDownstream,
                  std::to_array({arg1, arg2, arg3}));
}

void pCIeFatalReceivedFatalMessageFromDownstream(
    crow::Response& res, std::string_view arg1, std::string_view arg2,
    std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(
        res.jsonValue,
        pCIeFatalReceivedFatalMessageFromDownstream(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats PCIeFatalReceiverBufferOverflow message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t pCIeFatalReceiverBufferOverflow(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(
        redfish::registries::Openbmc::Index::pCIeFatalReceiverBufferOverflow,
        std::to_array({arg1, arg2, arg3}));
}

void pCIeFatalReceiverBufferOverflow(crow::Response& res, std::string_view arg1,
                                     std::string_view arg2,
                                     std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          pCIeFatalReceiverBufferOverflow(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats PCIeFatalSurpriseLinkDown message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t pCIeFatalSurpriseLinkDown(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(
        redfish::registries::Openbmc::Index::pCIeFatalSurpriseLinkDown,
        std::to_array({arg1, arg2, arg3}));
}

void pCIeFatalSurpriseLinkDown(crow::Response& res, std::string_view arg1,
                               std::string_view arg2, std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          pCIeFatalSurpriseLinkDown(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats PCIeFatalTLPPrefixBlocked message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t pCIeFatalTLPPrefixBlocked(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(
        redfish::registries::Openbmc::Index::pCIeFatalTLPPrefixBlocked,
        std::to_array({arg1, arg2, arg3}));
}

void pCIeFatalTLPPrefixBlocked(crow::Response& res, std::string_view arg1,
                               std::string_view arg2, std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          pCIeFatalTLPPrefixBlocked(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats PCIeFatalUncorrectableInternal message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t pCIeFatalUncorrectableInternal(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(
        redfish::registries::Openbmc::Index::pCIeFatalUncorrectableInternal,
        std::to_array({arg1, arg2, arg3}));
}

void pCIeFatalUncorrectableInternal(crow::Response& res, std::string_view arg1,
                                    std::string_view arg2,
                                    std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          pCIeFatalUncorrectableInternal(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats PCIeFatalUnexpectedCompletion message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t pCIeFatalUnexpectedCompletion(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(
        redfish::registries::Openbmc::Index::pCIeFatalUnexpectedCompletion,
        std::to_array({arg1, arg2, arg3}));
}

void pCIeFatalUnexpectedCompletion(crow::Response& res, std::string_view arg1,
                                   std::string_view arg2, std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          pCIeFatalUnexpectedCompletion(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats PCIeFatalUnspecifiedNonAERFatalError message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t pCIeFatalUnspecifiedNonAERFatalError(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(redfish::registries::Openbmc::Index::
                      pCIeFatalUnspecifiedNonAERFatalError,
                  std::to_array({arg1, arg2, arg3}));
}

void pCIeFatalUnspecifiedNonAERFatalError(
    crow::Response& res, std::string_view arg1, std::string_view arg2,
    std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(
        res.jsonValue, pCIeFatalUnspecifiedNonAERFatalError(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats PCIeFatalUnsupportedRequest message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t pCIeFatalUnsupportedRequest(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(
        redfish::registries::Openbmc::Index::pCIeFatalUnsupportedRequest,
        std::to_array({arg1, arg2, arg3}));
}

void pCIeFatalUnsupportedRequest(crow::Response& res, std::string_view arg1,
                                 std::string_view arg2, std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          pCIeFatalUnsupportedRequest(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats PowerButtonPressed message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t powerButtonPressed()
{
    return getLog(redfish::registries::Openbmc::Index::powerButtonPressed, {});
}

void powerButtonPressed(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, powerButtonPressed());
}

/**
 * @internal
 * @brief Formats PowerButtonLongPressed message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t powerButtonLongPressed()
{
    return getLog(redfish::registries::Openbmc::Index::powerButtonLongPressed,
                  {});
}

void powerButtonLongPressed(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, powerButtonLongPressed());
}

/**
 * @internal
 * @brief Formats PowerRestorePolicyApplied message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t powerRestorePolicyApplied()
{
    return getLog(
        redfish::registries::Openbmc::Index::powerRestorePolicyApplied, {});
}

void powerRestorePolicyApplied(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, powerRestorePolicyApplied());
}

/**
 * @internal
 * @brief Formats PowerSupplyConfigurationError message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t powerSupplyConfigurationError(std::string_view arg1)
{
    return getLog(
        redfish::registries::Openbmc::Index::powerSupplyConfigurationError,
        std::to_array({arg1}));
}

void powerSupplyConfigurationError(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, powerSupplyConfigurationError(arg1));
}

/**
 * @internal
 * @brief Formats PowerSupplyConfigurationErrorRecovered message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t powerSupplyConfigurationErrorRecovered(
    std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::
                      powerSupplyConfigurationErrorRecovered,
                  std::to_array({arg1}));
}

void powerSupplyConfigurationErrorRecovered(crow::Response& res,
                                            std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          powerSupplyConfigurationErrorRecovered(arg1));
}

/**
 * @internal
 * @brief Formats PowerSupplyFailed message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t powerSupplyFailed(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::powerSupplyFailed,
                  std::to_array({arg1}));
}

void powerSupplyFailed(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, powerSupplyFailed(arg1));
}

/**
 * @internal
 * @brief Formats PowerSupplyFailurePredicted message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t powerSupplyFailurePredicted(std::string_view arg1)
{
    return getLog(
        redfish::registries::Openbmc::Index::powerSupplyFailurePredicted,
        std::to_array({arg1}));
}

void powerSupplyFailurePredicted(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, powerSupplyFailurePredicted(arg1));
}

/**
 * @internal
 * @brief Formats PowerSupplyFanFailed message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t powerSupplyFanFailed(std::string_view arg1,
                                              std::string_view arg2)
{
    return getLog(redfish::registries::Openbmc::Index::powerSupplyFanFailed,
                  std::to_array({arg1, arg2}));
}

void powerSupplyFanFailed(crow::Response& res, std::string_view arg1,
                          std::string_view arg2)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, powerSupplyFanFailed(arg1, arg2));
}

/**
 * @internal
 * @brief Formats PowerSupplyFanRecovered message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t powerSupplyFanRecovered(std::string_view arg1,
                                                 std::string_view arg2)
{
    return getLog(redfish::registries::Openbmc::Index::powerSupplyFanRecovered,
                  std::to_array({arg1, arg2}));
}

void powerSupplyFanRecovered(crow::Response& res, std::string_view arg1,
                             std::string_view arg2)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, powerSupplyFanRecovered(arg1, arg2));
}

/**
 * @internal
 * @brief Formats PowerSupplyInserted message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t powerSupplyInserted(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::powerSupplyInserted,
                  std::to_array({arg1}));
}

void powerSupplyInserted(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, powerSupplyInserted(arg1));
}

/**
 * @internal
 * @brief Formats PowerSupplyPowerGoodFailed message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t powerSupplyPowerGoodFailed(std::string_view arg1)
{
    return getLog(
        redfish::registries::Openbmc::Index::powerSupplyPowerGoodFailed,
        std::to_array({arg1}));
}

void powerSupplyPowerGoodFailed(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, powerSupplyPowerGoodFailed(arg1));
}

/**
 * @internal
 * @brief Formats PowerSupplyPowerLost message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t powerSupplyPowerLost(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::powerSupplyPowerLost,
                  std::to_array({arg1}));
}

void powerSupplyPowerLost(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, powerSupplyPowerLost(arg1));
}

/**
 * @internal
 * @brief Formats PowerSupplyPowerRestored message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t powerSupplyPowerRestored(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::powerSupplyPowerRestored,
                  std::to_array({arg1}));
}

void powerSupplyPowerRestored(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, powerSupplyPowerRestored(arg1));
}

/**
 * @internal
 * @brief Formats PowerSupplyPredictedFailureRecovered message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t powerSupplyPredictedFailureRecovered(
    std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::
                      powerSupplyPredictedFailureRecovered,
                  std::to_array({arg1}));
}

void powerSupplyPredictedFailureRecovered(crow::Response& res,
                                          std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          powerSupplyPredictedFailureRecovered(arg1));
}

/**
 * @internal
 * @brief Formats PowerSupplyRecovered message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t powerSupplyRecovered(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::powerSupplyRecovered,
                  std::to_array({arg1}));
}

void powerSupplyRecovered(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, powerSupplyRecovered(arg1));
}

/**
 * @internal
 * @brief Formats PowerSupplyRemoved message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t powerSupplyRemoved(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::powerSupplyRemoved,
                  std::to_array({arg1}));
}

void powerSupplyRemoved(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, powerSupplyRemoved(arg1));
}

/**
 * @internal
 * @brief Formats PowerUnitDegradedFromNonRedundant message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t powerUnitDegradedFromNonRedundant()
{
    return getLog(
        redfish::registries::Openbmc::Index::powerUnitDegradedFromNonRedundant,
        {});
}

void powerUnitDegradedFromNonRedundant(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, powerUnitDegradedFromNonRedundant());
}

/**
 * @internal
 * @brief Formats PowerUnitDegradedFromRedundant message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t powerUnitDegradedFromRedundant()
{
    return getLog(
        redfish::registries::Openbmc::Index::powerUnitDegradedFromRedundant,
        {});
}

void powerUnitDegradedFromRedundant(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, powerUnitDegradedFromRedundant());
}

/**
 * @internal
 * @brief Formats PowerUnitNonRedundantFromInsufficient message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t powerUnitNonRedundantFromInsufficient()
{
    return getLog(redfish::registries::Openbmc::Index::
                      powerUnitNonRedundantFromInsufficient,
                  {});
}

void powerUnitNonRedundantFromInsufficient(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          powerUnitNonRedundantFromInsufficient());
}

/**
 * @internal
 * @brief Formats PowerUnitNonRedundantInsufficient message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t powerUnitNonRedundantInsufficient()
{
    return getLog(
        redfish::registries::Openbmc::Index::powerUnitNonRedundantInsufficient,
        {});
}

void powerUnitNonRedundantInsufficient(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, powerUnitNonRedundantInsufficient());
}

/**
 * @internal
 * @brief Formats PowerUnitNonRedundantSufficient message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t powerUnitNonRedundantSufficient()
{
    return getLog(
        redfish::registries::Openbmc::Index::powerUnitNonRedundantSufficient,
        {});
}

void powerUnitNonRedundantSufficient(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, powerUnitNonRedundantSufficient());
}

/**
 * @internal
 * @brief Formats PowerUnitRedundancyDegraded message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t powerUnitRedundancyDegraded()
{
    return getLog(
        redfish::registries::Openbmc::Index::powerUnitRedundancyDegraded, {});
}

void powerUnitRedundancyDegraded(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, powerUnitRedundancyDegraded());
}

/**
 * @internal
 * @brief Formats PowerUnitRedundancyLost message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t powerUnitRedundancyLost()
{
    return getLog(redfish::registries::Openbmc::Index::powerUnitRedundancyLost,
                  {});
}

void powerUnitRedundancyLost(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, powerUnitRedundancyLost());
}

/**
 * @internal
 * @brief Formats PowerUnitRedundancyRegained message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t powerUnitRedundancyRegained()
{
    return getLog(
        redfish::registries::Openbmc::Index::powerUnitRedundancyRegained, {});
}

void powerUnitRedundancyRegained(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, powerUnitRedundancyRegained());
}

/**
 * @internal
 * @brief Formats ResetButtonPressed message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t resetButtonPressed()
{
    return getLog(redfish::registries::Openbmc::Index::resetButtonPressed, {});
}

void resetButtonPressed(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, resetButtonPressed());
}

/**
 * @internal
 * @brief Formats SELEntryAdded message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t sELEntryAdded(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::sELEntryAdded,
                  std::to_array({arg1}));
}

void sELEntryAdded(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, sELEntryAdded(arg1));
}

/**
 * @internal
 * @brief Formats SecurityBoot2ndFlashEnabled message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t securityBoot2ndFlashEnabled()
{
    return getLog(
        redfish::registries::Openbmc::Index::securityBoot2ndFlashEnabled, {});
}

void securityBoot2ndFlashEnabled(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, securityBoot2ndFlashEnabled());
}

/**
 * @internal
 * @brief Formats SecurityP2aBridgeEnabled message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t securityP2aBridgeEnabled()
{
    return getLog(redfish::registries::Openbmc::Index::securityP2aBridgeEnabled,
                  {});
}

void securityP2aBridgeEnabled(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, securityP2aBridgeEnabled());
}

/**
 * @internal
 * @brief Formats SecurityUartPortDebugEnabled message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t securityUartPortDebugEnabled()
{
    return getLog(
        redfish::registries::Openbmc::Index::securityUartPortDebugEnabled, {});
}

void securityUartPortDebugEnabled(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, securityUartPortDebugEnabled());
}

/**
 * @internal
 * @brief Formats SecurityUserNonRootUidZeroAssigned message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t securityUserNonRootUidZeroAssigned()
{
    return getLog(
        redfish::registries::Openbmc::Index::securityUserNonRootUidZeroAssigned,
        {});
}

void securityUserNonRootUidZeroAssigned(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, securityUserNonRootUidZeroAssigned());
}

/**
 * @internal
 * @brief Formats SecurityUserNonRootUidZeroRemoved message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t securityUserNonRootUidZeroRemoved()
{
    return getLog(
        redfish::registries::Openbmc::Index::securityUserNonRootUidZeroRemoved,
        {});
}

void securityUserNonRootUidZeroRemoved(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, securityUserNonRootUidZeroRemoved());
}

/**
 * @internal
 * @brief Formats SecurityUserRootDisabled message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t securityUserRootDisabled()
{
    return getLog(redfish::registries::Openbmc::Index::securityUserRootDisabled,
                  {});
}

void securityUserRootDisabled(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, securityUserRootDisabled());
}

/**
 * @internal
 * @brief Formats SecurityUserRootEnabled message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t securityUserRootEnabled()
{
    return getLog(redfish::registries::Openbmc::Index::securityUserRootEnabled,
                  {});
}

void securityUserRootEnabled(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, securityUserRootEnabled());
}

/**
 * @internal
 * @brief Formats SecurityUserStrongHashAlgoRestored message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t securityUserStrongHashAlgoRestored()
{
    return getLog(
        redfish::registries::Openbmc::Index::securityUserStrongHashAlgoRestored,
        {});
}

void securityUserStrongHashAlgoRestored(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, securityUserStrongHashAlgoRestored());
}

/**
 * @internal
 * @brief Formats SecurityUserUnsupportedShellEnabled message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t securityUserUnsupportedShellEnabled()
{
    return getLog(redfish::registries::Openbmc::Index::
                      securityUserUnsupportedShellEnabled,
                  {});
}

void securityUserUnsupportedShellEnabled(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, securityUserUnsupportedShellEnabled());
}

/**
 * @internal
 * @brief Formats SecurityUserUnsupportedShellRemoved message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t securityUserUnsupportedShellRemoved()
{
    return getLog(redfish::registries::Openbmc::Index::
                      securityUserUnsupportedShellRemoved,
                  {});
}

void securityUserUnsupportedShellRemoved(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, securityUserUnsupportedShellRemoved());
}

/**
 * @internal
 * @brief Formats SecurityUserWeakHashAlgoEnabled message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t securityUserWeakHashAlgoEnabled()
{
    return getLog(
        redfish::registries::Openbmc::Index::securityUserWeakHashAlgoEnabled,
        {});
}

void securityUserWeakHashAlgoEnabled(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, securityUserWeakHashAlgoEnabled());
}

/**
 * @internal
 * @brief Formats SensorThresholdCriticalHighGoingHigh message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t sensorThresholdCriticalHighGoingHigh(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(redfish::registries::Openbmc::Index::
                      sensorThresholdCriticalHighGoingHigh,
                  std::to_array({arg1, arg2, arg3}));
}

void sensorThresholdCriticalHighGoingHigh(
    crow::Response& res, std::string_view arg1, std::string_view arg2,
    std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(
        res.jsonValue, sensorThresholdCriticalHighGoingHigh(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats SensorThresholdCriticalHighGoingLow message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t sensorThresholdCriticalHighGoingLow(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(redfish::registries::Openbmc::Index::
                      sensorThresholdCriticalHighGoingLow,
                  std::to_array({arg1, arg2, arg3}));
}

void sensorThresholdCriticalHighGoingLow(
    crow::Response& res, std::string_view arg1, std::string_view arg2,
    std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(
        res.jsonValue, sensorThresholdCriticalHighGoingLow(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats SensorThresholdCriticalLowGoingHigh message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t sensorThresholdCriticalLowGoingHigh(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(redfish::registries::Openbmc::Index::
                      sensorThresholdCriticalLowGoingHigh,
                  std::to_array({arg1, arg2, arg3}));
}

void sensorThresholdCriticalLowGoingHigh(
    crow::Response& res, std::string_view arg1, std::string_view arg2,
    std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(
        res.jsonValue, sensorThresholdCriticalLowGoingHigh(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats SensorThresholdCriticalLowGoingLow message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t sensorThresholdCriticalLowGoingLow(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(
        redfish::registries::Openbmc::Index::sensorThresholdCriticalLowGoingLow,
        std::to_array({arg1, arg2, arg3}));
}

void sensorThresholdCriticalLowGoingLow(
    crow::Response& res, std::string_view arg1, std::string_view arg2,
    std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          sensorThresholdCriticalLowGoingLow(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats SensorThresholdWarningHighGoingHigh message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t sensorThresholdWarningHighGoingHigh(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(redfish::registries::Openbmc::Index::
                      sensorThresholdWarningHighGoingHigh,
                  std::to_array({arg1, arg2, arg3}));
}

void sensorThresholdWarningHighGoingHigh(
    crow::Response& res, std::string_view arg1, std::string_view arg2,
    std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(
        res.jsonValue, sensorThresholdWarningHighGoingHigh(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats SensorThresholdWarningHighGoingLow message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t sensorThresholdWarningHighGoingLow(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(
        redfish::registries::Openbmc::Index::sensorThresholdWarningHighGoingLow,
        std::to_array({arg1, arg2, arg3}));
}

void sensorThresholdWarningHighGoingLow(
    crow::Response& res, std::string_view arg1, std::string_view arg2,
    std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          sensorThresholdWarningHighGoingLow(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats SensorThresholdWarningLowGoingHigh message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t sensorThresholdWarningLowGoingHigh(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(
        redfish::registries::Openbmc::Index::sensorThresholdWarningLowGoingHigh,
        std::to_array({arg1, arg2, arg3}));
}

void sensorThresholdWarningLowGoingHigh(
    crow::Response& res, std::string_view arg1, std::string_view arg2,
    std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          sensorThresholdWarningLowGoingHigh(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats SensorThresholdWarningLowGoingLow message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t sensorThresholdWarningLowGoingLow(
    std::string_view arg1, std::string_view arg2, std::string_view arg3)
{
    return getLog(
        redfish::registries::Openbmc::Index::sensorThresholdWarningLowGoingLow,
        std::to_array({arg1, arg2, arg3}));
}

void sensorThresholdWarningLowGoingLow(
    crow::Response& res, std::string_view arg1, std::string_view arg2,
    std::string_view arg3)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          sensorThresholdWarningLowGoingLow(arg1, arg2, arg3));
}

/**
 * @internal
 * @brief Formats ServiceFailure message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t serviceFailure(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::serviceFailure,
                  std::to_array({arg1}));
}

void serviceFailure(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, serviceFailure(arg1));
}

/**
 * @internal
 * @brief Formats ServiceStarted message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t serviceStarted(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::serviceStarted,
                  std::to_array({arg1}));
}

void serviceStarted(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, serviceStarted(arg1));
}

/**
 * @internal
 * @brief Formats SparingRedundancyDegraded message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t sparingRedundancyDegraded(
    std::string_view arg1, std::string_view arg2, std::string_view arg3,
    std::string_view arg4, std::string_view arg5)
{
    return getLog(
        redfish::registries::Openbmc::Index::sparingRedundancyDegraded,
        std::to_array({arg1, arg2, arg3, arg4, arg5}));
}

void sparingRedundancyDegraded(crow::Response& res, std::string_view arg1,
                               std::string_view arg2, std::string_view arg3,
                               std::string_view arg4, std::string_view arg5)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(
        res.jsonValue, sparingRedundancyDegraded(arg1, arg2, arg3, arg4, arg5));
}

/**
 * @internal
 * @brief Formats SparingRedundancyFull message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t sparingRedundancyFull(
    std::string_view arg1, std::string_view arg2, std::string_view arg3,
    std::string_view arg4, std::string_view arg5)
{
    return getLog(redfish::registries::Openbmc::Index::sparingRedundancyFull,
                  std::to_array({arg1, arg2, arg3, arg4, arg5}));
}

void sparingRedundancyFull(crow::Response& res, std::string_view arg1,
                           std::string_view arg2, std::string_view arg3,
                           std::string_view arg4, std::string_view arg5)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue,
                          sparingRedundancyFull(arg1, arg2, arg3, arg4, arg5));
}

/**
 * @internal
 * @brief Formats SsbThermalTrip message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t ssbThermalTrip()
{
    return getLog(redfish::registries::Openbmc::Index::ssbThermalTrip, {});
}

void ssbThermalTrip(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, ssbThermalTrip());
}

/**
 * @internal
 * @brief Formats SystemInterfaceDisabledProvisioned message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t systemInterfaceDisabledProvisioned()
{
    return getLog(
        redfish::registries::Openbmc::Index::systemInterfaceDisabledProvisioned,
        {});
}

void systemInterfaceDisabledProvisioned(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, systemInterfaceDisabledProvisioned());
}

/**
 * @internal
 * @brief Formats SystemInterfaceUnprovisioned message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t systemInterfaceUnprovisioned()
{
    return getLog(
        redfish::registries::Openbmc::Index::systemInterfaceUnprovisioned, {});
}

void systemInterfaceUnprovisioned(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, systemInterfaceUnprovisioned());
}

/**
 * @internal
 * @brief Formats SystemInterfaceWhitelistProvisioned message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t systemInterfaceWhitelistProvisioned()
{
    return getLog(redfish::registries::Openbmc::Index::
                      systemInterfaceWhitelistProvisioned,
                  {});
}

void systemInterfaceWhitelistProvisioned(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, systemInterfaceWhitelistProvisioned());
}

/**
 * @internal
 * @brief Formats SystemPowerGoodFailed message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t systemPowerGoodFailed(std::string_view arg1)
{
    return getLog(redfish::registries::Openbmc::Index::systemPowerGoodFailed,
                  std::to_array({arg1}));
}

void systemPowerGoodFailed(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, systemPowerGoodFailed(arg1));
}

/**
 * @internal
 * @brief Formats SystemPowerLost message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t systemPowerLost()
{
    return getLog(redfish::registries::Openbmc::Index::systemPowerLost, {});
}

void systemPowerLost(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, systemPowerLost());
}

/**
 * @internal
 * @brief Formats SystemPowerOffFailed message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t systemPowerOffFailed()
{
    return getLog(redfish::registries::Openbmc::Index::systemPowerOffFailed,
                  {});
}

void systemPowerOffFailed(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, systemPowerOffFailed());
}

/**
 * @internal
 * @brief Formats SystemPowerOnFailed message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t systemPowerOnFailed()
{
    return getLog(redfish::registries::Openbmc::Index::systemPowerOnFailed, {});
}

void systemPowerOnFailed(crow::Response& res)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, systemPowerOnFailed());
}

/**
 * @internal
 * @brief Formats VoltageRegulatorOverheated message into JSON
 *
 * See header file for more information
 * @endinternal
 */
nlohmann::json::object_t voltageRegulatorOverheated(std::string_view arg1)
{
    return getLog(
        redfish::registries::Openbmc::Index::voltageRegulatorOverheated,
        std::to_array({arg1}));
}

void voltageRegulatorOverheated(crow::Response& res, std::string_view arg1)
{
    res.result(boost::beast::http::status::bad_request);
    addMessageToErrorJson(res.jsonValue, voltageRegulatorOverheated(arg1));
}

} // namespace messages
} // namespace redfish
