/*
 * Copyright (c) 2024 Zephyr Weather Station
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>
#include "common/messages.h"

LOG_MODULE_REGISTER(main, CONFIG_WEATHER_STATION_LOG_LEVEL);

int main(void)
{
    LOG_INF("Weather Station starting...");

    // Initialize zbus channels
    int rc = zbus_init();
    if (rc != 0) {
        LOG_ERR("Failed to initialize zbus: %d", rc);
        return rc;
    }

    // Define zbus channels
    ZBUS_CHAN_DEFINE(ws_trigger,
                    struct trigger_msg,
                    NULL,
                    NULL,
                    ZBUS_OBSERVERS(sensor_mgr_listener),
                    ZBUS_MSG_INIT());

    ZBUS_CHAN_DEFINE(ws_sensor_data,
                    struct sensor_data_msg,
                    NULL,
                    NULL,
                    ZBUS_OBSERVERS(display_mgr_listener, shell_iface_listener),
                    ZBUS_MSG_INIT());

    LOG_INF("Weather Station initialized. Type 'ws trigger' to request sensor reading.");
    LOG_INF("Available commands: ws trigger, ws show, ws status");

    return 0;
}