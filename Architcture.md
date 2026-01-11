# Architecture

## Principles
- Follow Zephyr subsystem style: clear module boundaries, minimal coupling, proper initialization order.
- Communication is event-driven using zbus with well-defined message contracts.
- Every feature is operable via Zephyr shell following standard command patterns.
- Build health: unit tests + static analysis run regularly with Twister framework.
- Error handling: Use Zephyr error codes and proper logging subsystem.
- Memory management: Static allocation preferred, careful stack size considerations.

## Cline Implementation Principles
- Use modular file creation with write_to_file for each subsystem
- Use replace_in_file for targeted modifications to existing files
- Follow Zephyr coding guidelines in all Cline-generated code
- Use search_files to ensure consistency across the codebase
- Validate each change with west build before proceeding

## Directory Structure
```
app/
├── src/
│   ├── main.c              # Application entry point and initialization
│   ├── subsystems/
│   │   ├── sensor_mgr.c    # Internal sensor management
│   │   ├── display_mgr.c   # Display management and rendering
│   │   └── shell_iface.c   # Shell command interface
│   └── common/
│       ├── messages.h      # zbus message definitions
│       └── weather_common.h # Common constants and utilities
├── include/                # Public headers
├── tests/                  # Unit tests
└── boards/                 # Board-specific overlays
```

## Cline Directory Management
- Use write_to_file to create new subsystem directories and files
- Use list_files to verify directory structure before modifications
- Use search_files to find related files when making changes
- Always maintain proper directory structure as defined above

## Subsystems (MVP)

### 1) sensor_mgr (internal sensors)
**File**: `src/subsystems/sensor_mgr.c`

Responsibilities:
- Initialize and manage internal sensor devices using Zephyr sensor API
- Implement sampling policy with configurable intervals (Kconfig)
- On trigger: read sensors and publish a SensorData message to zbus
- Handle sensor errors gracefully with proper logging

Required Kconfig:
```
CONFIG_SENSOR=y
CONFIG_BME280=y  # Example sensor
CONFIG_WEATHER_STATION_SENSOR_MGR=y
CONFIG_WEATHER_STATION_SAMPLE_INTERVAL_MS=5000
```

Inputs:
- zbus channel: `ws_trigger` (messages requesting an update)
- Timer events for periodic sampling

Outputs:
- zbus channel: `ws_sensor_data` (latest measurements)
- Logging output for debug/status

## Cline Implementation Notes

### sensor_mgr Implementation with Cline
1. Use write_to_file to create `src/subsystems/sensor_mgr.c`
2. Use write_to_file to create `src/common/messages.h` with message definitions
3. Use replace_in_file to add sensor_mgr initialization to main.c
4. Use replace_in_file to add required Kconfig symbols to prj.conf
5. Use execute_command to validate with west build
6. Use search_files to verify zbus integration patterns

### 2) display_mgr
**File**: `src/subsystems/display_mgr.c`

Responsibilities:
- Subscribe to sensor_data and render current values
- Implement multiple display layouts (selectable via shell)
- Handle display driver initialization and error recovery
- Provide shell commands for view selection and forcing refresh

Required Kconfig:
```
CONFIG_DISPLAY=y
CONFIG_SSD1306=y  # Example display driver
CONFIG_WEATHER_STATION_DISPLAY_MGR=y
```

Inputs:
- zbus channel: `ws_sensor_data`
- Shell commands for view control

Outputs:
- Display updates
- Shell command responses

### 3) shell_iface
**File**: `src/subsystems/shell_iface.c`

Responsibilities:
- Implement shell commands following Zephyr patterns
- Commands to trigger sensor updates, print data, show status
- Provide configuration interface for runtime parameters

Required Kconfig:
```
CONFIG_SHELL=y
CONFIG_SHELL_BACKEND_SERIAL=y
CONFIG_WEATHER_STATION_SHELL=y
```

Commands:
- `ws trigger` - Request immediate sensor reading
- `ws show` - Display latest sensor data
- `ws status` - Show subsystem health and statistics
- `ws config` - Runtime configuration (sampling rate, display mode)

## Future subsystems
- **external_sensor_mgr** (BLE/WiFi sensor integration)
- **webserver_mgr** (HTTP API and web interface)
- **mqtt_mgr** (publish to broker)
- **storage_mgr** (data persistence and configuration)

