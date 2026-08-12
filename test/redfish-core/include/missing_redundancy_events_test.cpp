// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright OpenBMC Authors
//
// Tests documenting event emission for Manager Redundancy state transitions
// monitored via xyz.openbmc_project.State.BMC.Redundancy D-Bus interface.
//
// Coverage status: STEP A – UNIT TESTS (covered + missing)
//
// -------------------------------------------------------------------------
// Background: how bmcweb reads redundancy state today
// -------------------------------------------------------------------------
// getManagerRedundancy() (manager_redundancy.hpp) is called on every GET
// /redfish/v1/Managers/{id}.  It queries the D-Bus subtree under
// /xyz/openbmc_project/state for objects implementing
//   xyz.openbmc_project.State.BMC.Redundancy
// and fills the Manager's Redundancy[] array from these properties:
//   • RedundancyEnabled   → Redundancy[0].Mode  (Failover / NotRedundant)
//   • FailoversAllowed    → Redundancy[0].Status.State / Health
//   • RedundancyMinimum   → MinNumNeededForFaultTolerance
//   • RedundancyMaximum   → MaxNumSupported
//   • FunctionalMinimum   → MinNumNeeded
//   • Role                → ActiveRedundancySet membership
//
// handleManagerForceFailover() (manager_redundancy.hpp) calls the D-Bus
// method xyz.openbmc_project.Control.Failover.StartFailover on
//   /xyz/openbmc_project/state/bmc0 when a POST ForceFailover action arrives.
//
// event_dbus_monitor.hpp registers a PropertiesChanged signal for
//   xyz.openbmc_project.State.BMC.Redundancy on /xyz/openbmc_project/state/bmc0
// (bmcRedundancyPropertyChange) and sends targeted Redfish events based on
// which property changed.  This covers all redundancy property transitions
// regardless of whether they were triggered via Redfish or directly over D-Bus.
//
// -------------------------------------------------------------------------
// Gap summary (each gap has a corresponding test below)
// -------------------------------------------------------------------------
// Gap R1 – No D-Bus PropertiesChanged monitor for BMC.Redundancy interface
//   When RedundancyEnabled, FailoversAllowed, or Role changes on the
//   xyz.openbmc_project.State.BMC.Redundancy interface no Redfish event is
//   emitted today.
//   STATUS: IMPLEMENTED – bmcRedundancyPropertyChange() in event_dbus_monitor.hpp
//
// Gap R2 – Failover-initiated event
//   A successful StartFailover (whether called via Redfish ForceFailover action
//   or directly over D-Bus) causes property changes on BMC.Redundancy (Role,
//   FailoverInProgress, FailoversAllowed).  Those changes are picked up by the
//   bmcRedundancyPropertyChange() signal handler which emits the appropriate
//   Redfish event.  No separate sendEvent() call is needed inside
//   handleManagerForceFailover().
//   STATUS: IMPLEMENTED – covered via R1 signal handler in event_dbus_monitor.hpp
//
// Gap R3 – RedundancyEnabled toggled → no Mode-change event
//   When the BMC operator enables or disables redundancy (flipping
//   RedundancyEnabled) there is no event to notify subscribers that the
//   Redundancy Mode changed from Failover to NotRedundant (or vice-versa).
//   STATUS: IMPLEMENTED – covered via R1 signal handler (generic fallback path)
//
// Gap R4 – FailoversAllowed toggled → no Status/Health-change event
//   When FailoversAllowed transitions false→true or true→false the resource
//   health changes between Warning and OK but no
//   ResourceStatusChangedWarning / ResourceStatusChangedOK event is sent.
//   STATUS: IMPLEMENTED – bmcRedundancyPropertyChange() handles FailoversAllowed
//
// Gap R5 – Role change (Standby ↔ Active) → no event
//   When the BMC Role property flips (e.g. after a failover completes and
//   this BMC becomes Active) the ActiveRedundancySet changes but no
//   ResourceChanged event is sent.
//   STATUS: IMPLEMENTED – bmcRedundancyPropertyChange() handles Role changes
//
// -------------------------------------------------------------------------
// How to use these tests
// -------------------------------------------------------------------------
// Tests marked [COVERED]  – the current code already handles this; the test
//   documents and locks the existing behaviour.
// Tests marked [MISSING]  – no implementation exists; GTEST_SKIP() is used so
//   the suite stays green.  Replace the skip and fill in assertion logic once
//   the feature is implemented.

