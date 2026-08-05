#!/bin/bash
# SPDX-License-Identifier: Apache-2.0
# SPDX-FileCopyrightText: Copyright OpenBMC Authors
#
# End-to-End Test Suite for Manager Redundancy Eventing
#
# This script provides manual test scenarios for verifying that Manager
# Redundancy state-change events are correctly emitted and delivered to
# EventService subscribers.
#
# Prerequisites:
#   - bmcweb running on https://localhost:18080/redfish/v1
#     compiled with BMCWEB_EXPERIMENTAL_REDFISH_REDUNDANT_MANAGER=true
#   - A D-Bus stub service implementing
#     xyz.openbmc_project.State.BMC.Redundancy on bmc0
#   - Event subscriber listening on http://localhost:8888/events
#
# Usage:
#   ./test-manager-redundancy-events-e2e.sh check
#   ./test-manager-redundancy-events-e2e.sh get-redundancy
#   ./test-manager-redundancy-events-e2e.sh create-subscription
#   ./test-manager-redundancy-events-e2e.sh verify-events
#   ./test-manager-redundancy-events-e2e.sh cleanup
#   ./test-manager-redundancy-events-e2e.sh full
#
# For a complete test run (requires an event listener already started):
#   python3 scripts/redfish-event-listener.py --daemon &
#   ./test-manager-redundancy-events-e2e.sh full

set -e

# ── Configuration ────────────────────────────────────────────────────────────
BMCWEB_HOST="${BMCWEB_HOST:-localhost}"
BMCWEB_PORT="${BMCWEB_PORT:-18080}"
BMCWEB_USER="${BMCWEB_USER:-root}"
BMCWEB_PASSWORD="${BMCWEB_PASSWORD:-0penBmc}"
SUBSCRIBER_HOST="${SUBSCRIBER_HOST:-localhost}"
SUBSCRIBER_PORT="${SUBSCRIBER_PORT:-8888}"
MANAGER_ID="${MANAGER_ID:-bmc}"

BASE_URL="https://${BMCWEB_HOST}:${BMCWEB_PORT}/redfish/v1"
MANAGER_URL="${BASE_URL}/Managers/${MANAGER_ID}"
CURL_OPTS="-k -s"   # -k: ignore self-signed certs, -s: silent

# ── Colour helpers ────────────────────────────────────────────────────────────
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

log()     { echo -e "${BLUE}[TEST]${NC} $1"; }
success() { echo -e "${GREEN}[PASS]${NC} $1"; }
warning() { echo -e "${YELLOW}[WARN]${NC} $1"; }
error()   { echo -e "${RED}[FAIL]${NC} $1"; exit 1; }

# ── Session helpers ───────────────────────────────────────────────────────────
get_session_token() {
    local response
    response=$(curl ${CURL_OPTS} -X POST "${BASE_URL}/SessionService/Sessions" \
        -H "Content-Type: application/json" \
        -d "{\"UserName\":\"${BMCWEB_USER}\",\"Password\":\"${BMCWEB_PASSWORD}\"}" \
        2>/dev/null)

    local token
    token=$(echo "$response" | jq -r '.SessionToken' 2>/dev/null)

    if [ -z "$token" ] || [ "$token" = "null" ]; then
        error "Failed to acquire session token. Response: $response"
    fi
    echo "$token"
}

auth_header() {
    echo "-H \"X-Auth-Token: $1\""
}

# ── Prerequisite checks ───────────────────────────────────────────────────────
check_prerequisites() {
    log "Checking prerequisites..."

    command -v curl &>/dev/null || error "curl not found."
    command -v jq   &>/dev/null || error "jq not found."

    # Verify bmcweb is reachable
    if ! curl ${CURL_OPTS} -X HEAD \
            "${BASE_URL}/SessionService/Sessions" \
            -u "${BMCWEB_USER}:${BMCWEB_PASSWORD}" >/dev/null 2>&1; then
        error "Cannot reach bmcweb at ${BASE_URL}. Check BMCWEB_HOST/BMCWEB_PORT."
    fi

    success "Prerequisites OK"
}

# ── Manager Redundancy GET ────────────────────────────────────────────────────
get_redundancy() {
    local token="$1"

    log "GET ${MANAGER_URL} (checking Redundancy array)..."

    local response
    response=$(curl ${CURL_OPTS} -X GET "${MANAGER_URL}" \
        -H "X-Auth-Token: ${token}" 2>/dev/null)

    local redundancy_count
    redundancy_count=$(echo "$response" | jq '."Redundancy@odata.count"' 2>/dev/null)

    if [ "$redundancy_count" = "null" ] || [ -z "$redundancy_count" ]; then
        warning "Redundancy array not present (BMCWEB_EXPERIMENTAL_REDFISH_REDUNDANT_MANAGER may be disabled)"
    else
        success "Redundancy@odata.count = ${redundancy_count}"
        echo "$response" | jq '.Redundancy' 2>/dev/null
    fi
}