## Zbus channels (MVP required)

### 1) channel: ws_trigger
```c
ZBUS_CHAN_DEFINE(ws_trigger,
                 struct trigger_msg,
                 NULL,                 /* validator */
                 NULL,                 /* user data */
                 ZBUS_OBSERVERS(sensor_mgr_sub),
                 ZBUS_MSG_INIT());
```
- **Purpose**: request a sensor update
- **Message type**: `struct trigger_msg`
- **Publishers**: shell_iface, timer subsystem
- **Subscribers**: sensor_mgr

### 2) channel: ws_sensor_data
```c
ZBUS_CHAN_DEFINE(ws_sensor_data,
                 struct sensor_data_msg,
                 NULL,                 /* validator */
                 NULL,                 /* user data */
                 ZBUS_OBSERVERS(display_mgr_sub, shell_iface_sub),
                 ZBUS_MSG_INIT());
```
- **Purpose**: publish latest sensor readings
- **Message type**: `struct sensor_data_msg`
- **Publishers**: sensor_mgr (later also external_sensor_mgr)
- **Subscribers**: display_mgr, shell_iface

## Cline Zbus Implementation
- Use write_to_file to create zbus channel definitions in messages.h
- Use search_files to find existing zbus patterns in the codebase
- Use replace_in_file to add channel initialization to main.c
- Use execute_command to validate zbus integration with west build
- Use list_code_definition_names to verify zbus usage patterns

## Data model

### Message Definitions (`src/common/messages.h`)
```c
struct trigger_msg {
    enum trigger_source {
        TRIGGER_MANUAL,     /* Shell command */
        TRIGGER_TIMER,      /* Periodic update */
        TRIGGER_EXTERNAL    /* External request */
    } source;
    uint32_t sequence;      /* Request sequence number */
};

struct sensor_data_msg {
    uint64_t timestamp;     /* k_uptime_get() value */
    float temperature_c;    /* Celsius, NaN if invalid */
    float humidity_percent; /* Relative humidity 0-100, NaN if invalid */
    float pressure_pa;      /* Pascals, NaN if invalid */
    uint32_t source_flags;  /* Bitmask: INTERNAL/EXTERNAL/etc */
    uint32_t sequence;      /* Monotonic counter */
    int status;            /* Error code or 0 for success */
};

#define SENSOR_SOURCE_INTERNAL  BIT(0)
#define SENSOR_SOURCE_EXTERNAL  BIT(1)
```

## Cline Data Model Implementation
- Use write_to_file to create complete message definitions
- Use search_files to verify message usage patterns
- Use replace_in_file to update message definitions when needed
- Always include proper SPDX headers and copyright notices
- Use execute_command to validate message compilation

## Threading model and Timing

### Threading Strategy
- **sensor_mgr**: Runs in dedicated thread with configurable priority
- **display_mgr**: Uses work queue for non-blocking display updates
- **shell_iface**: Executes in shell context, minimal blocking operations
- **zbus**: Uses system work queue for message delivery

### Priority Assignment
```c
#define SENSOR_MGR_THREAD_PRIORITY    K_PRIO_COOP(7)
#define DISPLAY_WORK_QUEUE_PRIORITY   K_PRIO_PREEMPT(8)
#define SHELL_THREAD_PRIORITY         K_PRIO_PREEMPT(14)  /* Default shell priority */
```

### Memory Considerations
- Stack sizes configured via Kconfig
- Message buffers: statically allocated
- Sensor data: no dynamic allocation
- Display buffers: static allocation preferred

## Cline Threading Implementation
- Use write_to_file to create thread definitions in subsystem files
- Use search_files to find existing threading patterns
- Use replace_in_file to add thread initialization to main.c
- Use execute_command to validate threading with west build
- Use list_code_definition_names to verify thread usage patterns

## Error Handling Strategy
- Use Zephyr error codes consistently
- Log errors using Zephyr logging subsystem
- Implement recovery mechanisms for device failures
- Provide status information via shell commands
- Handle message validation in zbus subscribers

## Cline Error Handling Implementation
- Use write_to_file to create error handling code in subsystems
- Use search_files to find existing error handling patterns
- Use replace_in_file to add error handling to existing code
- Use execute_command to test error scenarios
- Always include proper error logging and recovery mechanisms