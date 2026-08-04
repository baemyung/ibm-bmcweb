// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright OpenBMC Authors
//
// Tests for satellite event forwarding infrastructure.
//
// Coverage status: PARTIALLY COVERED
//
// What IS tested here:
//   - URI prefix-fixing applied to event-like JSON payloads
//     (addPrefixes / addPrefixToItem for OriginOfCondition and @odata.id)
//   - AggregationSourceDiscovered message format & content
//   - ResourceCreated / ResourceRemoved message format (used when events are
//     forwarded or generated for aggregation lifecycle)
//   - Satellite response processing preserves non-JSON bodies (e.g. SSE
//     text/event-stream) — verifying the passthrough path used for event
//     forwarding
//
// What is NOT yet tested (known gaps):
//   - Actual injection of satellite events into the EventServiceManager
//     subscriber pipeline (no code path exists for this yet)
//   - End-to-end forwarding of a satellite SSE stream to local subscribers
//   - Filter matching applied to forwarded satellite events

#include "async_resp.hpp"
#include "http_response.hpp"
#include "redfish_aggregator.hpp"
#include "resource_messages.hpp"

#include <nlohmann/json.hpp>

#include <memory>
#include <string>

#include <gtest/gtest.h>

namespace redfish
{
namespace
{

// ---------------------------------------------------------------------------
// URI prefix-fixing for event-like payloads
// ---------------------------------------------------------------------------

// OriginOfCondition is excluded from nonUriProperties because in regular
// Redfish responses it is an object (not a URI string).  However, inside an
// Events array it IS a plain string URI representing a satellite resource.
// addPrefixes() now detects the string case and applies the same prefix-fixing
// as any other URI property, so the satellite resource is correctly identified
// on the aggregating BMC.
TEST(SatelliteEventForwarding, OriginOfConditionStringInEventPayloadGetsPrefixed)
{
    nlohmann::json eventPayload = nlohmann::json::parse(R"(
    {
      "@odata.type": "#Event.v1_4_0.Event",
      "Id": "42",
      "Name": "Event Log",
      "Events": [
        {
          "MemberId": "0",
          "MessageId": "ResourceEvent.1.3.ResourceCreated",
          "OriginOfCondition": "/redfish/v1/Chassis/SatChassis"
        }
      ]
    }
    )",
                                                        nullptr, false);

    addPrefixes(eventPayload, "sat1");

    // OriginOfCondition is a string URI here, so it must be prefix-fixed.
    const std::string& origin =
        eventPayload["Events"][0]["OriginOfCondition"].get_ref<
            const std::string&>();
    EXPECT_EQ(origin, "/redfish/v1/Chassis/sat1_SatChassis");
}

// @odata.id inside an event payload DOES get prefix-fixed, because it is in
// the nonUriProperties allow-list.
TEST(SatelliteEventForwarding, OdataIdInEventPayloadGetsPrefixed)
{
    nlohmann::json eventPayload = nlohmann::json::parse(R"(
    {
      "@odata.id": "/redfish/v1/Systems/system",
      "@odata.type": "#Event.v1_4_0.Event"
    }
    )",
                                                        nullptr, false);

    addPrefixes(eventPayload, "sat1");

    EXPECT_EQ(eventPayload["@odata.id"], "/redfish/v1/Systems/sat1_system");
}

// Multiple entries in an Events array: each should be processed independently.
TEST(SatelliteEventForwarding, MultipleEventMembersAllPrefixed)
{
    nlohmann::json eventPayload = nlohmann::json::parse(R"(
    {
      "Events": [
        {
          "@odata.id": "/redfish/v1/Chassis/chassisA"
        },
        {
          "@odata.id": "/redfish/v1/Chassis/chassisB"
        }
      ]
    }
    )",
                                                        nullptr, false);

    addPrefixes(eventPayload, "prefix");

    EXPECT_EQ(eventPayload["Events"][0]["@odata.id"],
              "/redfish/v1/Chassis/prefix_chassisA");
    EXPECT_EQ(eventPayload["Events"][1]["@odata.id"],
              "/redfish/v1/Chassis/prefix_chassisB");
}

