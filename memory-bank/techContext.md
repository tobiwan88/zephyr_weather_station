# Zephyr Weather Station - Technical Context

## 🛠️ Technologies Used

### Core Technologies
- **Zephyr RTOS 4.3.0**: Real-time operating system foundation
- **West Build System**: Zephyr's meta-tool for building and flashing
- **C Language**: Primary development language (C11 standard)
- **CMake + Ninja**: Build system configuration and execution

### Zephyr Subsystems
- **ZBUS**: Message bus for inter-thread communication
- **Sensor Framework**: Standardized sensor driver interface
- **Shell**: Interactive command interface
- **Logging**: System logging infrastructure
- **Device Tree**: Hardware description and configuration

## 💻 Development Setup

### Build Environment
- **Container**: Docker-based development environment
- **Python 3.10+**: For West and build tools
- **Toolchain**: Zephyr SDK with GCC cross-compilation
- **Target**: native_sim/native/64 (POSIX-based simulator)

### Essential Commands
```bash
# Environment setup
source /home/zephyr/.venv/bin/activate
source /workspace/zephyr/zephyr-env.sh

# Build and run
west build zephyr_weather_station/app -b native_sim/native/64 --pristine
west build -d build run
```

## ⚙️ Technical Constraints

### Platform Constraints
- **Memory**: Native simulator memory limitations
- **Performance**: Real-time sensor data processing requirements
- **Compatibility**: Multi-board Zephyr target support
- **Footprint**: Minimal resource usage for embedded deployment

### Development Constraints
- **Build System**: West/CMake/Ninja workflow required
- **Coding Standards**: Zephyr conventions mandatory
- **Testing**: Both simulation and hardware validation required

## 📦 Dependencies

### External Dependencies
- **Zephyr RTOS 4.3.0**: Core operating system
- **Zephyr SDK**: Toolchain and development tools
- **Python 3.10+**: Build system and tools
- **CMake 3.20+**: Build configuration

### Internal Dependencies
- **ZBUS**: Inter-component communication
- **Sensor Framework**: Sensor driver interface
- **Shell**: User interaction
- **Logging**: System diagnostics

## 🔧 Tool Usage Patterns

### Build Patterns
```bash
# Clean build
west build -b native_sim/native/64 --pristine

# Incremental build
west build -b native_sim/native/64
```

### Debug Patterns
```bash
# Debug run
west build -d build run -- -v

# Hardware flash
west flash
```