#!/bin/bash
# SPDX-License-Identifier: Apache-2.0
# SPDX-FileCopyrightText: Copyright OpenBMC Authors
#
# End-to-End Event Forwarding Test Suite
# 
# This script provides manual test scenarios for verifying satellite event
# forwarding functionality in bmcweb.
#
# Prerequisites:
#   - bmcweb running on https://localhost:18080/redfish/v1
#   - Event subscriber listening on http://localhost:8888/events
#   - Satellite BMC(s) registered as AggregationSource
#
# Usage:
#   ./test-satellite-events-e2e.sh setup
#   ./test-satellite-events-e2e.sh create-agg-source <hostname>
#   ./test-satellite-events-e2e.sh create-subscription
#   ./test-satellite-events-e2e.sh send-test-event
#   ./test-satellite-events-e2e.sh cleanup
#
# For full test suite:
#   ./test-satellite-events-e2e.sh full

set -e

# Configuration
BMCWEB_HOST="${BMCWEB_HOST:-localhost}"
BMCWEB_PORT="${BMCWEB_PORT:-18080}"
BMCWEB_USER="${BMCWEB_USER:-root}"
BMCWEB_PASSWORD="${BMCWEB_PASSWORD:-0penBmc}"
SUBSCRIBER_HOST="${SUBSCRIBER_HOST:-localhost}"
SUBSCRIBER_PORT="${SUBSCRIBER_PORT:-8888}"
CURL_OPTS="-k -X"  # -k: ignore self-signed certs
BASE_URL="https://${BMCWEB_HOST}:${BMCWEB_PORT}/redfish/v1"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'  # No Color

# Helper functions
log() {
    echo -e "${BLUE}[TEST]${NC} $1"
}

success() {
    echo -e "${GREEN}[PASS]${NC} $1"
}

warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

error() {
    echo -e "${RED}[FAIL]${NC} $1"
    exit 1
}

# Check prerequisites
check_prerequisites() {
    log "Checking prerequisites..."
    
    # Check curl
    if ! command -v curl &> /dev/null; then
        error "curl not found. Please install curl."
    fi
    
    # Check jq
    if ! command -v jq &> /dev/null; then
        error "jq not found. Please install jq."
    fi
    
    # Test bmcweb connectivity
    if ! curl ${CURL_OPTS} HEAD "${BASE_URL}/Sessions" \
        -H "Content-Type: application/json" \
        -u "${BMCWEB_USER}:${BMCWEB_PASSWORD}" > /dev/null 2>&1; then
        error "Cannot connect to bmcweb at ${BASE_URL}. Check BMCWEB_HOST and BMCWEB_PORT."
    fi
    
    success "Prerequisites check passed"
}

# Get session token
get_session_token() {
    log "Acquiring session token..."
    
    local response=$(curl -s ${CURL_OPTS} POST "${BASE_URL}/SessionService/Sessions" \
        -H "Content-Type: application/json" \
        -d "{\"UserName\": \"${BMCWEB_USER}\", \"Password\": \"${BMCWEB_PASSWORD}\"}" 2>/dev/null)
    
    local token=$(echo "$response" | jq -r '.SessionToken' 2>/dev/null)
    
    if [ -z "$token" ] || [ "$token" = "null" ]; then
        error "Failed to acquire session token. Response: $response"
    fi
    
    echo "$token"
}

# Create AggregationSource
create_agg_source() {
    local hostname="$1"
    local token="$2"
    
    if [ -z "$hostname" ]; then
        error "Usage: create_agg_source <hostname> <token>"
    fi
    
    log "Creating AggregationSource for ${hostname}..."
    
    local payload=$(cat <<EOF
{
    "HostName": "${hostname}",
    "UserName": "root",
    "Password": "0penBmc"
}
EOF
)
    
    local response=$(curl -s ${CURL_OPTS} POST "${BASE_URL}/AggregationService/AggregationSources" \
        -H "Content-Type: application/json" \
        -H "X-Auth-Token: ${token}" \
        -d "$payload" 2>/dev/null)
    
    local location=$(echo "$response" | jq -r '.@odata.id' 2>/dev/null)
    
    if [ -z "$location" ] || [ "$location" = "null" ]; then
        error "Failed to create AggregationSource. Response: $response"
    fi
    
    success "AggregationSource created: $location"
    echo "$location"
}

# Create EventSubscription
create_subscription() {
    local subscriber_url="http://${SUBSCRIBER_HOST}:${SUBSCRIBER_PORT}/events"
    local token="$1"
    
    log "Creating EventSubscription for ${subscriber_url}..."
    
    local payload=$(cat <<EOF
{
    "Destination": "${subscriber_url}",
    "EventFormatType": "Event",
    "ResourceTypes": ["AggregationSource"],
    "RegistryPrefixes": ["ResourceEvent"]
}
EOF
)
    
    local response=$(curl -s ${CURL_OPTS} POST "${BASE_URL}/EventService/Subscriptions" \
        -H "Content-Type: application/json" \
        -H "X-Auth-Token: ${token}" \
        -d "$payload" 2>/dev/null)
    
    local sub_id=$(echo "$response" | jq -r '.Id' 2>/dev/null)
    
    if [ -z "$sub_id" ] || [ "$sub_id" = "null" ]; then
        error "Failed to create subscription. Response: $response"
    fi
    
    success "Subscription created with ID: $sub_id"
    echo "$sub_id"
}

