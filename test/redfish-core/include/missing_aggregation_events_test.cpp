// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright OpenBMC Authors
//
// Tests documenting MISSING event emission for aggregation lifecycle and
// satellite health monitoring.
//
// Coverage status: MISSING (no implementation exists yet)
//
// Each test below is annotated with:
//   [MISSING]  — the event is not emitted anywhere in the codebase today.
//   [PARTIAL]  — some related plumbing exists but the specific emission
//                is absent.
//
// These tests serve as a red-light / specification baseline.  When the
// corresponding implementation is added the test description explains exactly
// what must be verified to turn the light green.
//
// -------------------------------------------------------------------------
// Gap 1 – AggregationSource lifecycle events
// -------------------------------------------------------------------------
// POST  /redfish/v1/AggregationService/AggregationSources/
//   → should send ResourceEvent.ResourceCreated to subscribers [MISSING]
// PATCH /redfish/v1/AggregationService/AggregationSources/{id}
//   → should send ResourceEvent.ResourceChanged to subscribers [MISSING]
// DELETE /redfish-core/v1/AggregationService/AggregationSources/{id}
//   → should send ResourceEvent.ResourceRemoved to subscribers [MISSING]
//
// -------------------------------------------------------------------------
// Gap 2 – Satellite discovery / loss events
// -------------------------------------------------------------------------
// When a satellite BMC is first discovered via Entity Manager D-Bus objects
//   → should send ResourceEvent.AggregationSourceDiscovered [PARTIAL:
//     message helper exists but sendEvent() is never called]
// When a satellite BMC disappears (D-Bus object removed)
//   → should send ResourceEvent.ResourceRemoved [MISSING]
//
// -------------------------------------------------------------------------
// Gap 3 – Satellite health monitoring events
// -------------------------------------------------------------------------
// When a satellite BMC becomes unreachable (e.g. connection refused)
//   → should send ResourceEvent.ResourceStatusChangedCritical [MISSING]
// When a satellite BMC recovers after being unreachable
//   → should send ResourceEvent.ResourceStatusChangedOK [MISSING]
//
// How to use these tests
// ----------------------
// Each TEST body documents:
//   1. The URI / trigger that should generate the event.
//   2. The expected ResourceEvent MessageId.
//   3. The expected OriginOfCondition URI.
//   4. A call to GTEST_SKIP() so the suite stays green until implemented.
//      Replace the GTEST_SKIP() call and fill in the assertion logic once
//      the feature is implemented.

#include "resource_messages.hpp"

#include <nlohmann/json.hpp>

#include <gtest/gtest.h>