// URIs that do not belong to an aggregated top-level collection must not be
// modified.  JsonSchemas URIs must always be skipped.
TEST(SatelliteEventForwarding, JsonSchemasUriNotPrefixed)
{
    nlohmann::json payload;
    payload["@odata.id"] =
        "/redfish/v1/JsonSchemas/Event/Event.json";

    addPrefixes(payload, "sat1");

    EXPECT_EQ(payload["@odata.id"],
              "/redfish/v1/JsonSchemas/Event/Event.json");
}

// ---------------------------------------------------------------------------
// Non-JSON (SSE / text/event-stream) passthrough via processResponse
// ---------------------------------------------------------------------------

// When the satellite returns a text/event-stream body (e.g. for SSE), the
// aggregator must forward it unchanged — no JSON parsing / prefix-fixing
// should be attempted.
TEST(SatelliteEventForwarding, NonJsonEventStreamBodyPassthrough)
{
    const std::string sseBody =
        "id: 1\ndata: {\"@odata.type\":\"#Event.v1_4_0.Event\"}\n\n";

    crow::Response resp;
    resp.addHeader("Content-Type", "text/event-stream");
    resp.write(sseBody);
    resp.result(200);

    auto asyncResp = std::make_shared<bmcweb::AsyncResp>();
    RedfishAggregator::processResponse("sat1", asyncResp, resp);

    EXPECT_EQ(asyncResp->res.resultInt(), 200);
    EXPECT_EQ(asyncResp->res.getHeaderValue("Content-Type"),
              "text/event-stream");
    ASSERT_TRUE(asyncResp->res.body());
    EXPECT_EQ(*asyncResp->res.body(), sseBody);
}

// ---------------------------------------------------------------------------
// AggregationSourceDiscovered message format
// ---------------------------------------------------------------------------

// The AggregationSourceDiscovered message is the primary mechanism by which
// the aggregator announces newly-found satellite BMCs to event subscribers.
// Verify the message is well-formed so we can trust it in further tests.
TEST(SatelliteEventForwarding, AggregationSourceDiscoveredMessageFormat)
{
    nlohmann::json::object_t msg =
        messages::aggregationSourceDiscovered("Redfish", "https://sat1.bmc");

    ASSERT_FALSE(msg.empty());

    // MessageId must follow the pattern
    // ResourceEvent.<major>.<minor>.AggregationSourceDiscovered
    const auto& msgId = msg.at("MessageId").get_ref<const std::string&>();
    EXPECT_TRUE(msgId.starts_with("ResourceEvent."));
    EXPECT_TRUE(msgId.ends_with("AggregationSourceDiscovered"));

    // Arguments must be echoed back correctly
    const auto& args = msg.at("MessageArgs");
    ASSERT_EQ(args.size(), 2U);
    EXPECT_EQ(args[0], "Redfish");
    EXPECT_EQ(args[1], "https://sat1.bmc");

    // Message body must be filled in (not empty after arg substitution)
    const auto& message = msg.at("Message").get_ref<const std::string&>();
    EXPECT_FALSE(message.empty());
    EXPECT_NE(message.find("Redfish"), std::string::npos);
    EXPECT_NE(message.find("https://sat1.bmc"), std::string::npos);

    // Severity must be OK for a discovery event
    EXPECT_EQ(msg.at("MessageSeverity"), "OK");
}

// A second call with different arguments must produce a fresh, independent
// message (no shared state between calls).
TEST(SatelliteEventForwarding, AggregationSourceDiscoveredIndependentCalls)
{
    nlohmann::json::object_t msg1 =
        messages::aggregationSourceDiscovered("Redfish", "https://sat1.bmc");
    nlohmann::json::object_t msg2 =
        messages::aggregationSourceDiscovered("SNMP", "https://sat2.bmc");

    EXPECT_NE(msg1.at("Message"), msg2.at("Message"));
    EXPECT_EQ(msg1.at("MessageArgs")[0], "Redfish");
    EXPECT_EQ(msg2.at("MessageArgs")[0], "SNMP");
}

// ---------------------------------------------------------------------------
// ResourceCreated / ResourceRemoved messages used in aggregation lifecycle
// ---------------------------------------------------------------------------

TEST(SatelliteEventForwarding, ResourceCreatedMessageFormat)
{
    nlohmann::json::object_t msg = messages::resourceCreated();

    ASSERT_FALSE(msg.empty());

    const auto& msgId = msg.at("MessageId").get_ref<const std::string&>();
    EXPECT_TRUE(msgId.starts_with("ResourceEvent."));
    EXPECT_TRUE(msgId.ends_with("ResourceCreated"));

    EXPECT_EQ(msg.at("MessageSeverity"), "OK");

    // Zero-arg message; MessageArgs array must exist but be empty
    EXPECT_TRUE(msg.at("MessageArgs").empty());
}

