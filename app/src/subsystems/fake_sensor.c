/*
 * Copyright (c) 2024 Zephyr Weather Station
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/math_extras.h>
#include <math.h>
#include <stdio.h>

/* Fake sensor device structure */
struct fake_sensor_data {
    float temperature_c;
    float humidity_percent;
    float pressure_pa;
    uint32_t sequence;
    uint32_t prng_state; /* Simple pseudo-random number generator state */
};

/* Fake sensor device instance */
static struct fake_sensor_data fake_sensor;

/* Simple pseudo-random number generator */
static uint32_t simple_prng(uint32_t *state)
{
    *state = *state * 1103515245U + 12345U;
    return *state;
}

/* Initialize fake sensor with realistic values */
static int fake_sensor_init(const struct device *dev)
{
    ARG_UNUSED(dev);

    /* Set initial values */
    fake_sensor.temperature_c = 22.5f;  /* Room temperature */
    fake_sensor.humidity_percent = 45.0f; /* Comfortable humidity */
    fake_sensor.pressure_pa = 101325.0f; /* Standard atmospheric pressure */
    fake_sensor.sequence = 0;
    fake_sensor.prng_state = 42; /* Seed for PRNG */

    printk("Fake sensor initialized: T=%.1f°C, H=%.1f%%, P=%.1f Pa\n",
           fake_sensor.temperature_c, fake_sensor.humidity_percent,
           fake_sensor.pressure_pa);

    return 0;
}

/* Simulate sensor reading with small variations */
static int fake_sensor_sample_fetch(const struct device *dev, enum sensor_channel chan)
{
    /* Add small random variations to simulate real sensor behavior */
    float temp_variation = ((float)simple_prng(&fake_sensor.prng_state) / (float)4294967295U) * 2.0f - 1.0f;
    float humidity_variation = ((float)simple_prng(&fake_sensor.prng_state) / (float)4294967295U) * 5.0f - 2.5f;
    float pressure_variation = ((float)simple_prng(&fake_sensor.prng_state) / (float)4294967295U) * 200.0f - 100.0f;

    /* Apply variations with bounds checking */
    fake_sensor.temperature_c = fmaxf(15.0f, fminf(30.0f,
        fake_sensor.temperature_c + temp_variation * 0.1f));
    fake_sensor.humidity_percent = fmaxf(20.0f, fminf(80.0f,
        fake_sensor.humidity_percent + humidity_variation * 0.1f));
    fake_sensor.pressure_pa = fmaxf(95000.0f, fminf(105000.0f,
        fake_sensor.pressure_pa + pressure_variation));

    fake_sensor.sequence++;

    printk("Fake sensor sampled: T=%.1f°C, H=%.1f%%, P=%.1f Pa (seq=%u)\n",
           fake_sensor.temperature_c, fake_sensor.humidity_percent,
           fake_sensor.pressure_pa, fake_sensor.sequence);

    return 0;
}

/* Get sensor value for specific channel */
static int fake_sensor_channel_get(const struct device *dev, enum sensor_channel chan,
                                 struct sensor_value *val)
{
    switch (chan) {
        case SENSOR_CHAN_AMBIENT_TEMP:
            sensor_value_from_double(val, fake_sensor.temperature_c);
            break;
        case SENSOR_CHAN_HUMIDITY:
            sensor_value_from_double(val, fake_sensor.humidity_percent);
            break;
        case SENSOR_CHAN_PRESS:
            sensor_value_from_double(val, fake_sensor.pressure_pa);
            break;
        default:
            return -ENOTSUP;
    }

    return 0;
}

/* Fake sensor driver API */
static const struct sensor_driver_api fake_sensor_api = {
    .sample_fetch = fake_sensor_sample_fetch,
    .channel_get = fake_sensor_channel_get,
};

/* Fake sensor device definition */
DEVICE_DEFINE(fake_sensor, "FAKE_SENSOR", fake_sensor_init, NULL, NULL, NULL,
             POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY, &fake_sensor_api);

/* Public API to get fake sensor data */
int fake_sensor_get_readings(float *temperature, float *humidity, float *pressure)
{
    if (!temperature || !humidity || !pressure) {
        return -EINVAL;
    }

    *temperature = fake_sensor.temperature_c;
    *humidity = fake_sensor.humidity_percent;
    *pressure = fake_sensor.pressure_pa;

    return 0;
}