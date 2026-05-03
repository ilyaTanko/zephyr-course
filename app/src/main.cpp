#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>

#include "drivers/our_driver/our_driver.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

static constexpr bool led_debugging = IS_ENABLED(CONFIG_LED_DEBUGGING);
static constexpr bool led_advanced_settings = IS_ENABLED(CONFIG_LED_ADVANCED_SETTINGS);

static void log_led_settings() {
    if (led_debugging) {
        LOG_INF("LED subsystem enabled");
        LOG_INF("Blink interval: %d ms", CONFIG_APP_HEARTBEAT_PERIOD_MS);
    }

    if (led_advanced_settings) {
        LOG_INF("LED brightness: %d%%", CONFIG_LED_BRIGHTNESS);
        LOG_INF("LED fade duration: %d ms", CONFIG_LED_FADE_DURATION_MS);
    }
}

int main() {
    const struct device* dev = DEVICE_DT_GET_ANY(our_driver);
    if (!device_is_ready(dev)) {
        LOG_ERR("Our driver not ready");
        return -ENODEV;
    }

    log_led_settings();

    while (true) {
        auto ret = sensor_sample_fetch(dev);
        LOG_INF("Sensor sample fetch result: %d", ret);
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);

        struct sensor_value val;
        ret = sensor_channel_get(dev, SENSOR_CHAN_ALL, &val);
        LOG_INF("Sensor channel get result: %d", ret);
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);

        uint32_t counter = our_driver_increment(dev);
        LOG_INF("Our driver increment result: %u", counter);
    }

    return 0;
}