TEST(SatelliteEventForwarding, ResourceRemovedMessageFormat)
{
    nlohmann::json::object_t msg = messages::resourceRemoved();

    ASSERT_FALSE(msg.empty());

    const auto& msgId = msg.at("MessageId").get_ref<const std::string&>();
    EXPECT_TRUE(msgId.starts_with("ResourceEvent."));
    EXPECT_TRUE(msgId.ends_with("ResourceRemoved"));

    EXPECT_EQ(msg.at("MessageSeverity"), "OK");
    EXPECT_TRUE(msg.at("MessageArgs").empty());
}

// ---------------------------------------------------------------------------
// Satellite health-change messages (ResourceStatusChanged*)
// ---------------------------------------------------------------------------

TEST(SatelliteEventForwarding, ResourceStatusChangedOKMessageFormat)
{
    nlohmann::json::object_t msg =
        messages::resourceStatusChangedOK("SatBMC", "OK");

    ASSERT_FALSE(msg.empty());

    const auto& msgId = msg.at("MessageId").get_ref<const std::string&>();
    EXPECT_TRUE(msgId.ends_with("ResourceStatusChangedOK"));

    EXPECT_EQ(msg.at("MessageSeverity"), "OK");
    EXPECT_EQ(msg.at("MessageArgs")[0], "SatBMC");
    EXPECT_EQ(msg.at("MessageArgs")[1], "OK");
}

TEST(SatelliteEventForwarding, ResourceStatusChangedWarningMessageFormat)
{
    nlohmann::json::object_t msg =
        messages::resourceStatusChangedWarning("SatBMC", "Warning");

    ASSERT_FALSE(msg.empty());

    const auto& msgId = msg.at("MessageId").get_ref<const std::string&>();
    EXPECT_TRUE(msgId.ends_with("ResourceStatusChangedWarning"));

    EXPECT_EQ(msg.at("MessageSeverity"), "Warning");
}

TEST(SatelliteEventForwarding, ResourceStatusChangedCriticalMessageFormat)
{
    nlohmann::json::object_t msg =
        messages::resourceStatusChangedCritical("SatBMC", "Critical");

    ASSERT_FALSE(msg.empty());

    const auto& msgId = msg.at("MessageId").get_ref<const std::string&>();
    EXPECT_TRUE(msgId.ends_with("ResourceStatusChangedCritical"));

    EXPECT_EQ(msg.at("MessageSeverity"), "Critical");
}

// ---------------------------------------------------------------------------
// processResponse: 429 / 502 short-circuit — events are not processed
// ---------------------------------------------------------------------------

// When a satellite returns 429 (Too Many Requests) or 502 (Bad Gateway) the
// aggregator must propagate the status code without attempting to parse or
// prefix-fix any event body.
TEST(SatelliteEventForwarding, TooManyRequestsNotProcessed)
{
    crow::Response resp;
    resp.addHeader("Content-Type", "application/json");
    resp.write(R"({"@odata.id":"/redfish/v1/Chassis/sat"})");
    resp.result(boost::beast::http::status::too_many_requests);

    auto asyncResp = std::make_shared<bmcweb::AsyncResp>();
    RedfishAggregator::processResponse("sat1", asyncResp, resp);

    EXPECT_EQ(asyncResp->res.resultInt(), 429);
    // JSON body must not have been parsed — jsonValue should be empty
    EXPECT_TRUE(asyncResp->res.jsonValue.empty());
}

TEST(SatelliteEventForwarding, BadGatewayNotProcessed)
{
    crow::Response resp;
    resp.addHeader("Content-Type", "application/json");
    resp.write(R"({"@odata.id":"/redfish/v1/Chassis/sat"})");
    resp.result(boost::beast::http::status::bad_gateway);

    auto asyncResp = std::make_shared<bmcweb::AsyncResp>();
    RedfishAggregator::processResponse("sat1", asyncResp, resp);

    EXPECT_EQ(asyncResp->res.resultInt(), 502);
    EXPECT_TRUE(asyncResp->res.jsonValue.empty());
}

} // namespace
} // namespace redfish
