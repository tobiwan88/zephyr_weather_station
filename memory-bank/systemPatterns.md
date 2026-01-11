# Zephyr Weather Station - System Patterns

## 🏗️ Architecture Overview
The Zephyr Weather Station follows a modular, event-driven architecture using Zephyr RTOS patterns and best practices.

## 🔧 Key Technical Decisions

### 1. ZBUS Messaging Pattern
**Decision**: Use ZBUS for inter-subsystem communication
**Rationale**:
- Decouples components for better maintainability
- Follows Zephyr RTOS recommended patterns
- Enables easy addition of new subscribers
- Provides type-safe message passing

**Implementation**:
```c
// Channel Definition (Global Scope)
ZBUS_CHAN_DEFINE(ws_sensor_data,
                struct sensor_data_msg,
                NULL,
                NULL,
                ZBUS_OBSERVERS(display_mgr_listener, shell_iface_listener),
                ZBUS_MSG_INIT());

// Publishing Data
zbus_chan_pub(ZBUS_REF(ws_sensor_data), &sensor_data, K_SECONDS(2));

// Listener Definition
ZBUS_LISTENER_DEFINE(sensor_mgr_listener, sensor_mgr_trigger_handler);
```

### 2. Sensor Simulation Pattern
**Decision**: Implement fake sensor with PRNG-based variations
**Rationale**:
- Enables development without hardware dependencies
- Provides realistic sensor behavior for testing
- Lightweight implementation suitable for embedded systems
- Easy to replace with real hardware drivers later

**Implementation**:
```c
// Simple PRNG for sensor variations
static uint32_t simple_prng(uint32_t *state)
{
    *state = *state * 1103515245U + 12345U;
    return *state;
}

// Sensor data with realistic variations
float temp_variation = ((float)simple_prng(&state) / 4294967295U) * 2.0f - 1.0f;
```

### 3. Shell Interface Pattern
**Decision**: Use Zephyr Shell for user interaction
**Rationale**:
- Standard Zephyr interface for debugging and control
- Provides interactive testing capabilities
- Easy to extend with new commands
- Consistent with Zephyr ecosystem patterns

**Implementation**:
```c
SHELL_STATIC_SUBCMD_SET_CREATE(
    ws_subcommands,
    SHELL_CMD(trigger, NULL, "Request immediate sensor reading", cmd_trigger),
    SHELL_CMD(show, NULL, "Display latest sensor data", cmd_show),
    SHELL_CMD(status, NULL, "Show subsystem health and statistics", cmd_status),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(ws, &ws_subcommands, "Weather Station commands", NULL);
```

### 4. Initialization Pattern
**Decision**: Use Zephyr's SYS_INIT for subsystem initialization
**Rationale**:
- Ensures proper initialization order
- Follows Zephyr RTOS conventions
- Automatic execution at system startup
- Clear dependency management

**Implementation**:
```c
static int sensor_mgr_init(void)
{
    LOG_INF("Sensor manager initialized");
    return 0;
}

SYS_INIT(sensor_mgr_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
```

## 📦 Component Relationships

### Subsystem Dependencies
```
┌─────────────────────────────────────────────────────────────┐
│                     Initialization Order                   │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  1. Zephyr Kernel (Core Services)                          │
│  2. ZBUS Subsystem                                         │
│  3. Sensor Framework                                       │
│  4. Shell Subsystem                                        │
│  5. Weather Station Components (APPLICATION priority)     │
│     ├─ Fake Sensor (POST_KERNEL)                           │
│     ├─ Sensor Manager (APPLICATION)                        │
│     ├─ Display Manager (APPLICATION)                       │
│     └─ Shell Interface (APPLICATION)                       │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### Runtime Data Flow
```
┌─────────────────────────────────────────────────────────────┐
│                      Runtime Operation                      │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  User Command  ┌───────────────────────────────────────┐   │
│  (Shell)       │               ZBUS                    │   │
│      │          │  ┌─────────────────────────────────┐  │   │
│      ▼          │  │                             │     │  │   │
│  ┌─────────┐    │  │  ┌─────────────────────┐  │     │  │   │
│  │         │    │  │  │                     │  │     │  │   │
│  │  Shell  │───▶│  │  │  Sensor Manager    │  │     │  │   │
│  │ Interface│    │  │  │                     │  │     │  │   │
│  │         │    │  │  │  (Trigger Handler) │  │     │  │   │
│  └─────────┘    │  │  └─────────────────────┘  │     │  │   │
│      ▲          │  │                             │     │  │   │
│      │          │  └─────────────────────────────┘     │  │   │
│      │          │                                    │  │   │
│      │          │  ┌─────────────────────────────────┐  │  │   │
│      │          │  │                                 │  │  │   │
│      │          │  │  ┌─────────────────────┐      │  │  │   │
│      │          │  │  │                     │      │  │  │   │
│      │          │  │  │  Fake Sensor        │      │  │  │   │
│      │          │  │  │                     │      │  │  │   │
│      │          │  │  │  (Data Provider)    │      │  │  │   │
│      │          │  │  └─────────────────────┘      │  │  │   │
│      │          │  │                                     │  │   │
│      │          │  └─────────────────────────────────┘  │  │   │
│      │          │                                          │   │
│      │          │  ┌─────────────────────────────────────┐  │   │
│      │          │  │                                     │  │   │
│      │          │  │  ┌─────────────────────┐          │  │   │
│      │          │  │  │                     │          │  │   │
│      │          │  │  │  Display Manager    │          │  │   │
│      │          │  │  │                     │          │  │   │
│      │          │  │  │  (Data Consumer)    │          │  │   │
│      │          │  │  └─────────────────────┘          │  │   │
│      │          │  │                                         │   │
│      │          │  └─────────────────────────────────────┘  │   │
│      │          │                                              │   │
│      └──────────┼──────────────────────────────────────────────┘   │
│               │ │                                                  │
│               │ └──────────────────────────────────────────────────┘
│               │
│               ▼
│          ┌─────────────────────┐
│          │                     │
│          │  User Feedback      │
│          │  (Shell Output)     │
│          │                     │
│          └─────────────────────┘

└─────────────────────────────────────────────────────────────┘
```

## 🎯 Critical Implementation Paths

### 1. Sensor Data Collection
**Path**: Shell Trigger → ZBUS Trigger → Sensor Manager → Fake Sensor → ZBUS Data → Display/Shell
**Critical Code**:
```c
// Trigger flow
struct trigger_msg trigger = {TRIGGER_MANUAL, sequence++};
zbus_chan_pub(ZBUS_REF(ws_trigger), &trigger, K_SECONDS(1));

// Data flow
struct sensor_data_msg sensor_data = {
    .timestamp = k_uptime_get(),
    .temperature_c = 22.5f,
    .humidity_percent = 45.0f,
    .pressure_pa = 101325.0f,
    .source_flags = SENSOR_SOURCE_INTERNAL,
    .sequence = sequence++,
    .status = 0
};
zbus_chan_pub(ZBUS_REF(ws_sensor_data), &sensor_data, K_SECONDS(2));
```

### 2. Error Handling Pattern
**Decision**: Use Zephyr error codes and logging
**Implementation**:
```c
int rc = zbus_chan_pub(ZBUS_REF(ws_sensor_data), &sensor_data, K_SECONDS(2));
if (rc != 0) {
    LOG_ERR("Failed to publish sensor data: %d", rc);
    return;
}
```

**Last Updated**: 2026-01-11
**Status**: Architecture stabilized, ready for testing and extension