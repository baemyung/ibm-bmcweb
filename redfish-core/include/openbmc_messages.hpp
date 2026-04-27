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
nlohmann::json::object_t aDDDCCorrectable(
    std::string_view arg1, std::string_view arg2, std::string_view arg3,
    std::string_view arg4);

void aDDDCCorrectable(crow::Response& res, std::string_view arg1,
                      std::string_view arg2, std::string_view arg3,
                      std::string_view arg4);

nlohmann::json::object_t atScaleDebugConnected(std::string_view arg1);

void atScaleDebugConnected(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t atScaleDebugConnectionFailed();

void atScaleDebugConnectionFailed(crow::Response& res);

nlohmann::json::object_t atScaleDebugDisconnected();

void atScaleDebugDisconnected(crow::Response& res);

nlohmann::json::object_t atScaleDebugFeatureDisabled();

void atScaleDebugFeatureDisabled(crow::Response& res);

nlohmann::json::object_t atScaleDebugFeatureDisabledAtHardware();

void atScaleDebugFeatureDisabledAtHardware(crow::Response& res);

nlohmann::json::object_t atScaleDebugFeatureEnabled();

void atScaleDebugFeatureEnabled(crow::Response& res);

nlohmann::json::object_t atScaleDebugFeatureEnabledAtHardware();

void atScaleDebugFeatureEnabledAtHardware(crow::Response& res);

nlohmann::json::object_t atScaleDebugSpecialUserDisabled();

void atScaleDebugSpecialUserDisabled(crow::Response& res);

nlohmann::json::object_t atScaleDebugSpecialUserEnabled();

void atScaleDebugSpecialUserEnabled(crow::Response& res);

nlohmann::json::object_t auditLogEntry(std::string_view arg1);

void auditLogEntry(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t auditLogUsysConfig(
    std::string_view arg1, std::string_view arg2, std::string_view arg3,
    std::string_view arg4, std::string_view arg5, std::string_view arg6,
    std::string_view arg7, std::string_view arg8);

void auditLogUsysConfig(
    crow::Response& res, std::string_view arg1, std::string_view arg2,
    std::string_view arg3, std::string_view arg4, std::string_view arg5,
    std::string_view arg6, std::string_view arg7, std::string_view arg8);

nlohmann::json::object_t bIOSAttributesChanged();

void bIOSAttributesChanged(crow::Response& res);

nlohmann::json::object_t bIOSBoot();

void bIOSBoot(crow::Response& res);

nlohmann::json::object_t bIOSFirmwarePanicReason(std::string_view arg1);

void bIOSFirmwarePanicReason(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t bIOSFirmwareRecoveryReason(std::string_view arg1);

void bIOSFirmwareRecoveryReason(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t bIOSFirmwareResiliencyError(std::string_view arg1);

void bIOSFirmwareResiliencyError(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t bIOSPOSTCode(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void bIOSPOSTCode(crow::Response& res, std::string_view arg1,
                  std::string_view arg2, std::string_view arg3);

nlohmann::json::object_t bIOSPOSTError(std::string_view arg1);

void bIOSPOSTError(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t bIOSRecoveryComplete();

void bIOSRecoveryComplete(crow::Response& res);

nlohmann::json::object_t bIOSRecoveryStart();

void bIOSRecoveryStart(crow::Response& res);

nlohmann::json::object_t bMCBootReason(std::string_view arg1,
                                       std::string_view arg2);

void bMCBootReason(crow::Response& res, std::string_view arg1,
                   std::string_view arg2);

nlohmann::json::object_t bMCFirmwarePanicReason(std::string_view arg1);

void bMCFirmwarePanicReason(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t bMCFirmwareRecoveryReason(std::string_view arg1);

void bMCFirmwareRecoveryReason(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t bMCFirmwareResiliencyError(std::string_view arg1);

void bMCFirmwareResiliencyError(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t bMCKernelPanic();

void bMCKernelPanic(crow::Response& res);

nlohmann::json::object_t cPLDFirmwarePanicReason(std::string_view arg1);

void cPLDFirmwarePanicReason(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t cPLDFirmwareRecoveryReason(std::string_view arg1);

void cPLDFirmwareRecoveryReason(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t cPLDFirmwareResiliencyError(std::string_view arg1);

void cPLDFirmwareResiliencyError(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t cPUError(std::string_view arg1);

void cPUError(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t cPUMismatch(std::string_view arg1);

void cPUMismatch(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t cPUThermalTrip(std::string_view arg1);

void cPUThermalTrip(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t chassisIntrusionDetected();

void chassisIntrusionDetected(crow::Response& res);

nlohmann::json::object_t chassisIntrusionReset();

void chassisIntrusionReset(crow::Response& res);

nlohmann::json::object_t componentOverTemperature(std::string_view arg1);

void componentOverTemperature(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t dCPowerOff();

void dCPowerOff(crow::Response& res);

nlohmann::json::object_t dCPowerOn();

void dCPowerOn(crow::Response& res);

nlohmann::json::object_t driveError(std::string_view arg1);

void driveError(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t eventLogCleared();

void eventLogCleared(crow::Response& res);

nlohmann::json::object_t fanInserted(std::string_view arg1);

void fanInserted(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t fanRedundancyLost();

void fanRedundancyLost(crow::Response& res);

nlohmann::json::object_t fanRedundancyRegained();

void fanRedundancyRegained(crow::Response& res);

nlohmann::json::object_t fanRemoved();

void fanRemoved(crow::Response& res);

nlohmann::json::object_t firmwareActivationCompleted(std::string_view arg1);

void firmwareActivationCompleted(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t firmwareActivationFailed(std::string_view arg1,
                                                  std::string_view arg2);

void firmwareActivationFailed(crow::Response& res, std::string_view arg1,
                              std::string_view arg2);

nlohmann::json::object_t firmwareActivationStarted(std::string_view arg1);

void firmwareActivationStarted(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t firmwareResiliencyError(std::string_view arg1);

void firmwareResiliencyError(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t firmwareUpdateCompleted(std::string_view arg1,
                                                 std::string_view arg2);

void firmwareUpdateCompleted(crow::Response& res, std::string_view arg1,
                             std::string_view arg2);

nlohmann::json::object_t firmwareUpdateFailed(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void firmwareUpdateFailed(crow::Response& res, std::string_view arg1,
                          std::string_view arg2, std::string_view arg3);

nlohmann::json::object_t firmwareUpdateStaged(std::string_view arg1,
                                              std::string_view arg2);

void firmwareUpdateStaged(crow::Response& res, std::string_view arg1,
                          std::string_view arg2);

nlohmann::json::object_t firmwareUpdateStarted(std::string_view arg1,
                                               std::string_view arg2);

void firmwareUpdateStarted(crow::Response& res, std::string_view arg1,
                           std::string_view arg2);

nlohmann::json::object_t generalFirmwareSecurityViolation(
    std::string_view arg1);

void generalFirmwareSecurityViolation(crow::Response& res,
                                      std::string_view arg1);

nlohmann::json::object_t guardRecord(std::string_view arg1,
                                     std::string_view arg2);

void guardRecord(crow::Response& res, std::string_view arg1,
                 std::string_view arg2);

nlohmann::json::object_t hardwareIsolationReason(std::string_view arg1);

void hardwareIsolationReason(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t iPMIWatchdog(std::string_view arg1);

void iPMIWatchdog(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t intelUPILinkWidthReducedToHalf(std::string_view arg1);

void intelUPILinkWidthReducedToHalf(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t intelUPILinkWidthReducedToQuarter(
    std::string_view arg1);

void intelUPILinkWidthReducedToQuarter(crow::Response& res,
                                       std::string_view arg1);

nlohmann::json::object_t invalidLoginAttempted(std::string_view arg1);

void invalidLoginAttempted(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t invalidUpload(std::string_view arg1,
                                       std::string_view arg2);

void invalidUpload(crow::Response& res, std::string_view arg1,
                   std::string_view arg2);

nlohmann::json::object_t inventoryAdded(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void inventoryAdded(crow::Response& res, std::string_view arg1,
                    std::string_view arg2, std::string_view arg3);

nlohmann::json::object_t inventoryRemoved(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void inventoryRemoved(crow::Response& res, std::string_view arg1,
                      std::string_view arg2, std::string_view arg3);

nlohmann::json::object_t lanLost(std::string_view arg1);

void lanLost(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t lanRegained(std::string_view arg1);

void lanRegained(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t legacyPCIPERR(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void legacyPCIPERR(crow::Response& res, std::string_view arg1,
                   std::string_view arg2, std::string_view arg3);

nlohmann::json::object_t legacyPCISERR(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void legacyPCISERR(crow::Response& res, std::string_view arg1,
                   std::string_view arg2, std::string_view arg3);

nlohmann::json::object_t mEAutoConfigFailed(std::string_view arg1);

void mEAutoConfigFailed(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t mEAutoConfigSuccess(
    std::string_view arg1, std::string_view arg2, std::string_view arg3,
    std::string_view arg4);

void mEAutoConfigSuccess(crow::Response& res, std::string_view arg1,
                         std::string_view arg2, std::string_view arg3,
                         std::string_view arg4);

nlohmann::json::object_t mEBootGuardHealthEvent(std::string_view arg1);

void mEBootGuardHealthEvent(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t mECpuDebugCapabilityDisabled();

void mECpuDebugCapabilityDisabled(crow::Response& res);

nlohmann::json::object_t mEDirectFlashUpdateRequested();

void mEDirectFlashUpdateRequested(crow::Response& res);

nlohmann::json::object_t mEExceptionDuringShutdown();

void mEExceptionDuringShutdown(crow::Response& res);

nlohmann::json::object_t mEFactoryResetError(std::string_view arg1);

void mEFactoryResetError(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t mEFactoryRestore(std::string_view arg1);

void mEFactoryRestore(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t mEFirmwareException(std::string_view arg1);

void mEFirmwareException(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t mEFirmwarePanicReason(std::string_view arg1);

void mEFirmwarePanicReason(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t mEFirmwareRecoveryReason(std::string_view arg1);

void mEFirmwareRecoveryReason(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t mEFirmwareResiliencyError(std::string_view arg1);

void mEFirmwareResiliencyError(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t mEFlashEraseError();

void mEFlashEraseError(crow::Response& res);

nlohmann::json::object_t mEFlashStateInformation(std::string_view arg1);

void mEFlashStateInformation(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t mEFlashStateInformationWritingEnabled(
    std::string_view arg1);

void mEFlashStateInformationWritingEnabled(crow::Response& res,
                                           std::string_view arg1);

nlohmann::json::object_t mEFlashVerificationError(std::string_view arg1);

void mEFlashVerificationError(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t mEFlashWearOutWarning(std::string_view arg1);

void mEFlashWearOutWarning(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t mEImageExecutionFailed();

void mEImageExecutionFailed(crow::Response& res);

nlohmann::json::object_t mEInternalError();

void mEInternalError(crow::Response& res);

nlohmann::json::object_t mEManufacturingError(std::string_view arg1);

void mEManufacturingError(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t mEMctpInterfaceError(std::string_view arg1);

void mEMctpInterfaceError(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t mEMultiPchModeMisconfig(std::string_view arg1);

void mEMultiPchModeMisconfig(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t mEPeciOverDmiError(std::string_view arg1);

void mEPeciOverDmiError(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t mEPttHealthEvent(std::string_view arg1);

void mEPttHealthEvent(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t mERecoveryGpioForced();

void mERecoveryGpioForced(crow::Response& res);

nlohmann::json::object_t mERestrictedMode(std::string_view arg1);

void mERestrictedMode(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t mESmbusLinkFailure(std::string_view arg1,
                                            std::string_view arg2);

void mESmbusLinkFailure(crow::Response& res, std::string_view arg1,
                        std::string_view arg2);

nlohmann::json::object_t mEUmaError(std::string_view arg1);

void mEUmaError(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t mEUnsupportedFeature(std::string_view arg1);

void mEUnsupportedFeature(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t manufacturingModeEntered();

void manufacturingModeEntered(crow::Response& res);

nlohmann::json::object_t manufacturingModeExited();

void manufacturingModeExited(crow::Response& res);

nlohmann::json::object_t memoryECCCorrectable(
    std::string_view arg1, std::string_view arg2, std::string_view arg3,
    std::string_view arg4);

void memoryECCCorrectable(crow::Response& res, std::string_view arg1,
                          std::string_view arg2, std::string_view arg3,
                          std::string_view arg4);

nlohmann::json::object_t memoryECCUncorrectable(
    std::string_view arg1, std::string_view arg2, std::string_view arg3,
    std::string_view arg4);

void memoryECCUncorrectable(crow::Response& res, std::string_view arg1,
                            std::string_view arg2, std::string_view arg3,
                            std::string_view arg4);

nlohmann::json::object_t memoryParityCommandAndAddress(
    std::string_view arg1, std::string_view arg2, std::string_view arg3,
    std::string_view arg4, std::string_view arg5);

void memoryParityCommandAndAddress(
    crow::Response& res, std::string_view arg1, std::string_view arg2,
    std::string_view arg3, std::string_view arg4, std::string_view arg5);

nlohmann::json::object_t memoryParityNotKnown(
    std::string_view arg1, std::string_view arg2, std::string_view arg3,
    std::string_view arg4, std::string_view arg5);

void memoryParityNotKnown(crow::Response& res, std::string_view arg1,
                          std::string_view arg2, std::string_view arg3,
                          std::string_view arg4, std::string_view arg5);

nlohmann::json::object_t memoryRASConfigurationDisabled(std::string_view arg1,
                                                        std::string_view arg2);

void memoryRASConfigurationDisabled(crow::Response& res, std::string_view arg1,
                                    std::string_view arg2);

nlohmann::json::object_t memoryRASConfigurationEnabled(std::string_view arg1,
                                                       std::string_view arg2);

void memoryRASConfigurationEnabled(crow::Response& res, std::string_view arg1,
                                   std::string_view arg2);

nlohmann::json::object_t memoryRASModeDisabled(std::string_view arg1,
                                               std::string_view arg2);

void memoryRASModeDisabled(crow::Response& res, std::string_view arg1,
                           std::string_view arg2);

nlohmann::json::object_t memoryRASModeEnabled(std::string_view arg1,
                                              std::string_view arg2);

void memoryRASModeEnabled(crow::Response& res, std::string_view arg1,
                          std::string_view arg2);

nlohmann::json::object_t memoryThermTrip(std::string_view arg1);

void memoryThermTrip(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t mirroringRedundancyDegraded(
    std::string_view arg1, std::string_view arg2, std::string_view arg3,
    std::string_view arg4, std::string_view arg5);

void mirroringRedundancyDegraded(crow::Response& res, std::string_view arg1,
                                 std::string_view arg2, std::string_view arg3,
                                 std::string_view arg4, std::string_view arg5);

nlohmann::json::object_t mirroringRedundancyFull(
    std::string_view arg1, std::string_view arg2, std::string_view arg3,
    std::string_view arg4, std::string_view arg5);

void mirroringRedundancyFull(crow::Response& res, std::string_view arg1,
                             std::string_view arg2, std::string_view arg3,
                             std::string_view arg4, std::string_view arg5);

nlohmann::json::object_t nMIButtonPressed();

void nMIButtonPressed(crow::Response& res);

nlohmann::json::object_t nMIDiagnosticInterrupt();

void nMIDiagnosticInterrupt(crow::Response& res);

nlohmann::json::object_t pCIeCorrectableAdvisoryNonFatal(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void pCIeCorrectableAdvisoryNonFatal(crow::Response& res, std::string_view arg1,
                                     std::string_view arg2,
                                     std::string_view arg3);

nlohmann::json::object_t pCIeCorrectableBadDLLP(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void pCIeCorrectableBadDLLP(crow::Response& res, std::string_view arg1,
                            std::string_view arg2, std::string_view arg3);

nlohmann::json::object_t pCIeCorrectableBadTLP(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void pCIeCorrectableBadTLP(crow::Response& res, std::string_view arg1,
                           std::string_view arg2, std::string_view arg3);

nlohmann::json::object_t pCIeCorrectableHeaderLogOverflow(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void pCIeCorrectableHeaderLogOverflow(
    crow::Response& res, std::string_view arg1, std::string_view arg2,
    std::string_view arg3);

nlohmann::json::object_t pCIeCorrectableInternal(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void pCIeCorrectableInternal(crow::Response& res, std::string_view arg1,
                             std::string_view arg2, std::string_view arg3);

nlohmann::json::object_t pCIeCorrectableLinkBWChanged(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void pCIeCorrectableLinkBWChanged(crow::Response& res, std::string_view arg1,
                                  std::string_view arg2, std::string_view arg3);

nlohmann::json::object_t pCIeCorrectableReceiverError(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void pCIeCorrectableReceiverError(crow::Response& res, std::string_view arg1,
                                  std::string_view arg2, std::string_view arg3);

nlohmann::json::object_t pCIeCorrectableReplayNumRollover(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void pCIeCorrectableReplayNumRollover(
    crow::Response& res, std::string_view arg1, std::string_view arg2,
    std::string_view arg3);

nlohmann::json::object_t pCIeCorrectableReplayTimerTimeout(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void pCIeCorrectableReplayTimerTimeout(
    crow::Response& res, std::string_view arg1, std::string_view arg2,
    std::string_view arg3);

nlohmann::json::object_t pCIeCorrectableUnspecifiedAERError(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void pCIeCorrectableUnspecifiedAERError(
    crow::Response& res, std::string_view arg1, std::string_view arg2,
    std::string_view arg3);

nlohmann::json::object_t pCIeFatalACSViolation(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void pCIeFatalACSViolation(crow::Response& res, std::string_view arg1,
                           std::string_view arg2, std::string_view arg3);

nlohmann::json::object_t pCIeFatalAtomicEgressBlocked(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void pCIeFatalAtomicEgressBlocked(crow::Response& res, std::string_view arg1,
                                  std::string_view arg2, std::string_view arg3);

nlohmann::json::object_t pCIeFatalCompleterAbort(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void pCIeFatalCompleterAbort(crow::Response& res, std::string_view arg1,
                             std::string_view arg2, std::string_view arg3);

nlohmann::json::object_t pCIeFatalCompletionTimeout(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void pCIeFatalCompletionTimeout(crow::Response& res, std::string_view arg1,
                                std::string_view arg2, std::string_view arg3);

nlohmann::json::object_t pCIeFatalDataLinkLayerProtocol(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void pCIeFatalDataLinkLayerProtocol(crow::Response& res, std::string_view arg1,
                                    std::string_view arg2,
                                    std::string_view arg3);

nlohmann::json::object_t pCIeFatalECRCError(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void pCIeFatalECRCError(crow::Response& res, std::string_view arg1,
                        std::string_view arg2, std::string_view arg3);

nlohmann::json::object_t pCIeFatalFlowControlProtocol(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void pCIeFatalFlowControlProtocol(crow::Response& res, std::string_view arg1,
                                  std::string_view arg2, std::string_view arg3);

nlohmann::json::object_t pCIeFatalMCBlockedTLP(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void pCIeFatalMCBlockedTLP(crow::Response& res, std::string_view arg1,
                           std::string_view arg2, std::string_view arg3);

nlohmann::json::object_t pCIeFatalMalformedTLP(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void pCIeFatalMalformedTLP(crow::Response& res, std::string_view arg1,
                           std::string_view arg2, std::string_view arg3);

nlohmann::json::object_t pCIeFatalPoisonedTLP(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void pCIeFatalPoisonedTLP(crow::Response& res, std::string_view arg1,
                          std::string_view arg2, std::string_view arg3);

nlohmann::json::object_t pCIeFatalReceivedErrNonFatalMessage(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void pCIeFatalReceivedErrNonFatalMessage(
    crow::Response& res, std::string_view arg1, std::string_view arg2,
    std::string_view arg3);

nlohmann::json::object_t pCIeFatalReceivedFatalMessageFromDownstream(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void pCIeFatalReceivedFatalMessageFromDownstream(
    crow::Response& res, std::string_view arg1, std::string_view arg2,
    std::string_view arg3);

nlohmann::json::object_t pCIeFatalReceiverBufferOverflow(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void pCIeFatalReceiverBufferOverflow(crow::Response& res, std::string_view arg1,
                                     std::string_view arg2,
                                     std::string_view arg3);

nlohmann::json::object_t pCIeFatalSurpriseLinkDown(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void pCIeFatalSurpriseLinkDown(crow::Response& res, std::string_view arg1,
                               std::string_view arg2, std::string_view arg3);

nlohmann::json::object_t pCIeFatalTLPPrefixBlocked(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void pCIeFatalTLPPrefixBlocked(crow::Response& res, std::string_view arg1,
                               std::string_view arg2, std::string_view arg3);

nlohmann::json::object_t pCIeFatalUncorrectableInternal(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void pCIeFatalUncorrectableInternal(crow::Response& res, std::string_view arg1,
                                    std::string_view arg2,
                                    std::string_view arg3);

nlohmann::json::object_t pCIeFatalUnexpectedCompletion(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void pCIeFatalUnexpectedCompletion(crow::Response& res, std::string_view arg1,
                                   std::string_view arg2,
                                   std::string_view arg3);

nlohmann::json::object_t pCIeFatalUnspecifiedNonAERFatalError(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void pCIeFatalUnspecifiedNonAERFatalError(
    crow::Response& res, std::string_view arg1, std::string_view arg2,
    std::string_view arg3);

nlohmann::json::object_t pCIeFatalUnsupportedRequest(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void pCIeFatalUnsupportedRequest(crow::Response& res, std::string_view arg1,
                                 std::string_view arg2, std::string_view arg3);

nlohmann::json::object_t powerButtonPressed();

void powerButtonPressed(crow::Response& res);

nlohmann::json::object_t powerButtonLongPressed();

void powerButtonLongPressed(crow::Response& res);

nlohmann::json::object_t powerRestorePolicyApplied();

void powerRestorePolicyApplied(crow::Response& res);

nlohmann::json::object_t powerSupplyConfigurationError(std::string_view arg1);

void powerSupplyConfigurationError(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t powerSupplyConfigurationErrorRecovered(
    std::string_view arg1);

void powerSupplyConfigurationErrorRecovered(crow::Response& res,
                                            std::string_view arg1);

nlohmann::json::object_t powerSupplyFailed(std::string_view arg1);

void powerSupplyFailed(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t powerSupplyFailurePredicted(std::string_view arg1);

void powerSupplyFailurePredicted(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t powerSupplyFanFailed(std::string_view arg1,
                                              std::string_view arg2);

void powerSupplyFanFailed(crow::Response& res, std::string_view arg1,
                          std::string_view arg2);

nlohmann::json::object_t powerSupplyFanRecovered(std::string_view arg1,
                                                 std::string_view arg2);

void powerSupplyFanRecovered(crow::Response& res, std::string_view arg1,
                             std::string_view arg2);

nlohmann::json::object_t powerSupplyInserted(std::string_view arg1);

void powerSupplyInserted(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t powerSupplyPowerGoodFailed(std::string_view arg1);

void powerSupplyPowerGoodFailed(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t powerSupplyPowerLost(std::string_view arg1);

void powerSupplyPowerLost(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t powerSupplyPowerRestored(std::string_view arg1);

void powerSupplyPowerRestored(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t powerSupplyPredictedFailureRecovered(
    std::string_view arg1);

void powerSupplyPredictedFailureRecovered(crow::Response& res,
                                          std::string_view arg1);

nlohmann::json::object_t powerSupplyRecovered(std::string_view arg1);

void powerSupplyRecovered(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t powerSupplyRemoved(std::string_view arg1);

void powerSupplyRemoved(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t powerUnitDegradedFromNonRedundant();

void powerUnitDegradedFromNonRedundant(crow::Response& res);

nlohmann::json::object_t powerUnitDegradedFromRedundant();

void powerUnitDegradedFromRedundant(crow::Response& res);

nlohmann::json::object_t powerUnitNonRedundantFromInsufficient();

void powerUnitNonRedundantFromInsufficient(crow::Response& res);

nlohmann::json::object_t powerUnitNonRedundantInsufficient();

void powerUnitNonRedundantInsufficient(crow::Response& res);

nlohmann::json::object_t powerUnitNonRedundantSufficient();

void powerUnitNonRedundantSufficient(crow::Response& res);

nlohmann::json::object_t powerUnitRedundancyDegraded();

void powerUnitRedundancyDegraded(crow::Response& res);

nlohmann::json::object_t powerUnitRedundancyLost();

void powerUnitRedundancyLost(crow::Response& res);

nlohmann::json::object_t powerUnitRedundancyRegained();

void powerUnitRedundancyRegained(crow::Response& res);

nlohmann::json::object_t resetButtonPressed();

void resetButtonPressed(crow::Response& res);

nlohmann::json::object_t sELEntryAdded(std::string_view arg1);

void sELEntryAdded(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t securityBoot2ndFlashEnabled();

void securityBoot2ndFlashEnabled(crow::Response& res);

nlohmann::json::object_t securityP2aBridgeEnabled();

void securityP2aBridgeEnabled(crow::Response& res);

nlohmann::json::object_t securityUartPortDebugEnabled();

void securityUartPortDebugEnabled(crow::Response& res);

nlohmann::json::object_t securityUserNonRootUidZeroAssigned();

void securityUserNonRootUidZeroAssigned(crow::Response& res);

nlohmann::json::object_t securityUserNonRootUidZeroRemoved();

void securityUserNonRootUidZeroRemoved(crow::Response& res);

nlohmann::json::object_t securityUserRootDisabled();

void securityUserRootDisabled(crow::Response& res);

nlohmann::json::object_t securityUserRootEnabled();

void securityUserRootEnabled(crow::Response& res);

nlohmann::json::object_t securityUserStrongHashAlgoRestored();

void securityUserStrongHashAlgoRestored(crow::Response& res);

nlohmann::json::object_t securityUserUnsupportedShellEnabled();

void securityUserUnsupportedShellEnabled(crow::Response& res);

nlohmann::json::object_t securityUserUnsupportedShellRemoved();

void securityUserUnsupportedShellRemoved(crow::Response& res);

nlohmann::json::object_t securityUserWeakHashAlgoEnabled();

void securityUserWeakHashAlgoEnabled(crow::Response& res);

nlohmann::json::object_t sensorThresholdCriticalHighGoingHigh(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void sensorThresholdCriticalHighGoingHigh(
    crow::Response& res, std::string_view arg1, std::string_view arg2,
    std::string_view arg3);

nlohmann::json::object_t sensorThresholdCriticalHighGoingLow(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void sensorThresholdCriticalHighGoingLow(
    crow::Response& res, std::string_view arg1, std::string_view arg2,
    std::string_view arg3);

nlohmann::json::object_t sensorThresholdCriticalLowGoingHigh(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void sensorThresholdCriticalLowGoingHigh(
    crow::Response& res, std::string_view arg1, std::string_view arg2,
    std::string_view arg3);

nlohmann::json::object_t sensorThresholdCriticalLowGoingLow(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void sensorThresholdCriticalLowGoingLow(
    crow::Response& res, std::string_view arg1, std::string_view arg2,
    std::string_view arg3);

nlohmann::json::object_t sensorThresholdWarningHighGoingHigh(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void sensorThresholdWarningHighGoingHigh(
    crow::Response& res, std::string_view arg1, std::string_view arg2,
    std::string_view arg3);

nlohmann::json::object_t sensorThresholdWarningHighGoingLow(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void sensorThresholdWarningHighGoingLow(
    crow::Response& res, std::string_view arg1, std::string_view arg2,
    std::string_view arg3);

nlohmann::json::object_t sensorThresholdWarningLowGoingHigh(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void sensorThresholdWarningLowGoingHigh(
    crow::Response& res, std::string_view arg1, std::string_view arg2,
    std::string_view arg3);

nlohmann::json::object_t sensorThresholdWarningLowGoingLow(
    std::string_view arg1, std::string_view arg2, std::string_view arg3);

void sensorThresholdWarningLowGoingLow(
    crow::Response& res, std::string_view arg1, std::string_view arg2,
    std::string_view arg3);

nlohmann::json::object_t serviceFailure(std::string_view arg1);

void serviceFailure(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t serviceStarted(std::string_view arg1);

void serviceStarted(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t sparingRedundancyDegraded(
    std::string_view arg1, std::string_view arg2, std::string_view arg3,
    std::string_view arg4, std::string_view arg5);

void sparingRedundancyDegraded(crow::Response& res, std::string_view arg1,
                               std::string_view arg2, std::string_view arg3,
                               std::string_view arg4, std::string_view arg5);

nlohmann::json::object_t sparingRedundancyFull(
    std::string_view arg1, std::string_view arg2, std::string_view arg3,
    std::string_view arg4, std::string_view arg5);

void sparingRedundancyFull(crow::Response& res, std::string_view arg1,
                           std::string_view arg2, std::string_view arg3,
                           std::string_view arg4, std::string_view arg5);

nlohmann::json::object_t ssbThermalTrip();

void ssbThermalTrip(crow::Response& res);

nlohmann::json::object_t systemInterfaceDisabledProvisioned();

void systemInterfaceDisabledProvisioned(crow::Response& res);

nlohmann::json::object_t systemInterfaceUnprovisioned();

void systemInterfaceUnprovisioned(crow::Response& res);

nlohmann::json::object_t systemInterfaceWhitelistProvisioned();

void systemInterfaceWhitelistProvisioned(crow::Response& res);

nlohmann::json::object_t systemPowerGoodFailed(std::string_view arg1);

void systemPowerGoodFailed(crow::Response& res, std::string_view arg1);

nlohmann::json::object_t systemPowerLost();

void systemPowerLost(crow::Response& res);

nlohmann::json::object_t systemPowerOffFailed();

void systemPowerOffFailed(crow::Response& res);

nlohmann::json::object_t systemPowerOnFailed();

void systemPowerOnFailed(crow::Response& res);

nlohmann::json::object_t voltageRegulatorOverheated(std::string_view arg1);

void voltageRegulatorOverheated(crow::Response& res, std::string_view arg1);

} // namespace messages
} // namespace redfish
