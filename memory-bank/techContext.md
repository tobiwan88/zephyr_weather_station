# Zephyr Weather Station - Technical Context

## 🛠️ Technologies Used

### Core Technologies
- **Zephyr RTOS 4.3.0**: Real-time operating system foundation
- **West Build System**: Zephyr's meta-tool for building and flashing
- **C Language**: Primary development language (C11 standard)
- **CMake**: Build system configuration
- **Ninja**: Build system execution
- **Git**: Version control

### Zephyr Subsystems
- **ZBUS**: Message bus for inter-thread communication
- **Sensor Framework**: Standardized sensor driver interface
- **Shell**: Interactive command interface
- **Logging**: System logging infrastructure
- **Device Tree**: Hardware description and configuration

## 💻 Development Setup

### Build Environment
- **Operating System**: Linux (Ubuntu/Debian recommended)
- **Container**: Docker-based development environment
- **Python**: 3.10+ (for West and build tools)
- **Toolchain**: Zephyr SDK with GCC cross-compilation tools
- **Build Target**: native_sim/native/64 (POSIX-based simulator)

### Required Tools
```bash
# Essential tools
sudo apt-get install git cmake ninja-build dfu-util device-tree-compiler

# Python dependencies
pip install west

# Zephyr SDK (provided in container)
```

### Build Commands
```bash
# Activate virtual environment
source /home/zephyr/.venv/bin/activate

# Source Zephyr environment
source /workspace/zephyr/zephyr-env.sh

# Build for native simulator (64-bit)
west build zephyr_weather_station/app -b native_sim/native/64 --pristine

# Run the application
west build -d build run
```

## ⚙️ Technical Constraints

### Platform Constraints
- **Memory**: Must fit within native_sim memory constraints
- **Performance**: Real-time requirements for sensor data processing
- **Compatibility**: Must work across different Zephyr board targets
- **Footprint**: Minimal resource usage for embedded deployment

### Development Constraints
- **Build System**: Must use West/CMake/Ninja workflow
- **Coding Standards**: Must follow Zephyr coding conventions
- **Testing**: Must support both simulation and hardware testing
- **Documentation**: Must maintain comprehensive documentation

## 📦 Dependencies

### External Dependencies
- **Zephyr RTOS**: Core operating system (version 4.3.0)
- **Zephyr SDK**: Toolchain and development tools
- **Python 3.10+**: For build system and tools
- **CMake 3.20+**: Build system configuration
- **Ninja**: Build system execution

### Internal Dependencies
- **ZBUS Subsystem**: For inter-component communication
- **Sensor Framework**: For sensor driver interface
- **Shell Subsystem**: For user interaction
- **Logging Subsystem**: For system diagnostics

## 🔧 Tool Usage Patterns

### Build System Patterns
```bash
# Clean build (recommended for reliability)
west build -b native_sim/native/64 --pristine

# Incremental build
west build -b native_sim/native/64

# Build with specific configuration
west build -b native_sim/native/64 -DCONFIG_WEATHER_STATION_FAKE_SENSOR=y
```

### Debugging Patterns
```bash
# Run with debug output
west build -d build run -- -v

# Attach debugger
west debug

# View logs
west esp serial-monitor
```

### Development Workflow
```bash
# Update Zephyr and modules
west update

# Add new modules
west manifest add <module>

# Flash to hardware (when available)
west flash
```

## 📋 Configuration Management

### Kconfig System
- **Project Configuration**: `app/prj.conf`
- **Board Configuration**: `boards/<board>/<board>_defconfig`
- **Application Symbols**: `app/Kconfig`

### Key Configuration Symbols
```kconfig
# Enable required subsystems
CONFIG_ZBUS=y
CONFIG_SENSOR=y
CONFIG_SHELL=y
CONFIG_LOG=y

# Application-specific
CONFIG_WEATHER_STATION_FAKE_SENSOR=y
CONFIG_WEATHER_STATION_LOG_LEVEL=4
```

## 🎯 Development Best Practices

### Coding Standards
- **Naming**: Follow Zephyr conventions (snake_case, UPPER_CASE)
- **Documentation**: Use Doxygen-style comments for public APIs
- **Error Handling**: Use Zephyr error codes (`-EINVAL`, `-ENOMEM`, etc.)
- **Memory Management**: Avoid dynamic allocation where possible

### Testing Patterns
- **Unit Testing**: Use Zephyr's ztest framework
- **Integration Testing**: Use shell commands for manual testing
- **Simulation Testing**: Leverage native_sim for development testing
- **Hardware Testing**: Validate on target hardware platforms

### Performance Considerations
- **Stack Usage**: Monitor stack usage with `CONFIG_THREAD_STACK_INFO`
- **Memory Usage**: Track memory with `CONFIG_HEAP_MEM_POOL_SIZE`
- **Timing**: Use `k_uptime_get()` for performance measurement
- **Power**: Consider power management for battery-operated devices

**Last Updated**: 2026-01-11
**Status**: Development environment stabilized and documented