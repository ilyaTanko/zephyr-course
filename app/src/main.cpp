#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>

/* The devicetree node identifier for the "led0" alias. */
#define LED_NODE DT_ALIAS(led0)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

static constexpr bool led_debugging = IS_ENABLED(CONFIG_LED_DEBUGGING);
static constexpr bool led_advanced_settings = IS_ENABLED(CONFIG_LED_ADVANCED_SETTINGS);

static void log_led_settings() {
    if (led_debugging) {
        LOG_INF("LED subsystem enabled");
        LOG_INF("Blink interval: %d ms", CONFIG_BLINK_SLEEP_TIME_MS);
    }

    if (led_advanced_settings) {
        LOG_INF("LED brightness: %d%%", CONFIG_LED_BRIGHTNESS);
        LOG_INF("LED fade duration: %d ms", CONFIG_LED_FADE_DURATION_MS);
    }
}

int main() {
    bool led_state = true;

    log_led_settings();

    if (!gpio_is_ready_dt(&led)) {
        return 0;
    }

    if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE) < 0) {
        return 0;
    }

    while (true) {
        if (gpio_pin_toggle_dt(&led) < 0) {
            return 0;
        }

        led_state = !led_state;

        if (led_debugging) {
            LOG_INF("LED state: %s", led_state ? "ON" : "OFF");
        }

        k_msleep(CONFIG_BLINK_SLEEP_TIME_MS);
    }
    return 0;
}
