# Plan

## Common commands
- **Build**: `west build -b native_sim -d build -p always app`
- **Build for board**: `west build -b <board> -d build -p always app`
- **Flash** (optional): `west flash -d build` (requires ALLOW_DEVICE_CMDS=1)
- **Shell**: `west build -t run` for native_sim, or monitor via UART for real boards
- **Unit tests**: `west twister -T tests -p native_sim` (quick iteration)
- **Full test suite**: `west twister -T tests` (all platforms)
- **Static analysis**: `west sca` (run per change-set / in CI)
- **Clean build**: `west build -b native_sim -d build -p always --pristine app`

## Development Workflow
1. Implement feature following Architecture.md patterns
2. Add unit tests in `tests/` directory
3. Update Kconfig and prj.conf as needed
4. Test on native_sim first: `west build -b native_sim -p always app && west build -t run`
5. Run tests: `west twister -T tests -p native_sim`
6. Run static analysis: `west sca`
7. Update documentation if architecture changes

## Cline Development Workflow
1. **Context Understanding**: Use read_file and search_files to understand current implementation
2. **Implementation**: Use write_to_file for new files, replace_in_file for modifications
3. **Validation**: Use execute_command to run build and test commands
4. **Progress Tracking**: Use task_progress parameter to track milestone completion
5. **Completion**: Use attempt_completion when milestone is reached
6. **Debugging**: Use search_files and list_code_definition_names for pattern analysis

## Milestone 1 (MVP): internal sensor + display + zbus + shell

### Prerequisites
- Set up basic project structure with proper CMakeLists.txt
- Configure prj.conf with required subsystems:
  ```
  CONFIG_ZBUS=y
  CONFIG_SHELL=y
  CONFIG_SENSOR=y
  CONFIG_DISPLAY=y
  CONFIG_LOG=y
  ```

### Deliverables
1. **zbus infrastructure**:
   - Define `ws_trigger` and `ws_sensor_data` channels in `src/common/messages.h`
   - Implement message structures with proper validation
   - Add channel initialization in main.c

2. **sensor_mgr subsystem** (`src/subsystems/sensor_mgr.c`):
   - Initialize sensor device using Zephyr sensor API
   - Subscribe to `ws_trigger` channel
   - Read sensor on trigger and publish to `ws_sensor_data`
   - Handle sensor errors gracefully
   - Implement configurable sampling with Kconfig

3. **display_mgr subsystem** (`src/subsystems/display_mgr.c`):
   - Initialize display device
   - Subscribe to `ws_sensor_data` channel
   - Render sensor values in readable format
   - Support multiple display layouts (basic text, formatted)
   - Handle display errors and recovery

4. **shell_iface subsystem** (`src/subsystems/shell_iface.c`):
   - Implement shell commands following Zephyr patterns:
     - `ws trigger` - Request immediate sensor reading
     - `ws show` - Print latest SensorDataMsg in human-readable format
     - `ws status` - Show subsystem health and counters
     - `ws config set <param> <value>` - Runtime configuration
     - `ws config get <param>` - Show current configuration

5. **Unit tests** (`tests/`):
   - Message encoding/decoding validation
   - sensor_mgr publish behavior (with mocked sensor)
   - display_mgr formatting logic
   - Shell command parsing and responses
   - Error handling scenarios

6. **Documentation updates**:
   - Update CMakeLists.txt project name and sources
   - Document required Kconfig symbols
   - Create example prj.conf with all required options

### Implementation Order
1. **Phase 1.1**: Basic zbus setup and message definitions
2. **Phase 1.2**: sensor_mgr with mock sensor for native_sim
3. **Phase 1.3**: display_mgr with console output fallback
4. **Phase 1.4**: shell_iface with basic commands
5. **Phase 1.5**: Integration testing and refinement
6. **Phase 1.6**: Unit test suite completion

### Cline Implementation Approach
1. **Phase 1.1**: Use write_to_file to create message definitions and zbus infrastructure
2. **Phase 1.2**: Use write_to_file for sensor_mgr.c, use replace_in_file for main.c integration
3. **Phase 1.3**: Use write_to_file for display_mgr.c, test with execute_command
4. **Phase 1.4**: Use write_to_file for shell_iface.c, validate with west build
5. **Phase 1.5**: Use search_files to find integration points, use replace_in_file for fixes
6. **Phase 1.6**: Use write_to_file for test files, validate with west twister

### Acceptance Criteria
- `west build -b native_sim -p always app && west build -t run` succeeds
- `ws trigger` command causes a new SensorDataMsg publish (sequence increments)
- Display updates after publish (visible in console/display)
- `ws show` prints current sensor data in readable format
- `ws status` shows all subsystems healthy
- `west twister -T tests -p native_sim` passes all tests
- `west sca` runs clean (or documented baseline violations)
- Memory usage stays within 32KB RAM, 64KB flash for native_sim
- No memory leaks or stack overflows detected

