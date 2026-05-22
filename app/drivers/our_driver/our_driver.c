#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

#include "our_driver.h"

LOG_MODULE_REGISTER(our_driver, LOG_LEVEL_INF);

struct our_driver_config {
    struct gpio_dt_spec led;
};

struct our_driver_data {
    uint32_t counter;
};

static int our_driver_sample_fetch(struct device const* dev, enum sensor_channel chan) {
    (void)chan;

    LOG_INF("Sample fetch: LED on");
    struct our_driver_config const* config = dev->config;
    return gpio_pin_set_dt(&config->led, 1);
}

static int our_driver_channel_get(struct device const* dev, enum sensor_channel chan, struct sensor_value* val) {
    (void)chan;
    (void)val;

    LOG_INF("Channel get: LED off");
    struct our_driver_config const* config = dev->config;
    int result = gpio_pin_set_dt(&config->led, 0);
    val->val1 = result;
    val->val2 = 0;
    return result;
}

static uint32_t our_driver_increment_impl(struct device const* dev) {
    struct our_driver_data* data = dev->data;
    data->counter++;
    return data->counter;
}

static int our_driver_init(struct device const* dev) {
    struct our_driver_config const* config = dev->config;
    if (!gpio_is_ready_dt(&config->led)) {
        LOG_ERR("LED GPIO is not ready");
        return -ENODEV;
    }

    LOG_INF("Our driver initialized");
    return gpio_pin_configure_dt(&config->led, GPIO_OUTPUT_INACTIVE);
}

static const struct our_driver_api our_driver_api = {
    .sensor = {
        .sample_fetch = our_driver_sample_fetch,
        .channel_get = our_driver_channel_get,
    },
    .increment = our_driver_increment_impl,
};

#define OUR_DRIVER_DATA(inst) \
    static struct our_driver_data our_driver_data_##inst = { \
        .counter = 0, \
    }

#define OUR_DRIVER_CONFIG(inst) \
    static const struct our_driver_config our_driver_config_##inst = { \
        .led = GPIO_DT_SPEC_GET(DT_INST_PHANDLE(inst, led), gpios), \
    }

#define DEV_INST(inst) \
    OUR_DRIVER_DATA(inst); \
    OUR_DRIVER_CONFIG(inst); \
    DEVICE_DT_INST_DEFINE(inst, our_driver_init, NULL, &our_driver_data_##inst, &our_driver_config_##inst, \
                          POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE, &our_driver_api)

DT_INST_FOREACH_STATUS_OKAY(DEV_INST);
