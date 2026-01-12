# Zephyr Weather Station - Product Context

## 🎯 Why This Project Exists
The Zephyr Weather Station project serves multiple important purposes:

1. **Learning Platform**: Provides a practical example of Zephyr RTOS development patterns
2. **Development Template**: Serves as a foundation for real weather monitoring systems
3. **Testing Framework**: Enables simulation-based testing of embedded weather applications
4. **Demonstration Tool**: Showcases Zephyr RTOS capabilities in a real-world scenario

## 🌡️ Problems It Solves
- **Embedded Development Complexity**: Simplifies the learning curve for Zephyr RTOS
- **Hardware Dependency**: Allows development and testing without physical sensors
- **Integration Testing**: Provides a framework for testing sensor data processing
- **System Architecture**: Demonstrates proper subsystem organization and communication

## 🔧 How It Should Work

### System Architecture
```
┌───────────────────────────────────────────────────────┐
│                 Zephyr Weather Station                │
├───────────────────────────────────────────────────────┤
│                                                       │
│  ┌─────────────┐    ┌─────────────┐    ┌───────────┐  │
│  │  Fake       │    │  Sensor     │    │           │  │
│  │  Sensor     │───▶│  Manager    │───▶│  Display  │  │
│  │  (Simulated)│    │  (ZBUS)     │    │  Manager  │  │
│  └─────────────┘    └─────────────┘    │  (Logging) │  │
│                                      └───────────┘  │
│                                              ▲       │
│                                              │       │
│  ┌───────────────────────────────────────────┴───┐  │
│  │                   ZBUS Messaging               │  │
│  └───────────────────────────────────────────────┘  │
│                                                       │
│  ┌───────────────────────────────────────────────┐  │
│  │               Shell Interface               │  │
│  │  (User Interaction & Control)               │  │
│  └───────────────────────────────────────────────┘  │
│                                                       │
└───────────────────────────────────────────────────────┘
```

### Data Flow
1. **Sensor Simulation**: Fake sensor generates realistic weather data (temperature, humidity, pressure)
2. **ZBUS Messaging**: Sensor data is published to ZBUS channels for decoupled communication
3. **Data Processing**: Sensor manager receives triggers and coordinates data collection
4. **Display & Logging**: Processed data is displayed and logged for monitoring
5. **User Control**: Shell interface allows manual triggering and status checking

### Expected Behavior
- **Automatic Operation**: System should initialize and be ready for sensor readings
- **Manual Control**: Users can trigger sensor readings via shell commands
- **Data Display**: Latest sensor data should be accessible via shell
- **Status Monitoring**: System health and statistics should be available
- **Error Handling**: Graceful handling of errors with appropriate logging

## 🎯 User Experience Goals
- **Developer Experience**: Clean, well-documented code following Zephyr standards
- **Testing Experience**: Reliable simulation for development and debugging
- **Extensibility**: Easy to add new sensors or modify existing behavior
- **Maintainability**: Clear architecture with well-defined responsibilities

## 📋 Key Features
- **ZBUS Messaging**: Decoupled communication between subsystems
- **Sensor Simulation**: Realistic fake sensor with PRNG-based variations
- **Shell Interface**: Interactive control and monitoring
- **Logging**: Comprehensive system logging for debugging
- **Cross-Platform**: Works on native simulator and hardware targets

**Last Updated**: 2026-01-11
**Status**: Initial implementation complete, ready for testing