# Zephyr Multi-Sensor Logger

A multithreaded sensor data logger built with **Zephyr RTOS v3.7.0** in C++, running on the native POSIX simulator.

## What it does
- Reads simulated temperature data every 500ms
- Passes data between threads safely using a **message queue**
- Logs timestamped output using **Zephyr's logging subsystem**
- Runs a **watchdog heartbeat** thread every 2 seconds to confirm system health

## Architecture
sensor_thread (prio 5)

└── reads temperature → puts in message queue

│

▼

[K_MSGQ message queue]

│

▼

logger_thread (prio 6)

└── gets from queue → LOG_INF output
watchdog_thread (prio 7)

└── logs system uptime every 2 seconds

## Concepts demonstrated
- Zephyr RTOS thread creation with K_THREAD_DEFINE
- Inter-thread communication with K_MSGQ_DEFINE
- Zephyr logging subsystem (LOG_INF, LOG_WRN, LOG_DBG)
- Thread priorities and cooperative scheduling
- Zephyr build system (West + CMake)

## How to build and run

### Requirements
- Zephyr RTOS v3.7.0
- West v1.5.0+
- Zephyr SDK 0.16.8

### Build
```bash
west build -b native_posix .
```

### Run
```bash
./build/zephyr/zephyr.exe
```

### Expected output
*** Booting Zephyr OS build v3.7.0 ***

[00:00:00.000,000] <inf> sensor_logger: Temperature: 25 C

[00:00:00.000,000] <dbg> sensor_logger: watchdog_thread: System alive - uptime: 0 ms

[00:00:00.510,000] <inf> sensor_logger: Temperature: 26 C

[00:00:02.010,000] <dbg> sensor_logger: watchdog_thread: System alive - uptime: 2010 ms

## Environment
- Developed on Windows 11 with WSL2 (Ubuntu 24.04)
- Runs on native POSIX simulator (no hardware required)
- Portable to any Zephyr-supported board (STM32, nRF52840, ESP32)
