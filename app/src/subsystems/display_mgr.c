/*
 * Copyright (c) 2024 Zephyr Weather Station
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>
#include "messages.h"

LOG_MODULE_REGISTER(display_mgr, CONFIG_WEATHER_STATION_LOG_LEVEL);

static void display_mgr_sensor_data_handler(const struct zbus_channel *chan)
{
    const struct sensor_data_msg *msg = zbus_chan_const_msg(chan);

    // Log the incoming sensor data (for native_sim, this acts as display output)
    LOG_INF("Sensor Data Received:");
    LOG_INF("  Timestamp: %llu ms", msg->timestamp);
    LOG_INF("  Temperature: %.1f°C", msg->temperature_c);
    LOG_INF("  Humidity: %.1f%%", msg->humidity_percent);
    LOG_INF("  Pressure: %.1f Pa", msg->pressure_pa);
    LOG_INF("  Source: %s", (msg->source_flags & SENSOR_SOURCE_INTERNAL) ? "INTERNAL" : "EXTERNAL");
    LOG_INF("  Sequence: %u", msg->sequence);
    LOG_INF("  Status: %d", msg->status);
}

ZBUS_LISTENER_DEFINE(display_mgr_listener,
                    display_mgr_sensor_data_handler);

static int display_mgr_init(void)
{
    LOG_INF("Display manager initialized");
    return 0;
}

SYS_INIT(display_mgr_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);