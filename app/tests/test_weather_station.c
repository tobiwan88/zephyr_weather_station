/*
 * Copyright (c) 2024 Zephyr Weather Station
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>
#include <zephyr/zbus/zbus.h>
#include "../src/common/messages.h"

static void test_message_structures(void)
{
    struct trigger_msg trigger = {
        .source = TRIGGER_MANUAL,
        .sequence = 42
    };

    struct sensor_data_msg sensor_data = {
        .timestamp = 123456,
        .temperature_c = 22.5f,
        .humidity_percent = 45.0f,
        .pressure_pa = 101325.0f,
        .source_flags = SENSOR_SOURCE_INTERNAL,
        .sequence = 1,
        .status = 0
    };

    zassert_equal(trigger.source, TRIGGER_MANUAL, "Trigger source should be manual");
    zassert_equal(trigger.sequence, 42, "Trigger sequence should be 42");

    zassert_equal(sensor_data.temperature_c, 22.5f, "Temperature should be 22.5°C");
    zassert_equal(sensor_data.humidity_percent, 45.0f, "Humidity should be 45.0%");
    zassert_equal(sensor_data.pressure_pa, 101325.0f, "Pressure should be 101325.0 Pa");
    zassert_true(sensor_data.source_flags & SENSOR_SOURCE_INTERNAL, "Should have internal source flag");
    zassert_equal(sensor_data.sequence, 1, "Sequence should be 1");
    zassert_equal(sensor_data.status, 0, "Status should be 0");
}

ZTEST(weather_station, test_message_structures)
{
    test_message_structures();
}

ZTEST_SUITE(weather_station, NULL, NULL, NULL, NULL, NULL);