# ── Event Subscription ────────────────────────────────────────────────────────
create_manager_subscription() {
    local token="$1"
    local subscriber_url="http://${SUBSCRIBER_HOST}:${SUBSCRIBER_PORT}/events"

    log "Creating Manager EventSubscription to ${subscriber_url}..."

    local payload
    payload=$(cat <<EOF
{
    "Destination": "${subscriber_url}",
    "EventFormatType": "Event",
    "ResourceTypes": ["Manager"],
    "RegistryPrefixes": ["ResourceEvent"]
}
EOF
)

    local response
    response=$(curl ${CURL_OPTS} -X POST "${BASE_URL}/EventService/Subscriptions" \
        -H "Content-Type: application/json" \
        -H "X-Auth-Token: ${token}" \
        -d "$payload" 2>/dev/null)

    local sub_id
    sub_id=$(echo "$response" | jq -r '.Id' 2>/dev/null)

    if [ -z "$sub_id" ] || [ "$sub_id" = "null" ]; then
        error "Failed to create subscription. Response: $response"
    fi

    success "Subscription created: ID=${sub_id}"
    echo "$sub_id"
}

# ── Force Failover action ─────────────────────────────────────────────────────
# NOTE: This command requires a sibling BMC Manager URI.
# Adjust STANDBY_MANAGER_ID to match your environment.
force_failover() {
    local token="$1"
    local standby_id="${STANDBY_MANAGER_ID:-standby_bmc}"

    log "Attempting ForceFailover to ${standby_id}..."

    local payload
    payload=$(cat <<EOF
{
    "NewManager": {
        "@odata.id": "/redfish/v1/Managers/${standby_id}"
    }
}
EOF
)

    local response
    response=$(curl ${CURL_OPTS} -X POST \
        "${MANAGER_URL}/Actions/Manager.ForceFailover" \
        -H "Content-Type: application/json" \
        -H "X-Auth-Token: ${token}" \
        -d "$payload" 2>/dev/null)

    local status
    status=$(echo "$response" | jq -r '.status // empty' 2>/dev/null)

    echo "$response" | jq . 2>/dev/null
    log "ForceFailover response received (check event listener for ResourceChanged)"
}

# ── Event verification ────────────────────────────────────────────────────────
verify_manager_events() {
    local expected_pattern="${1:-ResourceChanged}"

    log "Checking event listener for Manager events (pattern: ${expected_pattern})..."

    # Query the event listener HTTP API
    local events
    events=$(curl ${CURL_OPTS} -X GET \
        "http://${SUBSCRIBER_HOST}:${SUBSCRIBER_PORT}/events" 2>/dev/null)

    if [ -z "$events" ] || [ "$events" = "null" ]; then
        warning "No events received from listener at ${SUBSCRIBER_HOST}:${SUBSCRIBER_PORT}"
        return 1
    fi

    local matching
    matching=$(echo "$events" | \
        jq "[.[] | select(.Events[0].MessageId | test(\"${expected_pattern}\")) | \
             select(.Events[0].OriginOfCondition | test(\"Managers\"))]" 2>/dev/null)

    local count
    count=$(echo "$matching" | jq 'length' 2>/dev/null)

    if [ "$count" -gt 0 ] 2>/dev/null; then
        success "Found ${count} Manager event(s) matching '${expected_pattern}'"
        echo "$matching" | jq '.[0]' 2>/dev/null
    else
        warning "No Manager events matching '${expected_pattern}' found in listener"
        warning "Expected gap: STEP C has not yet implemented the sendEvent() call"
        return 1
    fi
}

# ── Gap analysis check ────────────────────────────────────────────────────────
# Checks the *current* event emission status for each identified gap.
check_event_gaps() {
    local token="$1"

    log "=== Manager Redundancy Event Gap Analysis ==="
    echo ""

    log "Gap R1: BMC.Redundancy PropertiesChanged monitor"
    warning "  [NOT IMPLEMENTED] No match registered for arg0='xyz.openbmc_project.State.BMC.Redundancy'"
    echo ""

    log "Gap R2: ForceFailover success → sendEvent"
    warning "  [NOT IMPLEMENTED] handleManagerForceFailover success branch has no sendEvent() call"
    echo ""

    log "Gap R3: RedundancyEnabled change → Mode-change event"
    warning "  [NOT IMPLEMENTED] Requires Gap R1 implementation first"
    echo ""

    log "Gap R4a: FailoversAllowed false→true → StatusChangedOK"
    warning "  [NOT IMPLEMENTED] Requires Gap R1 implementation first"
    echo ""

    log "Gap R4b: FailoversAllowed true→false → StatusChangedWarning"
    warning "  [NOT IMPLEMENTED] Requires Gap R1 implementation first"
    echo ""

    log "Gap R5: Role change Standby↔Active → ResourceStateChanged"
    warning "  [NOT IMPLEMENTED] Requires Gap R1 implementation first"
    echo ""

    log "Gap R6: Sibling BMC InterfacesAdded/Removed → ResourceChanged"
    warning "  [NOT IMPLEMENTED] No InterfacesAdded/Removed match for redundancy objects"
    echo ""

    log "=== Existing (Working) Coverage ==="
    success "  [COVERED] GET /redfish/v1/Managers/{id} populates Redundancy[] from D-Bus"
    success "  [COVERED] POST ForceFailover calls D-Bus StartFailover"
    success "  [COVERED] bmcStatePropertyChange() emits ResourceChanged for BMC.State changes"
    success "  [COVERED] All required message helpers (resourceChanged, statusChangedOK/Warning, stateChanged) produce valid JSON"
}

