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
// Gap 1a – AggregationSource POST (Create) event [MISSING]
// ---------------------------------------------------------------------------
// Expected behaviour (not yet implemented):
//   When handleAggregationSourceCollectionPost() successfully creates a new
//   AggregationSource it should call:
//     EventServiceManager::getInstance().sendEvent(
//         messages::resourceCreated(),
//         "/redfish/v1/AggregationService/AggregationSources/<new-id>",
//         "AggregationSource");
TEST(MissingAggregationEvents, AggregationSourceCreateEmitsResourceCreated)
{
    GTEST_SKIP() << "[MISSING] AggregationSource POST does not emit "
                    "ResourceEvent.ResourceCreated. "
                    "Implement sendEvent() call in "
                    "handleAggregationSourceCollectionPost() and remove skip.";

    // --- implementation checkpoint (fill in once feature is added) ---
    // 1. POST to /redfish/v1/AggregationService/AggregationSources/ with a
    //    valid HostName payload.
    // 2. Capture the event delivered to a test subscriber.
    // 3. Verify:
    //      event["MessageId"] ends with "ResourceCreated"
    //      event["OriginOfCondition"] == new AggregationSource URI
    // EXPECT_EQ(capturedEvent["MessageId"], expectedMsgId);
    // EXPECT_TRUE(capturedEvent["OriginOfCondition"]
    //               .get<std::string>()
    //               .starts_with(
    //                 "/redfish/v1/AggregationService/AggregationSources/"));
}

// ---------------------------------------------------------------------------
// Gap 1b – AggregationSource PATCH (Update) event [MISSING]
// ---------------------------------------------------------------------------
// Expected behaviour (not yet implemented):
//   When handleAggregationSourcePatch() successfully updates credentials it
//   should call:
//     EventServiceManager::getInstance().sendEvent(
//         messages::resourceChanged(),
//         "/redfish/v1/AggregationService/AggregationSources/<id>",
//         "AggregationSource");
TEST(MissingAggregationEvents, AggregationSourcePatchEmitsResourceChanged)
{
    GTEST_SKIP() << "[MISSING] AggregationSource PATCH does not emit "
                    "ResourceEvent.ResourceChanged. "
                    "Implement sendEvent() call in "
                    "handleAggregationSourcePatch() and remove skip.";

    // --- implementation checkpoint ---
    // 1. Pre-populate an AggregationSource entry.
    // 2. PATCH UserName/Password.
    // 3. Verify event with MessageId ending in "ResourceChanged" is delivered.
    // EXPECT_TRUE(capturedEvent["MessageId"]
    //               .get<std::string>().ends_with("ResourceChanged"));
}

