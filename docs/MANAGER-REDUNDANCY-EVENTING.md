# Manager Redundancy Event Service – Gap Analysis and Implementation

## Branch

`1210-eventing-for-redundant-manager`

---

## 1. Background

bmcweb exposes Manager Redundancy state through
`GET /redfish/v1/Managers/{id}` when
`BMCWEB_EXPERIMENTAL_REDFISH_REDUNDANT_MANAGER=true`.  The data is
read live from the D-Bus interface
`xyz.openbmc_project.State.BMC.Redundancy` on object
`/xyz/openbmc_project/state/bmc0`.

The `ForceFailover` action (`POST .../Actions/Manager.ForceFailover`)
calls the D-Bus method `xyz.openbmc_project.Control.Failover.StartFailover`.

Before this branch **none** of the redundancy property changes triggered
a Redfish event to EventService subscribers.

---

## 2. D-Bus → Redfish Property Mapping

| D-Bus Property      | Redfish Field                    | Type                        |
|---------------------|----------------------------------|-----------------------------|
| `RedundancyEnabled` | `Redundancy[].Mode`              | `Failover` / `NotRedundant` |
| `FailoversAllowed`  | `Redundancy[].Status.State`      | `Enabled` / `Disabled`      |
| `FailoversAllowed`  | `Redundancy[].Status.Health`     | `OK` / `Warning`            |
| `RedundancyMinimum` | `MinNumNeededForFaultTolerance`  | integer                     |
| `RedundancyMaximum` | `MaxNumSupported`                | integer                     |
| `FunctionalMinimum` | `MinNumNeeded`                   | integer                     |
| `Role`              | `ActiveRedundancySet[]`          | member link (Active only)   |

---

## 3. Event Gap Inventory

### 3.1 Gaps Before This Branch

| ID | Trigger | Expected Event | Status Before |
|----|---------|----------------|---------------|
| R1 | Any property changes on `xyz.openbmc_project.State.BMC.Redundancy` | `ResourceEvent.ResourceChanged` for Manager | **MISSING** |
| R2 | `ForceFailover` action succeeds | `ResourceEvent.ResourceChanged` for Manager | **MISSING** |
| R3 | `RedundancyEnabled` flips | `ResourceEvent.ResourceChanged` (Mode change) | **MISSING** (via R1) |
| R4a | `FailoversAllowed` becomes `true` | `ResourceEvent.ResourceStatusChangedOK` | **MISSING** (via R1) |
| R4b | `FailoversAllowed` becomes `false` | `ResourceEvent.ResourceStatusChangedWarning` | **MISSING** (via R1) |
| R5 | `Role` changes Standby↔Active | `ResourceEvent.ResourceStateChanged` | **MISSING** (via R1) |
| R6 | Sibling BMC object added/removed from D-Bus subtree | `ResourceEvent.ResourceChanged` | **MISSING** (future) |

### 3.2 Coverage That Already Existed

| Area | Signal / Path | Event |
|------|---------------|-------|
| BMC general state | `xyz.openbmc_project.State.BMC` on `/xyz/openbmc_project/state/bmc0` | `ResourceChanged` for Manager |
| Host state | `xyz.openbmc_project.State.Host` on `/xyz/openbmc_project/state/host0` | `ResourceChanged` for ComputerSystem |
| Boot progress | `xyz.openbmc_project.State.Boot.Progress` | `ResourceChanged` for ComputerSystem |

---

## 4. Implemented Fixes (STEP C)

### 4.1 `include/event_dbus_monitor.hpp`

Added:

1. **`matchBMCRedundancyChange`** – static `shared_ptr<sdbusplus::bus::match::match>` (Gap R1)

2. **`bmcRedundancyPropertyChange()`** – signal handler with three dispatch branches:
   - `FailoversAllowed = true`  → `sendEvent(resourceStatusChangedOK(managerId, "OK"), ...)`
   - `FailoversAllowed = false` → `sendEvent(resourceStatusChangedWarning(managerId, "Warning"), ...)`
   - `Role` changes             → `sendEvent(resourceStateChanged(managerId, role), ...)`
   - Any other property         → `sendEvent(resourceChanged(), ...)` (covers R3)

3. **`registerBMCRedundancyChangeSignal()`** – registers the D-Bus match on:
   ```
   type='signal',member='PropertiesChanged',
   interface='org.freedesktop.DBus.Properties',
   path='/xyz/openbmc_project/state/bmc0',
   arg0='xyz.openbmc_project.State.BMC.Redundancy'
   ```
   Guarded by `if constexpr (!BMCWEB_EXPERIMENTAL_REDFISH_REDUNDANT_MANAGER)` so
   the match is only created when the feature is enabled.

4. **`registerStateChangeSignal()`** – now calls `registerBMCRedundancyChangeSignal()`
   after the existing host/BMC/boot-progress registrations.

### 4.2 `redfish-core/include/manager_redundancy.hpp`

Added **`sendEvent(messages::resourceChanged(), origin, "Manager")`** inside the
success branch of `handleManagerForceFailover()`'s async callback (Gap R2).  The
`managerId` parameter is now captured in the lambda to construct the origin URI.

---

## 5. Remaining Gaps (Future Work)

