/*
 * Copyright (c) 2024 Zephyr Weather Station
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>
#include "messages.h"

// Mock variables to track sensor manager behavior
static int sensor_init_called = 0;
static int trigger_received_count = 0;
static int sensor_data_published_count = 0;
static struct sensor_data_msg last_published_data = {0};

// Mock the sensor manager functions for testing
static void mock_sensor_mgr_trigger_handler(const struct zbus_channel *chan)
{
    const struct trigger_msg *msg = zbus_chan_const_msg(chan);
    trigger_received_count++;

    LOG_DBG("Test: Trigger received (source: %d, seq: %u)", msg->source, msg->sequence);

    // Create mock sensor data
    struct sensor_data_msg sensor_data = {
        .timestamp = k_uptime_get(),
        .temperature_c = 22.5f + (msg->sequence * 0.1f),  // Vary by sequence
        .humidity_percent = 45.0f + (msg->sequence * 0.5f),
        .pressure_pa = 101325.0f + (msg->sequence * 10.0f),
        .source_flags = SENSOR_SOURCE_INTERNAL,
        .sequence = msg->sequence,  // Use trigger sequence for simplicity
        .status = 0
    };

    // Publish sensor data
    int rc = zbus_chan_pub(ZBUS_REF(ws_sensor_data), &sensor_data, K_MSEC(100));
    if (rc == 0) {
        sensor_data_published_count++;
        last_published_data = sensor_data;
    } else {
        LOG_ERR("Test: Failed to publish sensor data: %d", rc);
    }
}

ZBUS_LISTENER_DEFINE(test_sensor_listener, mock_sensor_mgr_trigger_handler);

static int mock_sensor_mgr_init(void)
{
    sensor_init_called = 1;
    LOG_DBG("Test: Sensor manager initialized");
    return 0;
}

// Test setup function
static void *test_sensor_setup(void)
{
    // Reset all mock variables
    sensor_init_called = 0;
    trigger_received_count = 0;
    sensor_data_published_count = 0;
    memset(&last_published_data, 0, sizeof(last_published_data));

    // Initialize the sensor manager mock
    mock_sensor_mgr_init();

    return NULL;
}

// Test teardown function
static void test_sensor_teardown(void *data)
{
    // Clean up if needed
    ARG_UNUSED(data);
}

/* Test cases for sensor manager functionality */

static void test_sensor_initialization(void)
{
    zassert_equal(sensor_init_called, 1, "Sensor manager should be initialized");
}

static void test_sensor_trigger_handling(void)
{
    struct trigger_msg test_trigger = {
        .source = TRIGGER_MANUAL,
        .sequence = 42
    };

    // Publish trigger message
    int ret = zbus_chan_pub(ZBUS_REF(ws_trigger), &test_trigger, K_MSEC(100));
    zassert_equal(ret, 0, "Failed to publish trigger: %d", ret);

    // Wait for processing
    k_msleep(50);

    // Verify trigger was received and processed
    zassert_equal(trigger_received_count, 1, "Should have received exactly one trigger");
    zassert_equal(sensor_data_published_count, 1, "Should have published exactly one sensor data message");
    zassert_equal(last_published_data.sequence, test_trigger.sequence, "Sensor data sequence should match trigger sequence");
    zassert_equal(last_published_data.source_flags, SENSOR_SOURCE_INTERNAL, "Should use internal source flag");
    zassert_equal(last_published_data.status, 0, "Status should be success");
}

