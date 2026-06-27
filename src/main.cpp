#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(sensor_logger, LOG_LEVEL_DBG);

// ── Message Queue ──────────────────────────────────────────────
#define QUEUE_SIZE 10
K_MSGQ_DEFINE(sensor_msgq, sizeof(int32_t), QUEUE_SIZE, 4);

#define STACK_SIZE 1024

// ── Simple LCG random number generator ────────────────────────
static uint32_t lcg_state = 42;
static int32_t lcg_rand()
{
    lcg_state = lcg_state * 1664525u + 1013904223u;
    return (int32_t)(lcg_state % 3) - 1; // returns -1, 0, or +1
}

// ── Simulated sensor ──────────────────────────────────────────
static int32_t read_temperature()
{
    static int32_t temp = 25;
    temp += lcg_rand();
    if (temp > 30) temp = 30;
    if (temp < 20) temp = 20;
    return temp;
}

// ── Sensor thread ─────────────────────────────────────────────
void sensor_thread(void *a, void *b, void *c)
{
    while (1) {
        int32_t temp = read_temperature();
        if (k_msgq_put(&sensor_msgq, &temp, K_NO_WAIT) != 0) {
            LOG_WRN("Queue full, dropping sample");
        }
        k_sleep(K_MSEC(500));
    }
}

// ── Logger thread ─────────────────────────────────────────────
void logger_thread(void *a, void *b, void *c)
{
    int32_t temp;
    while (1) {
        if (k_msgq_get(&sensor_msgq, &temp, K_MSEC(1000)) == 0) {
            LOG_INF("Temperature: %d C", temp);
        } else {
            LOG_WRN("No data received in time");
        }
    }
}

// ── Watchdog thread ───────────────────────────────────────────
void watchdog_thread(void *a, void *b, void *c)
{
    while (1) {
        LOG_DBG("System alive - uptime: %lld ms", k_uptime_get());
        k_sleep(K_MSEC(2000));
    }
}

// ── Thread definitions ────────────────────────────────────────
K_THREAD_DEFINE(sensor_tid,   STACK_SIZE, sensor_thread,   NULL, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(logger_tid,   STACK_SIZE, logger_thread,   NULL, NULL, NULL, 6, 0, 0);
K_THREAD_DEFINE(watchdog_tid, STACK_SIZE, watchdog_thread, NULL, NULL, NULL, 7, 0, 0);
