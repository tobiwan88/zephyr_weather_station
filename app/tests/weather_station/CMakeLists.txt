# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr COMPONENTS unittest REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(weather_station_tests)

target_sources(testbinary
  PRIVATE
    main.c
    test_display_mgr.c
    test_sensor_mgr.c
    test_weather_station.c
)