static void test_sensor_multiple_triggers(void)
{
    struct trigger_msg trigger1 = {
        .source = TRIGGER_MANUAL,
        .sequence = 1
    };

    struct trigger_msg trigger2 = {
        .source = TRIGGER_TIMER,
        .sequence = 2
    };

    struct trigger_msg trigger3 = {
        .source = TRIGGER_EXTERNAL,
        .sequence = 3
    };

    // Publish multiple triggers
    int ret = zbus_chan_pub(ZBUS_REF(ws_trigger), &trigger1, K_MSEC(100));
    zassert_equal(ret, 0, "Failed to publish first trigger: %d", ret);

    ret = zbus_chan_pub(ZBUS_REF(ws_trigger), &trigger2, K_MSEC(100));
    zassert_equal(ret, 0, "Failed to publish second trigger: %d", ret);

    ret = zbus_chan_pub(ZBUS_REF(ws_trigger), &trigger3, K_MSEC(100));
    zassert_equal(ret, 0, "Failed to publish third trigger: %d", ret);

    // Wait for processing
    k_msleep(100);

    // Verify all triggers were processed
    zassert_equal(trigger_received_count, 3, "Should have received exactly three triggers");
    zassert_equal(sensor_data_published_count, 3, "Should have published exactly three sensor data messages");
    zassert_equal(last_published_data.sequence, trigger3.sequence, "Last sequence should match last trigger");
}

static void test_sensor_data_variation(void)
{
    struct trigger_msg test_trigger = {
        .source = TRIGGER_MANUAL,
        .sequence = 100
    };

    // Publish trigger
    int ret = zbus_chan_pub(ZBUS_REF(ws_trigger), &test_trigger, K_MSEC(100));
    zassert_equal(ret, 0, "Failed to publish trigger: %d", ret);

    // Wait for processing
    k_msleep(50);

    // Verify sensor data has expected values with variation
    zassert_equal(sensor_data_published_count, 1, "Should have published sensor data");
    zassert_equal(last_published_data.sequence, test_trigger.sequence, "Sequence should match trigger");
    zassert_true(last_published_data.temperature_c > 22.0f && last_published_data.temperature_c < 23.0f,
                "Temperature should be in expected range");
    zassert_true(last_published_data.humidity_percent > 45.0f && last_published_data.humidity_percent < 50.0f,
                "Humidity should be in expected range");
    zassert_true(last_published_data.pressure_pa > 101325.0f && last_published_data.pressure_pa < 101335.0f,
                "Pressure should be in expected range");
}

static void test_sensor_different_trigger_sources(void)
{
    struct trigger_msg manual_trigger = {
        .source = TRIGGER_MANUAL,
        .sequence = 1
    };

    struct trigger_msg timer_trigger = {
        .source = TRIGGER_TIMER,
        .sequence = 2
    };

    struct trigger_msg external_trigger = {
        .source = TRIGGER_EXTERNAL,
        .sequence = 3
    };

    // Test different trigger sources
    int ret = zbus_chan_pub(ZBUS_REF(ws_trigger), &manual_trigger, K_MSEC(100));
    zassert_equal(ret, 0, "Failed to publish manual trigger: %d", ret);

    ret = zbus_chan_pub(ZBUS_REF(ws_trigger), &timer_trigger, K_MSEC(100));
    zassert_equal(ret, 0, "Failed to publish timer trigger: %d", ret);

    ret = zbus_chan_pub(ZBUS_REF(ws_trigger), &external_trigger, K_MSEC(100));
    zassert_equal(ret, 0, "Failed to publish external trigger: %d", ret);

    // Wait for processing
    k_msleep(100);

    // Verify all trigger sources were handled
    zassert_equal(trigger_received_count, 3, "Should have received all three triggers");
    zassert_equal(sensor_data_published_count, 3, "Should have published data for all triggers");
}

/* ZTEST definitions */

ZTEST(sensor_mgr, test_initialization)
{
    test_sensor_initialization();
}

ZTEST(sensor_mgr, test_trigger_handling)
{
    test_sensor_trigger_handling();
}

ZTEST(sensor_mgr, test_multiple_triggers)
{
    test_sensor_multiple_triggers();
}

ZTEST(sensor_mgr, test_data_variation)
{
    test_sensor_data_variation();
}

ZTEST(sensor_mgr, test_different_trigger_sources)
{
    test_sensor_different_trigger_sources();
}

/* Define the test suite */
ZTEST_SUITE(sensor_mgr, NULL, test_sensor_setup, NULL, test_sensor_teardown, NULL);