### Cline Validation Criteria
- All file operations use appropriate Cline tools (write_to_file, replace_in_file)
- Changes follow Zephyr coding guidelines and architecture patterns
- task_progress parameter is used to track implementation progress
- execute_command is used for build and test validation
- attempt_completion is used when milestone is reached
- All changes are validated with west build and west twister

### Testing Strategy
- Primary testing on native_sim for development speed
- Mock sensor driver for repeatable testing
- Console-based display output for CI testing
- Stress testing: rapid trigger commands, message flooding
- Error injection: sensor read failures, display errors

### Cline Testing Approach
- Use execute_command to run west build and west twister commands
- Use search_files to verify test coverage across subsystems
- Use replace_in_file to add test cases when needed
- Use list_code_definition_names to verify test structure
- Always validate changes on native_sim before proceeding

## Milestone 2: external sensors (BLE or other)

### Prerequisites
- Milestone 1 complete and stable
- BLE stack configuration (if using BLE sensors)
- Network configuration for WiFi sensors

### Deliverables
1. **external_sensor_mgr subsystem** (`src/subsystems/external_sensor_mgr.c`):
   - BLE GATT client or WiFi HTTP client implementation
   - Device discovery and pairing logic
   - Data parsing and validation
   - Merge external data into `ws_sensor_data` channel
   - Configuration for sensor addresses/endpoints

2. **Enhanced message format**:
   - Extend `sensor_data_msg` with source identification
   - Support multiple sensor sources in single message
   - Add data quality indicators and timestamps per source

3. **Shell commands for external sensors**:
   - `ws external scan` - Discover available sensors
   - `ws external pair <address>` - Pair with BLE device
   - `ws external config <url>` - Configure WiFi sensor endpoint
   - `ws external status` - Show external sensor states

4. **Unit tests**:
   - External data parsing and validation
   - Merge logic for internal + external data
   - Connection error handling
   - Configuration validation

### Acceptance Criteria
- Can inject external sensor data and see it displayed
- Shell commands work for configuration and monitoring
- System gracefully handles external sensor disconnections
- Data integrity maintained when mixing internal/external sources

## Milestone 3: webserver (display + config)

### Prerequisites
- Milestone 2 complete
- Network stack configured (WiFi or Ethernet)
- HTTP server library integrated

### Deliverables
1. **webserver_mgr subsystem** (`src/subsystems/webserver_mgr.c`):
   - HTTP server with REST API endpoints
   - JSON serialization of sensor data
   - Web interface for configuration
   - Authentication/authorization (basic)

2. **HTTP endpoints**:
   - `GET /api/sensors` - Current sensor readings (JSON)
   - `GET /api/status` - System status and health
   - `POST /api/config` - Update configuration
   - `GET /` - Web interface (static files)

3. **Enhanced configuration system**:
   - Persistent storage for settings
   - Runtime configuration validation
   - Configuration backup/restore

4. **Tests for web interface**:
   - API endpoint responses
   - Configuration validation
   - Security (input sanitization)

### Acceptance Criteria
- Web endpoint shows current data with proper JSON format
- Configuration changes via web interface affect system behavior
- Web interface is responsive and user-friendly
- Security: no obvious vulnerabilities in config endpoints

## Milestone 4: MQTT publish

### Prerequisites
- Milestone 3 complete
- MQTT client library configured
- Network connectivity established

### Deliverables
1. **mqtt_mgr subsystem** (`src/subsystems/mqtt_mgr.c`):
   - MQTT client with automatic reconnection
   - Subscribe to `ws_sensor_data` and publish to broker
   - Configurable topics, QoS, and retention
   - Status reporting and error handling

2. **MQTT integration**:
   - JSON payload formatting for sensor data
   - Topic structure: `weather-station/{device-id}/{sensor-type}`
   - Support for multiple brokers and failover
   - TLS/SSL support for secure connections

3. **Shell commands for MQTT**:
   - `ws mqtt connect <broker>` - Connect to MQTT broker
   - `ws mqtt status` - Show connection and publish statistics
   - `ws mqtt config` - Configure topics and credentials

4. **Tests for MQTT functionality**:
   - Payload formatting (unit tests)
   - Connection state handling and recovery
   - Message delivery confirmation

### Acceptance Criteria
- Sensor values appear on MQTT broker with correct topic/payload format
- Automatic reconnection works when network is interrupted
- Configuration persists across restarts
- No message loss during normal operation conditions

## Release Criteria (All Milestones)
- Complete test coverage with Twister
- Documentation updated and accurate
- Memory usage profiled and optimized
- Performance benchmarks established
- Static analysis clean
- Code review completed
- Example configurations for common boards
- User guide with setup instructions