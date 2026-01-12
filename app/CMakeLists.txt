# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr)
project(weather_station)

target_sources(app PRIVATE
    src/main.c
    src/subsystems/fake_sensor.c
    src/subsystems/sensor_mgr.c
    src/subsystems/display_mgr.c
    src/subsystems/shell_iface.c
)

target_include_directories(app PRIVATE
    src/common
)