| ID | Description | Implementation Hint |
|----|-------------|---------------------|
| R6 | Sibling BMC added/removed from redundancy subtree | Register `InterfacesAdded`/`InterfacesRemoved` on `/xyz/openbmc_project/state` for `xyz.openbmc_project.State.BMC.Redundancy` |

---

## 6. Test Files Created

### STEP A – Unit Tests

| File | Tests | Purpose |
|------|-------|---------|
| [`test/redfish-core/include/missing_redundancy_events_test.cpp`](../test/redfish-core/include/missing_redundancy_events_test.cpp) | 20 | Document covered/missing gaps; lock helper payloads |

Tests are grouped:
- `ManagerRedundancyCovered` – 6 tests locking existing D-Bus→Redfish enum mapping
- `ManagerRedundancyMissing` – 14 tests; R1–R5 now implemented and green; R6 still skipped

### STEP B – Integration Tests

| File | Tests | Purpose |
|------|-------|---------|
| [`test/redfish-core/include/manager_redundancy_event_integration_test.cpp`](../test/redfish-core/include/manager_redundancy_event_integration_test.cpp) | 15 | EventService filter routing for Manager redundancy events |

Tests are grouped:
- `ManagerRedundancyIntegration` – 9 green tests covering filter routing
- `ManagerRedundancyIntegrationPlaceholder` – 6 tests; R1/R2/R4a/R4b/R5 now green (payload validation); R6 still skipped

### STEP B – E2E Script

| File | Purpose |
|------|---------|
| [`scripts/test-manager-redundancy-events-e2e.sh`](../scripts/test-manager-redundancy-events-e2e.sh) | curl-based HTTP E2E suite with gap analysis command |

---

## 7. Build Integration

New test files registered in [`test/meson.build`](../test/meson.build):

```meson
'redfish-core/include/missing_redundancy_events_test.cpp',
'redfish-core/include/manager_redundancy_event_integration_test.cpp',
```

---

## 8. Event Coverage Matrix (After This Branch)

| Trigger | Message | Origin | Resource Type | Status |
|---------|---------|--------|---------------|--------|
| `FailoversAllowed` → `true` | `ResourceStatusChangedOK` | `/redfish/v1/Managers/{id}` | `Manager` | ✅ Implemented |
| `FailoversAllowed` → `false` | `ResourceStatusChangedWarning` | `/redfish/v1/Managers/{id}` | `Manager` | ✅ Implemented |
| `Role` → Active/Standby | `ResourceStateChanged` | `/redfish/v1/Managers/{id}` | `Manager` | ✅ Implemented |
| Any other `BMC.Redundancy` prop | `ResourceChanged` | `/redfish/v1/Managers/{id}` | `Manager` | ✅ Implemented |
| `ForceFailover` success | `ResourceChanged` | `/redfish/v1/Managers/{id}` | `Manager` | ✅ Implemented |
| Sibling BMC appears/disappears | `ResourceChanged` | `/redfish/v1/Managers/{id}` | `Manager` | 📋 Future (R6) |

---

## 9. Subscriber Filter Examples

### Subscribe to all Manager redundancy events

```json
{
  "Destination": "https://listener.example.com/events",
  "EventFormatType": "Event",
  "ResourceTypes": ["Manager"],
  "RegistryPrefixes": ["ResourceEvent"]
}
```

### Subscribe to health-change events only

Use `MessageIds` filter:

```json
{
  "Destination": "https://listener.example.com/events",
  "EventFormatType": "Event",
  "MessageIds": [
    "ResourceEvent.1.3.ResourceStatusChangedOK",
    "ResourceEvent.1.3.ResourceStatusChangedWarning"
  ]
}
```

### Subscribe to a specific Manager's redundancy events

```json
{
  "Destination": "https://listener.example.com/events",
  "EventFormatType": "Event",
  "OriginResources": [
    { "@odata.id": "/redfish/v1/Managers/bmc" }
  ]
}
```

---

## 10. Running Tests

```bash
# Build
cd build && ninja

# STEP A unit tests
meson test missing_redundancy_events_test -v

# STEP B integration tests
meson test manager_redundancy_event_integration_test -v

# E2E gap analysis (no running BMC needed)
./scripts/test-manager-redundancy-events-e2e.sh gap-analysis

# E2E full run (requires bmcweb + D-Bus redundancy service + event listener)
python3 scripts/redfish-event-listener.py --daemon &
./scripts/test-manager-redundancy-events-e2e.sh full
```

---

## 11. Parallel AggregationService Work

This branch mirrors the structure of the AggregationService eventing work:

| | AggregationService | Manager Redundancy |
|--|--------------------|--------------------|
| Unit tests (STEP A) | `missing_aggregation_events_test.cpp` | `missing_redundancy_events_test.cpp` |
| Integration tests (STEP B) | `satellite_event_forwarding_integration_test.cpp` | `manager_redundancy_event_integration_test.cpp` |
| E2E script (STEP B) | `test-satellite-events-e2e.sh` | `test-manager-redundancy-events-e2e.sh` |
| Implementation (STEP C) | `aggregation_service.hpp`, `redfish_aggregator.hpp` | `event_dbus_monitor.hpp`, `manager_redundancy.hpp` |
