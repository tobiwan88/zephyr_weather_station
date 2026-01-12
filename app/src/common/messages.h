/*
 * Copyright (c) 2024 Zephyr Weather Station
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef WEATHER_STATION_MESSAGES_H
#define WEATHER_STATION_MESSAGES_H

#include <zephyr/zbus/zbus.h>
#include <stdint.h>
#include <stddef.h>

/* Message definitions for zbus communication */

/* Trigger message - request sensor update */
struct trigger_msg {
    enum trigger_source {
        TRIGGER_MANUAL,     /* Shell command */
        TRIGGER_TIMER,      /* Periodic update */
        TRIGGER_EXTERNAL    /* External request */
    } source;
    uint32_t sequence;      /* Request sequence number */
};

/* Sensor data message - publish sensor readings */
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

/* Zbus channel declarations */
ZBUS_CHAN_DECLARE(ws_trigger);
ZBUS_CHAN_DECLARE(ws_sensor_data);

#endif /* WEATHER_STATION_MESSAGES_H */