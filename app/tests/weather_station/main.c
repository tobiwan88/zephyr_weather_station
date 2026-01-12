/*
 * Copyright (c) 2024 Zephyr Weather Station
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>

// Include all test suites
#include "test_weather_station.c"
#include "test_display_mgr.c"
#include "test_sensor_mgr.c"

// Define all test suites (they will be automatically discovered by ztest)
ZTEST_SUITE(weather_station, NULL, NULL, NULL, NULL, NULL);
ZTEST_SUITE(display_mgr, NULL, NULL, NULL, NULL, NULL);
ZTEST_SUITE(sensor_mgr, NULL, NULL, NULL, NULL, NULL);