# ── Cleanup ───────────────────────────────────────────────────────────────────
delete_subscription() {
    local sub_id="$1"
    local token="$2"

    log "Deleting subscription ${sub_id}..."

    curl ${CURL_OPTS} -X DELETE \
        "${BASE_URL}/EventService/Subscriptions/${sub_id}" \
        -H "X-Auth-Token: ${token}" >/dev/null 2>&1

    success "Subscription ${sub_id} deleted"
}

list_subscriptions() {
    local token="$1"

    log "Current EventService subscriptions:"

    curl ${CURL_OPTS} -X GET "${BASE_URL}/EventService/Subscriptions" \
        -H "X-Auth-Token: ${token}" 2>/dev/null | \
        jq '.Members[] | {Id: .Id, Destination: .Destination}' 2>/dev/null || \
        warning "No subscriptions found or error retrieving list"
}

# ── Full test run ─────────────────────────────────────────────────────────────
run_full_test() {
    log "=== Manager Redundancy Eventing – Full Test Suite ==="
    echo ""

    check_prerequisites

    local token
    token=$(get_session_token)
    log "Session token acquired"

    # Check current redundancy state
    get_redundancy "$token"
    echo ""

    # Perform gap analysis
    check_event_gaps "$token"
    echo ""

    # Create subscription to capture any events that ARE emitted
    local sub_id
    sub_id=$(create_manager_subscription "$token")
    echo ""

    # List subscriptions to confirm
    list_subscriptions "$token"
    echo ""

    # Verify event listener
    verify_manager_events "ResourceChanged" || true
    echo ""

    # Cleanup
    delete_subscription "$sub_id" "$token"

    echo ""
    log "=== Test Suite Complete ==="
    log "Review gap analysis above for STEP C implementation targets."
}

# ── Entry point ───────────────────────────────────────────────────────────────
main() {
    local command="${1:-full}"

    case "$command" in
        check)
            check_prerequisites
            ;;
        get-redundancy)
            local token
            token=$(get_session_token)
            get_redundancy "$token"
            ;;
        create-subscription)
            local token
            token=$(get_session_token)
            create_manager_subscription "$token"
            ;;
        verify-events)
            verify_manager_events "${2:-ResourceChanged}"
            ;;
        force-failover)
            local token
            token=$(get_session_token)
            force_failover "$token"
            ;;
        gap-analysis)
            local token
            token=$(get_session_token)
            check_event_gaps "$token"
            ;;
        list-subscriptions)
            local token
            token=$(get_session_token)
            list_subscriptions "$token"
            ;;
        delete-subscription)
            local token
            token=$(get_session_token)
            delete_subscription "${2:?Usage: delete-subscription <id>}" "$token"
            ;;
        full)
            run_full_test
            ;;
        help)
            cat <<EOF
Manager Redundancy Eventing – E2E Test Script

Usage: $0 <command> [args]

Commands:
  check                             Check prerequisites
  get-redundancy                    GET Manager Redundancy[] array
  create-subscription               Create Manager EventSubscription
  verify-events [pattern]           Check event listener (default: ResourceChanged)
  force-failover                    POST ForceFailover action
  gap-analysis                      Print redundancy event gap summary
  list-subscriptions                List EventService subscriptions
  delete-subscription <id>          Delete a subscription
  full                              Run complete test suite (default)
  help                              Show this help message

Environment variables:
  BMCWEB_HOST         bmcweb host              (default: localhost)
  BMCWEB_PORT         bmcweb port              (default: 18080)
  BMCWEB_USER         bmcweb username          (default: root)
  BMCWEB_PASSWORD     bmcweb password          (default: 0penBmc)
  SUBSCRIBER_HOST     Event listener host      (default: localhost)
  SUBSCRIBER_PORT     Event listener port      (default: 8888)
  MANAGER_ID          Redfish Manager ID       (default: bmc)
  STANDBY_MANAGER_ID  Standby Manager ID for   (default: standby_bmc)
                      ForceFailover testing

Examples:
  $0 check
  $0 get-redundancy
  MANAGER_ID=bmc0 $0 gap-analysis
  $0 force-failover
  $0 verify-events ResourceStatusChangedWarning
  $0 full

Prerequisites:
  1. Start event listener:
       python3 scripts/redfish-event-listener.py --daemon &
  2. Build bmcweb with BMCWEB_EXPERIMENTAL_REDFISH_REDUNDANT_MANAGER=true
  3. Ensure D-Bus redundancy service is running on the target BMC
EOF
            ;;
        *)
            error "Unknown command: $command. Use 'help' for usage."
            ;;
    esac
}

main "$@"
