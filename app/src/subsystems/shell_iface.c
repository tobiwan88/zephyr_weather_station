/*
 * Copyright (c) 2024 Zephyr Weather Station
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>
#include <stdlib.h>
#include "messages.h"

LOG_MODULE_REGISTER(shell_iface, CONFIG_WEATHER_STATION_LOG_LEVEL);

static uint32_t trigger_sequence = 0;
static struct sensor_data_msg last_sensor_data = {0};
static bool has_sensor_data = false;

static int cmd_trigger(const struct shell *shell, size_t argc, char **argv)
{
    if (argc > 1) {
        shell_error(shell, "Usage: ws trigger");
        return -EINVAL;
    }

    struct trigger_msg trigger = {
        .source = TRIGGER_MANUAL,
        .sequence = trigger_sequence++
    };

    int rc = zbus_chan_pub(&ZBUS_CHAN_GET(ws_trigger), &trigger, K_SECONDS(1));
    if (rc != 0) {
        shell_error(shell, "Failed to publish trigger: %d", rc);
        return rc;
    }

    shell_print(shell, "Trigger sent (source: manual, seq: %u)", trigger.sequence);
    return 0;
}

static int cmd_show(const struct shell *shell, size_t argc, char **argv)
{
    if (argc > 1) {
        shell_error(shell, "Usage: ws show");
        return -EINVAL;
    }

    if (!has_sensor_data) {
        shell_error(shell, "No sensor data available");
        return -ENODATA;
    }

    shell_print(shell, "Latest Sensor Data:");
    shell_print(shell, "  Timestamp: %llu ms", last_sensor_data.timestamp);
    shell_print(shell, "  Temperature: %.1f°C", last_sensor_data.temperature_c);
    shell_print(shell, "  Humidity: %.1f%%", last_sensor_data.humidity_percent);
    shell_print(shell, "  Pressure: %.1f Pa", last_sensor_data.pressure_pa);
    shell_print(shell, "  Source: %s", (last_sensor_data.source_flags & SENSOR_SOURCE_INTERNAL) ? "INTERNAL" : "EXTERNAL");
    shell_print(shell, "  Sequence: %u", last_sensor_data.sequence);
    shell_print(shell, "  Status: %d", last_sensor_data.status);

    return 0;
}

static int cmd_status(const struct shell *shell, size_t argc, char **argv)
{
    if (argc > 1) {
        shell_error(shell, "Usage: ws status");
        return -EINVAL;
    }

    shell_print(shell, "Weather Station Status:");
    shell_print(shell, "  Sensor Data Available: %s", has_sensor_data ? "YES" : "NO");
    shell_print(shell, "  Last Trigger Sequence: %u", trigger_sequence);
    shell_print(shell, "  System Uptime: %llu ms", k_uptime_get());

    return 0;
}

static void shell_iface_sensor_data_handler(const struct zbus_channel *chan)
{
    const struct sensor_data_msg *msg = zbus_chan_const_msg(chan);
    last_sensor_data = *msg;
    has_sensor_data = true;
}

ZBUS_LISTENER_DEFINE(shell_iface_listener,
                    shell_iface_sensor_data_handler);

SHELL_STATIC_SUBCMD_SET_CREATE(
    ws_subcommands,
    SHELL_CMD(trigger, NULL, "Request immediate sensor reading", cmd_trigger),
    SHELL_CMD(show, NULL, "Display latest sensor data", cmd_show),
    SHELL_CMD(status, NULL, "Show subsystem health and statistics", cmd_status),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(ws, &ws_subcommands, "Weather Station commands", NULL);