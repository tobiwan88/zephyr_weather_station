/*
 * Copyright (c) 2024 Zephyr Weather Station
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>
#include "messages.h"

LOG_MODULE_REGISTER(sensor_mgr, CONFIG_WEATHER_STATION_LOG_LEVEL);

static uint32_t sensor_sequence = 0;

static void sensor_mgr_trigger_handler(const struct zbus_channel *chan)
{
    const struct trigger_msg *msg = zbus_chan_const_msg(chan);
    LOG_INF("Trigger received (source: %d, seq: %u)", msg->source, msg->sequence);

    // TODO: Read real sensors here
    struct sensor_data_msg sensor_data = {
        .timestamp = k_uptime_get(),
        .temperature_c = 22.5f,
        .humidity_percent = 45.0f,
        .pressure_pa = 101325.0f,
        .source_flags = SENSOR_SOURCE_INTERNAL,
        .sequence = sensor_sequence++,
        .status = 0
    };

    // Publish sensor data
    int rc = zbus_chan_pub(&ZBUS_CHAN_GET(ws_sensor_data), &sensor_data, K_SECONDS(2));
    if (rc != 0) {
        LOG_ERR("Failed to publish sensor data: %d", rc);
    }
}

ZBUS_LISTENER_DEFINE(sensor_mgr_listener,
                    sensor_mgr_trigger_handler);

static int sensor_mgr_init(void)
{
    LOG_INF("Sensor manager initialized");
    return 0;
}

SYS_INIT(sensor_mgr_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);