# Send test event
send_test_event() {
    local token="$1"
    
    log "Sending test event to EventService..."
    
    local payload=$(cat <<EOF
{
    "EventFormatType": "Event",
    "EventGroupId": 1,
    "EventTimestamp": "$(date -u +'%Y-%m-%dT%H:%M:%SZ')",
    "EventType": "ResourceCreated",
    "Message": "Test event for satellite event forwarding",
    "MessageArgs": ["test"],
    "MessageId": "Base.1.13.ResourceCreated",
    "OriginOfCondition": "/redfish/v1/AggregationService/AggregationSources/test",
    "Severity": "OK"
}
EOF
)
    
    local response=$(curl -s ${CURL_OPTS} POST "${BASE_URL}/EventService/SendTestEvent" \
        -H "Content-Type: application/json" \
        -H "X-Auth-Token: ${token}" \
        -d "$payload" 2>/dev/null)
    
    success "Test event sent. Response: $response"
}

# Verify satellite connectivity
verify_satellite() {
    local satellite_uri="$1"
    local token="$2"
    
    if [ -z "$satellite_uri" ]; then
        error "Usage: verify_satellite <satellite_uri> <token>"
    fi
    
    log "Verifying satellite at ${satellite_uri}..."
    
    local response=$(curl -s ${CURL_OPTS} GET "${BASE_URL}${satellite_uri}" \
        -H "X-Auth-Token: ${token}" 2>/dev/null)
    
    local health=$(echo "$response" | jq -r '.Status.Health' 2>/dev/null)
    
    if [ "$health" = "OK" ]; then
        success "Satellite health: OK"
    else
        warning "Satellite health: ${health:-Unknown}"
    fi
}

# List AggregationSources
list_agg_sources() {
    local token="$1"
    
    log "Listing AggregationSources..."
    
    curl -s ${CURL_OPTS} GET "${BASE_URL}/AggregationService/AggregationSources" \
        -H "X-Auth-Token: ${token}" 2>/dev/null | jq '.Members[] | {Id: .Id, HostName: .HostName}' || error "Failed to list AggregationSources"
}

# List subscriptions
list_subscriptions() {
    local token="$1"
    
    log "Listing EventService Subscriptions..."
    
    curl -s ${CURL_OPTS} GET "${BASE_URL}/EventService/Subscriptions" \
        -H "X-Auth-Token: ${token}" 2>/dev/null | jq '.Members[] | {Id: .Id, Destination: .Destination}' || error "Failed to list subscriptions"
}

# Delete subscription
delete_subscription() {
    local sub_id="$1"
    local token="$2"
    
    if [ -z "$sub_id" ] || [ -z "$token" ]; then
        error "Usage: delete_subscription <sub_id> <token>"
    fi
    
    log "Deleting subscription ${sub_id}..."
    
    curl -s ${CURL_OPTS} DELETE "${BASE_URL}/EventService/Subscriptions/${sub_id}" \
        -H "X-Auth-Token: ${token}" 2>/dev/null
    
    success "Subscription deleted"
}

# Full test flow
run_full_test() {
    log "=== Starting Full Test Suite ==="
    
    check_prerequisites
    
    local token=$(get_session_token)
    log "Session token acquired: ${token:0:20}..."
    
    # Create AggregationSource
    local sat_uri=$(create_agg_source "satellite1.local" "$token")
    
    # Verify connectivity
    verify_satellite "$sat_uri" "$token"
    
    # Create subscription
    local sub_id=$(create_subscription "$token")
    
    # Send test event
    send_test_event "$token"
    
    # List resources
    list_agg_sources "$token"
    list_subscriptions "$token"
    
    log "=== Full Test Suite Complete ==="
}

# Script entry point
main() {
    local command="${1:-full}"
    
    case "$command" in
        check)
            check_prerequisites
            ;;
        create-agg-source)
            local hostname="$2"
            local token=$(get_session_token)
            create_agg_source "$hostname" "$token"
            ;;
        create-subscription)
            local token=$(get_session_token)
            create_subscription "$token"
            ;;
        send-test-event)
            local token=$(get_session_token)
            send_test_event "$token"
            ;;
        verify-satellite)
            local sat_uri="$2"
            local token=$(get_session_token)
            verify_satellite "$sat_uri" "$token"
            ;;
        list-agg-sources)
            local token=$(get_session_token)
            list_agg_sources "$token"
            ;;
        list-subscriptions)
            local token=$(get_session_token)
            list_subscriptions "$token"
            ;;
        delete-subscription)
            local sub_id="$2"
            local token=$(get_session_token)
            delete_subscription "$sub_id" "$token"
            ;;
        full)
            run_full_test
            ;;
        help)
            cat <<EOF
End-to-End Event Forwarding Test Suite

Usage: $0 <command> [args]

Commands:
  check                           Check prerequisites
  create-agg-source <hostname>    Create an AggregationSource
  create-subscription             Create an EventSubscription
  send-test-event                 Send a test event
  verify-satellite <uri>          Verify satellite connectivity
  list-agg-sources                List all AggregationSources
  list-subscriptions              List all subscriptions
  delete-subscription <id>        Delete a subscription
  full                            Run complete test suite (default)
  help                            Show this help message

Environment variables:
  BMCWEB_HOST                     bmcweb host (default: localhost)
  BMCWEB_PORT                     bmcweb port (default: 18080)
  BMCWEB_USER                     bmcweb username (default: root)
  BMCWEB_PASSWORD                 bmcweb password (default: 0penBmc)
  SUBSCRIBER_HOST                 Event subscriber host (default: localhost)
  SUBSCRIBER_PORT                 Event subscriber port (default: 8888)

Examples:
  $0 check
  BMCWEB_HOST=bmc.example.com $0 list-agg-sources
  $0 create-agg-source satellite1.local
  $0 full

For testing, start a simple event listener first:
  python3 -m http.server 8888 --directory ./events &
EOF
            ;;
        *)
            error "Unknown command: $command. Use 'help' for usage."
            ;;
    esac
}

main "$@"