// ---------------------------------------------------------------------------
// Gap 1c – AggregationSource DELETE event [MISSING]
// ---------------------------------------------------------------------------
// Expected behaviour (not yet implemented):
//   When handleAggregationSourceDelete() removes an AggregationSource it
//   should call:
//     EventServiceManager::getInstance().sendEvent(
//         messages::resourceRemoved(),
//         "/redfish/v1/AggregationService/AggregationSources/<id>",
//         "AggregationSource");
TEST(MissingAggregationEvents, AggregationSourceDeleteEmitsResourceRemoved)
{
    GTEST_SKIP() << "[MISSING] AggregationSource DELETE does not emit "
                    "ResourceEvent.ResourceRemoved. "
                    "Implement sendEvent() call in "
                    "handleAggregationSourceDelete() and remove skip.";

    // --- implementation checkpoint ---
    // 1. Pre-populate an AggregationSource entry.
    // 2. DELETE it.
    // 3. Verify event with MessageId ending in "ResourceRemoved" is delivered,
    //    and OriginOfCondition points to the deleted resource URI.
    // EXPECT_TRUE(capturedEvent["MessageId"]
    //               .get<std::string>().ends_with("ResourceRemoved"));
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
// Gap 3a – Satellite becomes unreachable [MISSING]
// ---------------------------------------------------------------------------
// When HTTP requests to a satellite BMC fail (e.g. connection refused,
// repeated 429/502 responses) no health-change event is currently emitted.
//
// Expected behaviour (not yet implemented):
//   When forwardRequest() / forwardCollectionRequests() receives a persistent
//   failure for a satellite it should transition that satellite to a degraded
//   state and call:
//     EventServiceManager::getInstance().sendEvent(
//         messages::resourceStatusChangedCritical(prefix, "Critical"),
//         "/redfish/v1/AggregationService/AggregationSources/<prefix>",
//         "AggregationSource");
TEST(MissingSatelliteHealthEvents, SatelliteUnreachableEmitsStatusCritical)
{
    GTEST_SKIP() << "[MISSING] No satellite health-state tracking exists. "
                    "Implement per-satellite health state in RedfishAggregator "
                    "and emit ResourceStatusChangedCritical on repeated "
                    "failures, then remove skip.";

    // --- implementation checkpoint ---
    // Preconditions:
    //   - A satellite is registered and considered healthy.
    // Trigger:
    //   - Simulate N consecutive HTTP failures (bad_gateway responses).
    // Verify:
    //   - Event delivered with MessageId ending "ResourceStatusChangedCritical"
    //   - OriginOfCondition == AggregationSource URI for the satellite
    //   - MessageArgs[1] == "Critical"
}

// ---------------------------------------------------------------------------
// Gap 3b – Satellite recovers after being unreachable [MISSING]
// ---------------------------------------------------------------------------
// Complementary to 3a: once a satellite that was marked unreachable responds
// successfully, a recovery event should be emitted.
//
// Expected behaviour (not yet implemented):
//   When forwardRequest() receives a successful response after the satellite
//   was in a degraded state it should emit:
//     EventServiceManager::getInstance().sendEvent(
//         messages::resourceStatusChangedOK(prefix, "OK"),
//         "/redfish/v1/AggregationService/AggregationSources/<prefix>",
//         "AggregationSource");
TEST(MissingSatelliteHealthEvents, SatelliteRecoveryEmitsStatusOK)
{
    GTEST_SKIP() << "[MISSING] No satellite health-state tracking exists. "
                    "Implement recovery transition in RedfishAggregator and "
                    "emit ResourceStatusChangedOK, then remove skip.";

    // --- implementation checkpoint ---
    // Preconditions:
    //   - A satellite is registered and currently in the degraded/critical
    //     state (per Gap 3a).
    // Trigger:
    //   - Simulate a successful HTTP response from the satellite.
    // Verify:
    //   - Event delivered with MessageId ending "ResourceStatusChangedOK"
    //   - OriginOfCondition == AggregationSource URI for the satellite
    //   - MessageArgs[1] == "OK"
}

// ---------------------------------------------------------------------------
// Gap 3c – Satellite health polling / heartbeat [MISSING]
// ---------------------------------------------------------------------------
// There is no periodic health-check mechanism for satellite BMCs.  The
// aggregator only discovers satellites at startup via the D-Bus scan and does
// not re-query periodically, so stale or unreachable satellites are never
// detected proactively.
//
// Expected behaviour (not yet implemented):
//   A periodic timer should poll each registered satellite's health endpoint
//   (/redfish/v1 or a ping equivalent) and emit appropriate status-change
//   events when the satellite's availability changes.
TEST(MissingSatelliteHealthEvents, NoPeriodicHealthPollingImplemented)
{
    GTEST_SKIP() << "[MISSING] No periodic satellite health-polling timer "
                    "exists in RedfishAggregator. Implement a timer-based "
                    "health check and the corresponding status-change events, "
                    "then remove skip.";
}

// ---------------------------------------------------------------------------
// Summary: message helpers that EXIST but are never wired to sendEvent()
// ---------------------------------------------------------------------------
// The following test verifies that the raw message helpers produce valid JSON
// so that once sendEvent() calls are added the payloads will be correct.
TEST(MissingAggregationEvents, MessageHelpersProduceValidJsonForFutureUse)
{
    // resourceCreated — needed for POST AggregationSource
    {
        nlohmann::json::object_t msg = messages::resourceCreated();
        ASSERT_FALSE(msg.empty());
        EXPECT_FALSE(msg.at("MessageId").get<std::string>().empty());
    }

    // resourceRemoved — needed for DELETE AggregationSource and satellite loss
    {
        nlohmann::json::object_t msg = messages::resourceRemoved();
        ASSERT_FALSE(msg.empty());
        EXPECT_FALSE(msg.at("MessageId").get<std::string>().empty());
    }

    // resourceChanged — needed for PATCH AggregationSource
    {
        nlohmann::json::object_t msg = messages::resourceChanged();
        ASSERT_FALSE(msg.empty());
        EXPECT_FALSE(msg.at("MessageId").get<std::string>().empty());
    }

    // aggregationSourceDiscovered — needed for satellite discovery
    {
        nlohmann::json::object_t msg =
            messages::aggregationSourceDiscovered("Redfish", "https://sat.bmc");
        ASSERT_FALSE(msg.empty());
        EXPECT_FALSE(msg.at("MessageId").get<std::string>().empty());
        EXPECT_EQ(msg.at("MessageArgs").size(), 2U);
    }

    // resourceStatusChangedCritical — needed for satellite health monitoring
    {
        nlohmann::json::object_t msg =
            messages::resourceStatusChangedCritical("SatBMC", "Critical");
        ASSERT_FALSE(msg.empty());
        EXPECT_EQ(msg.at("MessageSeverity"), "Critical");
    }

    // resourceStatusChangedOK — needed for satellite recovery
    {
        nlohmann::json::object_t msg =
            messages::resourceStatusChangedOK("SatBMC", "OK");
        ASSERT_FALSE(msg.empty());
        EXPECT_EQ(msg.at("MessageSeverity"), "OK");
    }
}

} // namespace
} // namespace redfish
