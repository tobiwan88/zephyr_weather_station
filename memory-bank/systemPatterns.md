# Zephyr Weather Station - System Patterns

## 🏗️ System Architecture

Event-driven modular design with three core layers:

```
┌─────────────────────────────────────────────────────────┐
│                  Application Layer                      │
├─────────────────────────────────────────────────────────┤
│  Shell Interface  │  Display Manager  │  Main App      │
└─────────────────────────────────────────────────────────┘
                              │
                         ZBUS Messaging
                              │
┌─────────────────────────────────────────────────────────┐
│                   Service Layer                         │
├─────────────────────────────────────────────────────────┤
│         Sensor Manager        │      Data Processor     │
└─────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────┐
│                  Hardware Layer                         │
├─────────────────────────────────────────────────────────┤
│  Fake Sensor Driver  │  Zephyr Sensor Framework        │
└─────────────────────────────────────────────────────────┘
```

**Core Principles:**
- Event-driven communication via ZBUS
- Modular subsystem isolation
- Standard Zephyr device model compliance
- Static memory allocation

## 🎯 Major Architecture Decisions

### 1. ZBUS for Inter-Component Communication
**Rationale**: Decouples components, enables extension, provides type-safe messaging

### 2. Fake Sensor with Simulation
**Rationale**: Hardware-independent development, easy migration to real hardware

### 3. Zephyr Shell Interface
**Rationale**: Standard debugging interface, follows Zephyr conventions

### 4. SYS_INIT Initialization Order
**Rationale**: Deterministic startup sequence, proper dependency management

### 5. Static Memory Management
**Rationale**: Predictable memory usage, real-time compatibility

## 🔧 Design Patterns

- **Event-Driven Messaging**: ZBUS channels for component communication
- **Device Driver Pattern**: Standard Zephyr sensor framework compliance
- **Observer Pattern**: ZBUS listeners for event handling
- **Command Pattern**: Shell interface for user interaction