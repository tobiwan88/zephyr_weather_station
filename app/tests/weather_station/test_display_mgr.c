/*
 * Copyright (c) 2024 Zephyr Weather Station
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>
#include "messages.h"

// Mock variables to track display manager behavior
static int display_init_called = 0;
static int sensor_data_received_count = 0;
static struct sensor_data_msg last_received_data = {0};

// Mock the display manager functions for testing
static void mock_display_mgr_sensor_data_handler(const struct zbus_channel *chan)
{
    const struct sensor_data_msg *msg = zbus_chan_const_msg(chan);
    sensor_data_received_count++;
    last_received_data = *msg;

    // In tests, we'll verify the data rather than logging it
    LOG_DBG("Test: Sensor data received (seq: %u, temp: %.1f°C)",
           msg->sequence, msg->temperature_c);
}

ZBUS_LISTENER_DEFINE(test_display_listener, mock_display_mgr_sensor_data_handler);

static int mock_display_mgr_init(void)
{
    display_init_called = 1;
    LOG_DBG("Test: Display manager initialized");
    return 0;
}

// Test setup function
static void *test_display_setup(void)
{
    // Reset all mock variables
    display_init_called = 0;
    sensor_data_received_count = 0;
    memset(&last_received_data, 0, sizeof(last_received_data));

    // Initialize the display manager mock
    mock_display_mgr_init();

    return NULL;
}

// Test teardown function
static void test_display_teardown(void *data)
{
    // Clean up if needed
    ARG_UNUSED(data);
}

/* Test cases for display manager functionality */

static void test_display_initialization(void)
{
    zassert_equal(display_init_called, 1, "Display manager should be initialized");
}

static void test_display_sensor_data_reception(void)
{
    struct sensor_data_msg test_data = {
        .timestamp = 123456,
        .temperature_c = 22.5f,
        .humidity_percent = 45.0f,
        .pressure_pa = 101325.0f,
        .source_flags = SENSOR_SOURCE_INTERNAL,
        .sequence = 1,
        .status = 0
    };

    // Publish test data to the sensor data channel
    int ret = zbus_chan_pub(ZBUS_REF(ws_sensor_data), &test_data, K_MSEC(100));
    zassert_equal(ret, 0, "Failed to publish sensor data: %d", ret);

    // Wait a bit for the listener to process
    k_msleep(50);

    // Verify the data was received and processed
    zassert_equal(sensor_data_received_count, 1, "Should have received exactly one sensor data message");
    zassert_equal(last_received_data.sequence, test_data.sequence, "Sequence number should match");
    zassert_equal(last_received_data.temperature_c, test_data.temperature_c, "Temperature should match");
    zassert_equal(last_received_data.humidity_percent, test_data.humidity_percent, "Humidity should match");
    zassert_equal(last_received_data.pressure_pa, test_data.pressure_pa, "Pressure should match");
    zassert_equal(last_received_data.source_flags, test_data.source_flags, "Source flags should match");
    zassert_equal(last_received_data.status, test_data.status, "Status should match");
}

static void test_display_multiple_data_messages(void)
{
    struct sensor_data_msg test_data1 = {
        .timestamp = 1000,
        .temperature_c = 20.0f,
        .humidity_percent = 50.0f,
        .pressure_pa = 101000.0f,
        .source_flags = SENSOR_SOURCE_INTERNAL,
        .sequence = 1,
        .status = 0
    };

    struct sensor_data_msg test_data2 = {
        .timestamp = 2000,
        .temperature_c = 25.0f,
        .humidity_percent = 40.0f,
        .pressure_pa = 102000.0f,
        .source_flags = SENSOR_SOURCE_EXTERNAL,
        .sequence = 2,
        .status = 0
    };

    // Publish first message
    int ret = zbus_chan_pub(ZBUS_REF(ws_sensor_data), &test_data1, K_MSEC(100));
    zassert_equal(ret, 0, "Failed to publish first sensor data: %d", ret);

    // Publish second message
    ret = zbus_chan_pub(ZBUS_REF(ws_sensor_data), &test_data2, K_MSEC(100));
    zassert_equal(ret, 0, "Failed to publish second sensor data: %d", ret);

    // Wait for processing
    k_msleep(50);

    // Verify both messages were received
    zassert_equal(sensor_data_received_count, 2, "Should have received exactly two sensor data messages");
    zassert_equal(last_received_data.sequence, test_data2.sequence, "Last sequence should be from second message");
    zassert_equal(last_received_data.temperature_c, test_data2.temperature_c, "Last temperature should be from second message");
}

static void test_display_error_status_handling(void)
{
    struct sensor_data_msg test_data = {
        .timestamp = 9999,
        .temperature_c = 0.0f,
        .humidity_percent = 0.0f,
        .pressure_pa = 0.0f,
        .source_flags = SENSOR_SOURCE_INTERNAL,
        .sequence = 100,
        .status = -EINVAL  // Error status
    };

    // Publish error data
    int ret = zbus_chan_pub(ZBUS_REF(ws_sensor_data), &test_data, K_MSEC(100));
    zassert_equal(ret, 0, "Failed to publish error sensor data: %d", ret);

    // Wait for processing
    k_msleep(50);

    // Verify error data was received and processed
    zassert_equal(sensor_data_received_count, 1, "Should have received error data message");
    zassert_equal(last_received_data.status, -EINVAL, "Error status should be preserved");
    zassert_equal(last_received_data.sequence, test_data.sequence, "Sequence should match even for error data");
}

/* ZTEST definitions */

ZTEST(display_mgr, test_initialization)
{
    test_display_initialization();
}

ZTEST(display_mgr, test_sensor_data_reception)
{
    test_display_sensor_data_reception();
}

ZTEST(display_mgr, test_multiple_data_messages)
{
    test_display_multiple_data_messages();
}

ZTEST(display_mgr, test_error_status_handling)
{
    test_display_error_status_handling();
}

/* Define the test suite */
ZTEST_SUITE(display_mgr, NULL, test_display_setup, NULL, test_display_teardown, NULL);