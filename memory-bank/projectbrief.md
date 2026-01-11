# Zephyr Weather Station - Project Brief

## 🎯 Project Overview
The Zephyr Weather Station is an embedded application built on the Zephyr RTOS platform that simulates a weather monitoring system. It's designed to run on various hardware platforms but is currently being developed and tested using the native_sim/native/64 simulator target.

## 🚀 Core Objectives
1. **Build a functional Zephyr RTOS application** that demonstrates real-world embedded system patterns
2. **Implement a weather monitoring system** with sensor data collection and processing
3. **Provide interactive control** through a shell interface for testing and demonstration
4. **Showcase Zephyr RTOS capabilities** including ZBUS messaging, device drivers, and multi-threaded architecture

## 🔧 Technical Stack
- **RTOS**: Zephyr RTOS 4.3.0
- **Build System**: West (Zephyr's meta-tool)
- **Target Platform**: native_sim/native/64 (simulator for development)
- **Programming Language**: C (following Zephyr coding standards)
- **Key Subsystems**: ZBUS, Sensor Framework, Shell, Logging

## 📦 Project Structure
```
zephyr_weather_station/
├── app/
│   ├── CMakeLists.txt          # Build configuration
│   ├── Kconfig                 # Application config symbols
│   ├── prj.conf                # Project configuration
│   ├── src/                    # Source code
│   │   ├── main.c              # Main application entry
│   │   ├── common/             # Shared components
│   │   └── subsystems/         # Functional modules
│   └── tests/                  # Unit tests
└── memory-bank/                # Project memory and context
```

## 🎯 Current Development Focus
The project is currently in the **initial development phase** with the following priorities:
1. **Build System Stabilization** - Ensure reliable compilation and linking
2. **Core Architecture** - Establish ZBUS messaging patterns and subsystem integration
3. **Sensor Simulation** - Implement realistic fake sensor behavior for testing
4. **Shell Interface** - Provide interactive control and monitoring

## 📋 Key Requirements
- Must build successfully with `west build -b native_sim/native/64`
- Must follow Zephyr RTOS coding standards and patterns
- Must be maintainable and well-documented
- Must support future hardware integration

## 🔮 Future Roadmap
1. **Phase 1 (Current)**: Build stabilization and core functionality
2. **Phase 2**: Hardware sensor integration
3. **Phase 3**: Data logging and storage
4. **Phase 4**: Network connectivity and cloud integration

**Last Updated**: 2026-01-11
**Status**: Active Development