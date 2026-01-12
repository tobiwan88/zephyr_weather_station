# Zephyr Coding Guidelines for Cline AI Assistant

## Quick Reference for AI Development

**CRITICAL**: Cline must read and apply ALL rules in `.cline_rules/basic.md` first, then use these coding guidelines for implementation details.

## 1. File Operations (Cline-Specific)

- **Always use `read_file`** before editing any file to understand current structure
- **Use `replace_in_file`** with 3-5 lines of context before/after changes
- **Check file after edits** with `read_file` to verify successful changes
- **Never assume file contents** - always verify current state first

## 2. Zephyr Project Structure for AI

```
zephyr_weather_station/app/
├── CMakeLists.txt          # Build configuration
├── Kconfig                 # Application config symbols
├── prj.conf               # Project configuration
├── src/                   # Source code
└── boards/                # Board-specific configs (optional)
```

## 3. C Coding Style (AI Implementation)

### Indentation & Formatting
- **Tabs**: Use 8-character tabs (not spaces) for indentation
- **Braces**: K&R style - opening brace on same line for control structures
- **Line Length**: Maximum 100 characters per line
- **Functions**: Opening brace on new line for function definitions

```c
// Correct control structure
if (condition) {
    statement;
}

// Correct function definition
int weather_station_init(void)
{
    return 0;
}
```

### Naming Conventions
- **Functions/Variables**: `lower_case_with_underscores`
- **Macros/Constants**: `UPPER_CASE_WITH_UNDERSCORES`
- **Application Prefix**: `weather_station_` for all global symbols
- **Booleans**: Use `bool` type from `<stdbool.h>`

```c
// Correct naming
static int weather_station_sensor_read(void);
static bool weather_station_is_ready = false;
#define WEATHER_STATION_MAX_READINGS 100
```

## 4. Zephyr-Specific Requirements

### Headers & Includes
```c
// Required order:
#include <zephyr/kernel.h>           // Always first
#include <zephyr/device.h>           // Other Zephyr headers
#include <zephyr/drivers/sensor.h>
#include "weather_station.h"         // Local headers last
```

### Data Types
- **Use fixed-width types**: `uint32_t`, `int16_t`, etc. (never `int`, `long`)
- **Error codes**: Return negative errno values (`-EINVAL`, `-ENOMEM`)
- **Device references**: Use `const struct device *` for devices

### Configuration
- **Conditional compilation**: Use `if (IS_ENABLED(CONFIG_FOO))` inside functions
- **Avoid `#ifdef`** inside function bodies when possible
- **Kconfig**: All features need CONFIG symbols defined in `app/Kconfig`

```c
// Correct conditional code
int weather_station_init(void)
{
    if (IS_ENABLED(CONFIG_WEATHER_STATION_FAKE_SENSOR)) {
        return fake_sensor_init();
    }
    return real_sensor_init();
}
```

## 5. Memory Management

- **NO dynamic allocation**: Never use `malloc()`, `free()`, `realloc()`
- **Static allocation preferred**: Use static variables and arrays
- **Stack allocation**: Use local variables for temporary data
- **Zephyr APIs**: Use `k_mem_slab` or `k_heap` if dynamic memory needed

## 6. Thread Safety & Concurrency

- **Document thread safety** in function comments
- **Use Zephyr synchronization**: `k_mutex`, `k_sem`, `k_msgq`
- **Atomic operations**: Use `atomic_*` functions for shared variables
- **No busy waiting**: Use `k_sleep()` or event-based waits

## 7. Error Handling Patterns

```c
// Standard error handling pattern
int weather_station_read_sensor(struct weather_data *data)
{
    if (!data) {
        return -EINVAL;
    }

    int ret = sensor_sample_fetch(sensor_dev);
    if (ret) {
        LOG_ERR("Sensor fetch failed: %d", ret);
        return ret;
    }

    return 0;
}
```

## 8. Documentation Requirements

### Function Documentation
```c
/**
 * @brief Initialize the weather station subsystem
 *
 * @param config Pointer to configuration structure
 * @return 0 on success, negative errno on failure
 *
 * @note This function is not thread-safe and should only be called during initialization
 */
int weather_station_init(const struct weather_config *config);
```

### File Headers
```c
/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (c) 2026 Weather Station Project
 */

/**
 * @file weather_station.c
 * @brief Weather station main implementation
 */
```

## 9. Cline Implementation Checklist

When implementing code, Cline must verify:

- [ ] Read current file structure with `read_file`
- [ ] Used 8-character tabs for indentation
- [ ] Applied proper naming conventions with `weather_station_` prefix
- [ ] Used fixed-width integer types (`uint32_t`, etc.)
- [ ] Included proper SPDX license header
- [ ] Added Doxygen documentation for public functions
- [ ] Used `IS_ENABLED()` for conditional compilation
- [ ] Returned proper Zephyr error codes
- [ ] No dynamic memory allocation used
- [ ] Added thread safety documentation if relevant
- [ ] Verified file contents after editing

## 10. Common Patterns for AI Implementation

### Application Main Function
```c
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(weather_station, CONFIG_WEATHER_STATION_LOG_LEVEL);

int main(void)
{
    LOG_INF("Weather Station starting");

    int ret = weather_station_init();
    if (ret) {
        LOG_ERR("Initialization failed: %d", ret);
        return ret;
    }

    while (1) {
        weather_station_process();
        k_sleep(K_SECONDS(1));
    }

    return 0;
}
```

### Kconfig Symbol Definition
```kconfig
config WEATHER_STATION_FAKE_SENSOR
    bool "Use fake sensor for testing"
    default y if BOARD_NATIVE_SIM
    help
      Enable fake sensor implementation for testing on simulator platforms.
```

**Remember**: Always follow the build commands and environment setup from `.cline_rules/basic.md` before implementing any code.