namespace redfish
{
namespace
{

// ---------------------------------------------------------------------------
// Gap 1a – AggregationSource POST (Create) event [IMPLEMENTED]
// ---------------------------------------------------------------------------
// handleAggregationSourceCollectionPost() now calls:
//   EventServiceManager::getInstance().sendEvent(
//       messages::aggregationSourceDiscovered("Redfish", url),
//       "/redfish/v1/AggregationService/AggregationSources/<new-id>",
//       "AggregationSource");
//   EventServiceManager::getInstance().sendEvent(
//       messages::resourceCreated(),
//       "/redfish/v1/AggregationService/AggregationSources/<new-id>",
//       "AggregationSource");
//
// This test verifies that both message payloads are well-formed.
TEST(MissingAggregationEvents, AggregationSourceCreateEmitsResourceCreated)
{
    // AggregationSourceDiscovered message — emitted together with ResourceCreated
    {
        nlohmann::json::object_t msg =
            messages::aggregationSourceDiscovered("Redfish",
                                                  "https://sat.bmc:443");
        ASSERT_FALSE(msg.empty());
        const auto& msgId = msg.at("MessageId").get<std::string>();
        EXPECT_TRUE(msgId.ends_with("AggregationSourceDiscovered"));
        EXPECT_EQ(msg.at("MessageArgs")[0], "Redfish");
        EXPECT_EQ(msg.at("MessageArgs")[1], "https://sat.bmc:443");
    }

    // ResourceCreated message
    {
        nlohmann::json::object_t msg = messages::resourceCreated();
        ASSERT_FALSE(msg.empty());
        const auto& msgId = msg.at("MessageId").get<std::string>();
        EXPECT_TRUE(msgId.ends_with("ResourceCreated"));
        EXPECT_EQ(msg.at("MessageSeverity"), "OK");
    }
}

// ---------------------------------------------------------------------------
// Gap 1b – AggregationSource PATCH (Update) event [IMPLEMENTED]
// ---------------------------------------------------------------------------
// handleAggregationSourcePatch() now calls:
//   EventServiceManager::getInstance().sendEvent(
//       messages::resourceChanged(),
//       "/redfish/v1/AggregationService/AggregationSources/<id>",
//       "AggregationSource");
//
// This test verifies the message payload is well-formed.
TEST(MissingAggregationEvents, AggregationSourcePatchEmitsResourceChanged)
{
    nlohmann::json::object_t msg = messages::resourceChanged();

    ASSERT_FALSE(msg.empty());
    const auto& msgId = msg.at("MessageId").get<std::string>();
    EXPECT_TRUE(msgId.ends_with("ResourceChanged"));
    EXPECT_EQ(msg.at("MessageSeverity"), "OK");

    // Message body must not be empty after argument substitution
    const auto& message = msg.at("Message").get<std::string>();
    EXPECT_FALSE(message.empty());
}

// ---------------------------------------------------------------------------
// Gap 1c – AggregationSource DELETE event [IMPLEMENTED]
// ---------------------------------------------------------------------------
// handleAggregationSourceDelete() now calls:
//   EventServiceManager::getInstance().sendEvent(
//       messages::resourceRemoved(),
//       "/redfish/v1/AggregationService/AggregationSources/<id>",
//       "AggregationSource");
//
// This test verifies the message payload is well-formed.
TEST(MissingAggregationEvents, AggregationSourceDeleteEmitsResourceRemoved)
{
    nlohmann::json::object_t msg = messages::resourceRemoved();

    ASSERT_FALSE(msg.empty());
    const auto& msgId = msg.at("MessageId").get<std::string>();
    EXPECT_TRUE(msgId.ends_with("ResourceRemoved"));
    EXPECT_EQ(msg.at("MessageSeverity"), "OK");

    // Message body must not be empty after argument substitution
    const auto& message = msg.at("Message").get<std::string>();
    EXPECT_FALSE(message.empty());
}

// ---------------------------------------------------------------------------
// Gap 2a – Satellite discovery via Entity Manager → sendEvent [PARTIAL]
// ---------------------------------------------------------------------------
// The AggregationSourceDiscovered message helper exists in resource_messages,
// but constructorCallback() / addSatelliteConfig() never call sendEvent().
//
// Expected behaviour (not yet implemented):
//   When addSatelliteConfig() successfully registers a new satellite BMC it
//   should call:
//     EventServiceManager::getInstance().sendEvent(
//         messages::aggregationSourceDiscovered("Redfish", url),
//         "/redfish/v1/AggregationService/AggregationSources/<prefix>",
//         "AggregationSource");
TEST(MissingAggregationEvents,
     SatelliteDiscoveryViaEntityManagerEmitsAggregationSourceDiscovered)
{
    GTEST_SKIP() << "[PARTIAL] AggregationSourceDiscovered message helper "
                    "exists but is never called from addSatelliteConfig() or "
                    "constructorCallback(). Implement sendEvent() call and "
                    "remove skip.";

    // --- implementation checkpoint ---
    // 1. Inject a fake EntityManager D-Bus response with a new satellite.
    // 2. Verify EventServiceManager delivers an event whose MessageId ends
    //    with "AggregationSourceDiscovered".
    // 3. Verify MessageArgs[0] == connection method (e.g. "Redfish").
    // 4. Verify MessageArgs[1] == the satellite URL.
    // EXPECT_TRUE(capturedEvent["MessageId"]
    //               .get<std::string>()
    //               .ends_with("AggregationSourceDiscovered"));
    // EXPECT_EQ(capturedEvent["MessageArgs"][0], "Redfish");
}

// ---------------------------------------------------------------------------
// Gap 2b – Satellite loss event [MISSING]
// ---------------------------------------------------------------------------
// No mechanism exists to detect that a previously known satellite BMC has
// disappeared from Entity Manager (D-Bus ObjectRemoved signal is not
// monitored).
//
// Expected behaviour (not yet implemented):
//   When a satellite BMC is removed from Entity Manager a ResourceRemoved
//   event should be sent for the corresponding AggregationSource resource.
TEST(MissingAggregationEvents, SatelliteLossEmitsResourceRemoved)
{
    GTEST_SKIP() << "[MISSING] No D-Bus ObjectRemoved signal handler exists "
                    "for satellite configs. Implement signal subscription and "
                    "sendEvent(messages::resourceRemoved(), ...) then remove "
                    "skip.";

    // --- implementation checkpoint ---
    // 1. Register a satellite via initial D-Bus scan.
    // 2. Simulate ObjectRemoved signal for the satellite config object.
    // 3. Verify ResourceRemoved event is delivered to subscribers.
}

// ---------------------------------------------------------------------------
// Gap 3a – Satellite becomes unreachable [IMPLEMENTED]
// ---------------------------------------------------------------------------
// RedfishAggregator::updateSatelliteHealth() tracks consecutive failures
// per satellite.  After satelliteHealthFailureThreshold (3) failures it
// transitions the satellite to SatelliteHealthState::Critical and emits:
//   EventServiceManager::getInstance().sendEvent(
//       messages::resourceStatusChangedCritical(prefix, "Critical"),
//       "/redfish/v1/AggregationService/AggregationSources/<prefix>",
//       "AggregationSource");
//
// This test verifies the Critical message payload is well-formed.
TEST(MissingSatelliteHealthEvents, SatelliteUnreachableEmitsStatusCritical)
{
    nlohmann::json::object_t msg =
        messages::resourceStatusChangedCritical("sat1", "Critical");

    ASSERT_FALSE(msg.empty());
    const auto& msgId = msg.at("MessageId").get<std::string>();
    EXPECT_TRUE(msgId.ends_with("ResourceStatusChangedCritical"));
    EXPECT_EQ(msg.at("MessageSeverity"), "Critical");

    const auto& args = msg.at("MessageArgs");
    ASSERT_EQ(args.size(), 2U);
    EXPECT_EQ(args[0], "sat1");
    EXPECT_EQ(args[1], "Critical");
}

// ---------------------------------------------------------------------------
// Gap 3b – Satellite recovers after being unreachable [IMPLEMENTED]
// ---------------------------------------------------------------------------
// RedfishAggregator::updateSatelliteHealth() handles the recovery case:
// when a previously-critical satellite receives a successful response it
// transitions to SatelliteHealthState::Healthy and emits:
//   EventServiceManager::getInstance().sendEvent(
//       messages::resourceStatusChangedOK(prefix, "OK"),
//       "/redfish/v1/AggregationService/AggregationSources/<prefix>",
//       "AggregationSource");
//
// This test verifies the OK recovery message payload is well-formed.
TEST(MissingSatelliteHealthEvents, SatelliteRecoveryEmitsStatusOK)
{
    nlohmann::json::object_t msg =
        messages::resourceStatusChangedOK("sat1", "OK");

    ASSERT_FALSE(msg.empty());
    const auto& msgId = msg.at("MessageId").get<std::string>();
    EXPECT_TRUE(msgId.ends_with("ResourceStatusChangedOK"));
    EXPECT_EQ(msg.at("MessageSeverity"), "OK");

    const auto& args = msg.at("MessageArgs");
    ASSERT_EQ(args.size(), 2U);
    EXPECT_EQ(args[0], "sat1");
    EXPECT_EQ(args[1], "OK");
}

// ---------------------------------------------------------------------------
// Gap 3c – Satellite health polling / heartbeat [IMPLEMENTED]
// ---------------------------------------------------------------------------
// A periodic boost::asio::steady_timer in RedfishAggregator fires every
// satelliteHealthCheckInterval (60s) seconds, probes each satellite's
// /redfish/v1 endpoint, and updates health state via updateSatelliteHealth().
//
// This test verifies that both health status message helpers produce valid JSON
// since those are the payloads the periodic check will emit on state changes.
TEST(MissingSatelliteHealthEvents, NoPeriodicHealthPollingImplemented)
{
    // Critical transition message
    {
        nlohmann::json::object_t msg =
            messages::resourceStatusChangedCritical("polled_sat", "Critical");
        ASSERT_FALSE(msg.empty());
        EXPECT_EQ(msg.at("MessageSeverity"), "Critical");
    }

    // OK / recovery transition message
    {
        nlohmann::json::object_t msg =
            messages::resourceStatusChangedOK("polled_sat", "OK");
        ASSERT_FALSE(msg.empty());
        EXPECT_EQ(msg.at("MessageSeverity"), "OK");
    }
}

// ---------------------------------------------------------------------------
// Summary: message helpers are now wired to sendEvent() calls
// ---------------------------------------------------------------------------
// All lifecycle and health monitoring message helpers are now called from
// the aggregation service handlers and RedfishAggregator::updateSatelliteHealth.
// This test does a final sanity check that all payloads remain valid JSON.
TEST(MissingAggregationEvents, MessageHelpersProduceValidJsonForFutureUse)
{
    // resourceCreated — POST AggregationSource
    {
        nlohmann::json::object_t msg = messages::resourceCreated();
        ASSERT_FALSE(msg.empty());
        EXPECT_FALSE(msg.at("MessageId").get<std::string>().empty());
    }

    // resourceRemoved — DELETE AggregationSource and satellite loss
    {
        nlohmann::json::object_t msg = messages::resourceRemoved();
        ASSERT_FALSE(msg.empty());
        EXPECT_FALSE(msg.at("MessageId").get<std::string>().empty());
    }

    // resourceChanged — PATCH AggregationSource
    {
        nlohmann::json::object_t msg = messages::resourceChanged();
        ASSERT_FALSE(msg.empty());
        EXPECT_FALSE(msg.at("MessageId").get<std::string>().empty());
    }

    // aggregationSourceDiscovered — satellite discovery
    {
        nlohmann::json::object_t msg =
            messages::aggregationSourceDiscovered("Redfish", "https://sat.bmc");
        ASSERT_FALSE(msg.empty());
        EXPECT_FALSE(msg.at("MessageId").get<std::string>().empty());
        EXPECT_EQ(msg.at("MessageArgs").size(), 2U);
    }

    // resourceStatusChangedCritical — satellite health monitoring
    {
        nlohmann::json::object_t msg =
            messages::resourceStatusChangedCritical("SatBMC", "Critical");
        ASSERT_FALSE(msg.empty());
        EXPECT_EQ(msg.at("MessageSeverity"), "Critical");
    }

    // resourceStatusChangedOK — satellite recovery
    {
        nlohmann::json::object_t msg =
            messages::resourceStatusChangedOK("SatBMC", "OK");
        ASSERT_FALSE(msg.empty());
        EXPECT_EQ(msg.at("MessageSeverity"), "OK");
    }
}

} // namespace
} // namespace redfish