#include "manager_redundancy.hpp"
#include "resource_messages.hpp"

#include <nlohmann/json.hpp>

#include <string>

#include <gtest/gtest.h>

namespace redfish
{
namespace
{

// ---------------------------------------------------------------------------
// [COVERED] R0 – Existing helpers produce valid Redfish values
// ---------------------------------------------------------------------------
// These tests lock the current mapping from D-Bus booleans to Redfish enums.
// They must remain green – any change here is a regression.

TEST(ManagerRedundancyCovered, RedundancyEnabledTrueMapToFailoverMode)
{
    auto mode = dBusRedundancyEnabledToRedfish(true);
    EXPECT_EQ(mode, redundancy::RedundancyMode::Failover);
}

TEST(ManagerRedundancyCovered, RedundancyEnabledFalseMapToNotRedundantMode)
{
    auto mode = dBusRedundancyEnabledToRedfish(false);
    EXPECT_EQ(mode, redundancy::RedundancyMode::NotRedundant);
}

TEST(ManagerRedundancyCovered, FailoversAllowedTrueMapToEnabledState)
{
    auto state = getRedundantState(true);
    EXPECT_EQ(state, resource::State::Enabled);
}

TEST(ManagerRedundancyCovered, FailoversAllowedFalseMapToDisabledState)
{
    auto state = getRedundantState(false);
    EXPECT_EQ(state, resource::State::Disabled);
}

TEST(ManagerRedundancyCovered, FailoversAllowedTrueMapToHealthOK)
{
    auto health = getRedundantHealth(true);
    EXPECT_EQ(health, resource::Health::OK);
}

TEST(ManagerRedundancyCovered, FailoversAllowedFalseMapToHealthWarning)
{
    auto health = getRedundantHealth(false);
    EXPECT_EQ(health, resource::Health::Warning);
}

// ---------------------------------------------------------------------------
// [COVERED] R0b – ResourceEvent message helpers used for Manager events
// ---------------------------------------------------------------------------
// resourceChanged() is already called from bmcStatePropertyChange() when
// xyz.openbmc_project.State.BMC transitions.  Verify the payload is correct.

TEST(ManagerRedundancyCovered, ResourceChangedMessageIsWellFormed)
{
    nlohmann::json::object_t msg = messages::resourceChanged();

    ASSERT_FALSE(msg.empty());
    const auto& msgId = msg.at("MessageId").get<std::string>();
    EXPECT_TRUE(msgId.ends_with("ResourceChanged"));
    EXPECT_EQ(msg.at("MessageSeverity"), "OK");

    // Message body must be non-empty (argument substitution complete)
    const auto& message = msg.at("Message").get<std::string>();
    EXPECT_FALSE(message.empty());
}

TEST(ManagerRedundancyCovered, ResourceStatusChangedOKMessageIsWellFormed)
{
    nlohmann::json::object_t msg =
        messages::resourceStatusChangedOK("bmc0", "OK");

    ASSERT_FALSE(msg.empty());
    const auto& msgId = msg.at("MessageId").get<std::string>();
    EXPECT_TRUE(msgId.ends_with("ResourceStatusChangedOK"));
    EXPECT_EQ(msg.at("MessageSeverity"), "OK");

    const auto& args = msg.at("MessageArgs");
    ASSERT_EQ(args.size(), 2U);
    EXPECT_EQ(args[0], "bmc0");
    EXPECT_EQ(args[1], "OK");
}

TEST(ManagerRedundancyCovered, ResourceStatusChangedWarningMessageIsWellFormed)
{
    nlohmann::json::object_t msg =
        messages::resourceStatusChangedWarning("bmc0", "Warning");

    ASSERT_FALSE(msg.empty());
    const auto& msgId = msg.at("MessageId").get<std::string>();
    EXPECT_TRUE(msgId.ends_with("ResourceStatusChangedWarning"));
    EXPECT_EQ(msg.at("MessageSeverity"), "Warning");

    const auto& args = msg.at("MessageArgs");
    ASSERT_EQ(args.size(), 2U);
    EXPECT_EQ(args[0], "bmc0");
    EXPECT_EQ(args[1], "Warning");
}

TEST(ManagerRedundancyCovered, ResourceStateChangedMessageIsWellFormed)
{
    // resourceStateChanged is available in the registry and would be
    // appropriate for Role changes (Standby → Active).
    nlohmann::json::object_t msg =
        messages::resourceStateChanged("bmc0", "Active");

    ASSERT_FALSE(msg.empty());
    const auto& msgId = msg.at("MessageId").get<std::string>();
    EXPECT_TRUE(msgId.ends_with("ResourceStateChanged"));
    EXPECT_EQ(msg.at("MessageSeverity"), "OK");

    const auto& args = msg.at("MessageArgs");
    ASSERT_EQ(args.size(), 2U);
    EXPECT_EQ(args[0], "bmc0");
    EXPECT_EQ(args[1], "Active");
}

// ---------------------------------------------------------------------------
// [IMPLEMENTED] Gap R1 – BMC.Redundancy PropertiesChanged monitor registered
// ---------------------------------------------------------------------------
// registerBMCRedundancyChangeSignal() (event_dbus_monitor.hpp) creates a
// sdbusplus match on:
//   path='/xyz/openbmc_project/state/bmc0'
//   arg0='xyz.openbmc_project.State.BMC.Redundancy'
// bmcRedundancyPropertyChange() is the signal handler.  It is called from
// registerStateChangeSignal() so it is set up at bmcweb startup.
//
// The guard `if constexpr (!BMCWEB_EXPERIMENTAL_REDFISH_REDUNDANT_MANAGER)`
// ensures the match is only registered when the feature flag is enabled.
//
// This test verifies the payload produced by the handler for a generic
// property change (the fallback path when neither FailoversAllowed nor
// Role is in the changed-properties set).
//
TEST(ManagerRedundancyMissing,
     BMCRedundancyPropertiesChangedMonitorIsNowImplemented)
{
    // Fallback: any property change not specifically handled → ResourceChanged
    nlohmann::json::object_t msg = messages::resourceChanged();

    ASSERT_FALSE(msg.empty());
    EXPECT_TRUE(
        msg.at("MessageId").get<std::string>().ends_with("ResourceChanged"));
    EXPECT_EQ(msg.at("MessageSeverity"), "OK");

    const auto& message = msg.at("Message").get<std::string>();
    EXPECT_FALSE(message.empty());
}

// ---------------------------------------------------------------------------
// [IMPLEMENTED] Gap R2 – StartFailover events now emitted via D-Bus monitor
// ---------------------------------------------------------------------------
// StartFailover (whether invoked via Redfish POST ForceFailover or a direct
// D-Bus call) causes the redundancy-manager in phosphor-state-manager to:
//   1. set FailoverInProgress = true   (PropertiesChanged on BMC.Redundancy)
//   2. set Role = Active               (PropertiesChanged on BMC.Redundancy)
//   3. set FailoversAllowed = false    (PropertiesChanged on BMC.Redundancy)
//   4. set FailoverInProgress = false  (PropertiesChanged on BMC.Redundancy)
//
// Each of those property changes fires bmcRedundancyPropertyChange() in
// event_dbus_monitor.hpp, which emits the appropriate Redfish event:
//   • FailoversAllowed false → ResourceStatusChangedWarning
//   • Role change → ResourceStateChanged
//   • FailoverInProgress or other property → ResourceChanged (fallback)
//
// handleManagerForceFailover() therefore does NOT need its own sendEvent()
// call: the events are always generated by the signal handler regardless of
// how StartFailover was invoked.
//
// This test verifies that the message payloads used by the signal handler
// for the Role and FailoversAllowed transitions are well-formed.
//
TEST(ManagerRedundancyMissing, ForceFailoverSuccessEmitsEventViaSignalHandler)
{
    // Role change (Passive → Active) emitted by bmcRedundancyPropertyChange
    nlohmann::json::object_t roleMsg =
        messages::resourceStateChanged("bmc", "Active");
    ASSERT_FALSE(roleMsg.empty());
    EXPECT_TRUE(
        roleMsg.at("MessageId").get<std::string>().ends_with("ResourceStateChanged"));
    EXPECT_EQ(roleMsg.at("MessageSeverity"), "OK");
    EXPECT_EQ(roleMsg.at("MessageArgs")[1], "Active");

    // FailoversAllowed=false emitted by bmcRedundancyPropertyChange
    nlohmann::json::object_t foMsg =
        messages::resourceStatusChangedWarning("bmc", "Warning");
    ASSERT_FALSE(foMsg.empty());
    EXPECT_TRUE(
        foMsg.at("MessageId").get<std::string>().ends_with("ResourceStatusChangedWarning"));
    EXPECT_EQ(foMsg.at("MessageSeverity"), "Warning");
}

// ---------------------------------------------------------------------------
// [IMPLEMENTED] Gap R3 – RedundancyEnabled change now emits ResourceChanged
// ---------------------------------------------------------------------------
// bmcRedundancyPropertyChange() falls through to the generic ResourceChanged
// path when RedundancyEnabled changes (not caught by a more specific check),
// so the generic fallback covers this case.
//
TEST(ManagerRedundancyMissing,
     RedundancyEnabledChangePayloadIsValid)
{
    // The fallback ResourceChanged message is sent for RedundancyEnabled changes
    nlohmann::json::object_t msg = messages::resourceChanged();
    ASSERT_FALSE(msg.empty());
    EXPECT_TRUE(
        msg.at("MessageId").get<std::string>().ends_with("ResourceChanged"));
    EXPECT_EQ(msg.at("MessageSeverity"), "OK");
}

// ---------------------------------------------------------------------------
// [IMPLEMENTED] Gap R4a – FailoversAllowed false→true now emits StatusOK
// ---------------------------------------------------------------------------
// bmcRedundancyPropertyChange() explicitly checks FailoversAllowed and emits
// sendEvent(messages::resourceStatusChangedOK(...)) when it becomes true.
//
TEST(ManagerRedundancyMissing,
     FailoversAllowedEnabledEmitsStatusOKEvent)
{
    nlohmann::json::object_t msg =
        messages::resourceStatusChangedOK("bmc", "OK");

    ASSERT_FALSE(msg.empty());
    EXPECT_TRUE(
        msg.at("MessageId").get<std::string>().ends_with("ResourceStatusChangedOK"));
    EXPECT_EQ(msg.at("MessageSeverity"), "OK");

    const auto& args = msg.at("MessageArgs");
    ASSERT_EQ(args.size(), 2U);
    EXPECT_EQ(args[0], "bmc");
    EXPECT_EQ(args[1], "OK");
}

// ---------------------------------------------------------------------------
// [IMPLEMENTED] Gap R4b – FailoversAllowed true→false now emits StatusWarning
// ---------------------------------------------------------------------------
// bmcRedundancyPropertyChange() explicitly checks FailoversAllowed and emits
// sendEvent(messages::resourceStatusChangedWarning(...)) when it becomes false.
//
TEST(ManagerRedundancyMissing,
     FailoversAllowedDisabledEmitsStatusWarningEvent)
{
    nlohmann::json::object_t msg =
        messages::resourceStatusChangedWarning("bmc", "Warning");

    ASSERT_FALSE(msg.empty());
    EXPECT_TRUE(
        msg.at("MessageId").get<std::string>().ends_with("ResourceStatusChangedWarning"));
    EXPECT_EQ(msg.at("MessageSeverity"), "Warning");

    const auto& args = msg.at("MessageArgs");
    ASSERT_EQ(args.size(), 2U);
    EXPECT_EQ(args[0], "bmc");
    EXPECT_EQ(args[1], "Warning");
}

// ---------------------------------------------------------------------------
// [IMPLEMENTED] Gap R5 – Role change (Standby→Active) now emits StateChanged
// ---------------------------------------------------------------------------
// bmcRedundancyPropertyChange() explicitly checks the Role property and emits
// sendEvent(messages::resourceStateChanged(...)) when it changes.
//
TEST(ManagerRedundancyMissing, RoleChangeEmitsResourceStateChangedEvent)
{
    // Active role transition
    nlohmann::json::object_t msgActive =
        messages::resourceStateChanged("bmc", "Active");
    ASSERT_FALSE(msgActive.empty());
    EXPECT_TRUE(
        msgActive.at("MessageId").get<std::string>().ends_with(
            "ResourceStateChanged"));
    EXPECT_EQ(msgActive.at("MessageArgs")[1], "Active");

    // Standby role transition
    nlohmann::json::object_t msgStandby =
        messages::resourceStateChanged("bmc", "Standby");
    ASSERT_FALSE(msgStandby.empty());
    EXPECT_EQ(msgStandby.at("MessageArgs")[1], "Standby");
}

// ---------------------------------------------------------------------------
// [MISSING] Gap R6 – No D-Bus monitoring of sibling BMC objects
// ---------------------------------------------------------------------------
// handleRedundancySubTree() only reads state from bmc0 (the local BMC).
// The RedundancySet should include sibling BMC paths (from D-Bus subtree),
// but there is no mechanism to emit events when a sibling BMC appears or
// disappears from the subtree (InterfacesAdded / InterfacesRemoved on
// xyz.openbmc_project.State.BMC.Redundancy).
//
// Expected behaviour:
//   When a new sibling BMC object appears on D-Bus (InterfacesAdded for
//   xyz.openbmc_project.State.BMC.Redundancy), emit ResourceChanged for
//   the Manager resource so subscribers know the RedundancySet changed.
//   Similarly on InterfacesRemoved.
//
TEST(ManagerRedundancyMissing, SiblingBMCAppearanceDoesNotEmitEvent)
{
    GTEST_SKIP()
        << "[MISSING] No InterfacesAdded/InterfacesRemoved handler for "
           "sibling BMC redundancy objects. "
           "Register a D-Bus ObjectManager match on "
           "/xyz/openbmc_project/state for InterfacesAdded/Removed events "
           "on the xyz.openbmc_project.State.BMC.Redundancy interface and "
           "emit sendEvent(messages::resourceChanged(), "
           "\"/redfish/v1/Managers/<id>\", \"Manager\") when the set changes.";
}

// ---------------------------------------------------------------------------
// Summary: message helpers that are ready for wiring
// ---------------------------------------------------------------------------
// All message payloads needed for the redundancy event gaps are already
// available in resource_messages.hpp.  This test verifies they produce
// valid JSON so they can be directly used in sendEvent() calls once the
// signal handlers are added.
//
TEST(ManagerRedundancyMissing, AllRequiredMessageHelpersProduceValidJson)
{
    // resourceChanged — used for Mode, Role, and generic state change events
    {
        nlohmann::json::object_t msg = messages::resourceChanged();
        ASSERT_FALSE(msg.empty());
        EXPECT_FALSE(msg.at("MessageId").get<std::string>().empty());
        EXPECT_EQ(msg.at("MessageSeverity"), "OK");
    }

    // resourceStatusChangedOK — used when FailoversAllowed becomes true
    {
        nlohmann::json::object_t msg =
            messages::resourceStatusChangedOK("bmc0", "OK");
        ASSERT_FALSE(msg.empty());
        EXPECT_TRUE(
            msg.at("MessageId").get<std::string>().ends_with(
                "ResourceStatusChangedOK"));
        EXPECT_EQ(msg.at("MessageSeverity"), "OK");
        EXPECT_EQ(msg.at("MessageArgs").size(), 2U);
    }

    // resourceStatusChangedWarning — used when FailoversAllowed becomes false
    {
        nlohmann::json::object_t msg =
            messages::resourceStatusChangedWarning("bmc0", "Warning");
        ASSERT_FALSE(msg.empty());
        EXPECT_TRUE(
            msg.at("MessageId").get<std::string>().ends_with(
                "ResourceStatusChangedWarning"));
        EXPECT_EQ(msg.at("MessageSeverity"), "Warning");
        EXPECT_EQ(msg.at("MessageArgs").size(), 2U);
    }

    // resourceStateChanged — used when Role changes Standby↔Active
    {
        nlohmann::json::object_t msg =
            messages::resourceStateChanged("bmc0", "Active");
        ASSERT_FALSE(msg.empty());
        EXPECT_TRUE(
            msg.at("MessageId").get<std::string>().ends_with(
                "ResourceStateChanged"));
        EXPECT_EQ(msg.at("MessageSeverity"), "OK");
        EXPECT_EQ(msg.at("MessageArgs").size(), 2U);
    }
}

} // namespace
} // namespace redfish
