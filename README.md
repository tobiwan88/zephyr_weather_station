# zephyr_weather_station

A test to generate a zephyr weather station firmware just using AI

## Project Structure

This is a Zephyr T1 project workspace managed by west. The application code is in the `app/` directory.

## Build Instructions

### Prerequisites

- west tool installed
- Zephyr SDK or Docker environment

### Initialize the workspace

```bash
west init -l .
west update
```

### Build for native_sim

```bash
cd app
west build -b native_sim
```

### Run on native_sim

```bash
west build -t run
```

## Docker Setup

Build and use the Docker environment:

```bash
docker build -t zephyr_weather_station .
docker run -it -v $(pwd):/workspace zephyr_weather_station
```

Inside the container:

```bash
west init -l .
west update
cd app
west build -b native_sim
west build -t run
```
