#include <zephyr/drivers/sensor.h>
#include <zephyr/shell/shell.h>

static int cmd_greet_handler(struct shell const* shell, size_t argc, char** argv) {
    shell_info(shell, "Hello from our driver!");
    return 0;
}

static int cmd_read_handler(struct shell const* shell, size_t argc, char** argv) {
    struct device const* dev = shell_device_get_binding(argv[1]);
    if (dev == NULL) {
        shell_error(shell, "Device '%s' not found", argv[1]);
        return -ENODEV;
    }

    struct sensor_value val;
    int ret = sensor_channel_get(dev, SENSOR_CHAN_ALL, &val);

    if (ret < 0) {
        shell_error(shell, "Failed to get channel data: %d", ret);
        return ret;
    }

    shell_info(shell, "Channel data: val1=%d, val2=%d", val.val1, val.val2);

    return 0;
}

static int cmd_fetch_handler(struct shell const* shell, size_t argc, char** argv) {
    struct device const* dev = shell_device_get_binding(argv[1]);
    if (dev == NULL) {
        shell_error(shell, "Device '%s' not found", argv[1]);
        return -ENODEV;
    }

    int ret = sensor_sample_fetch(dev);
    if (ret < 0) {
        shell_error(shell, "Failed to fetch sample: %d", ret);
        return ret;
    }

    shell_info(shell, "Fetch result: %d", ret);
    return 0;
}

static int cmd_info_handler(struct shell const* shell, size_t argc, char** argv) {
    struct device const* dev = shell_device_get_binding(argv[1]);
    if (dev == NULL) {
        shell_error(shell, "Device '%s' not found", argv[1]);
        return -ENODEV;
    }

    bool ready = device_is_ready(dev);
    shell_info(shell, "Device '%s' is %s", argv[1], ready ? "ready" : "not ready");
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(our_driver_subcmds,
    SHELL_CMD(hello, NULL, "Greeting from our driver", cmd_greet_handler),
    SHELL_CMD_ARG(read, NULL, "Get channel data", cmd_read_handler, 2, 0),
    SHELL_CMD_ARG(fetch, NULL, "Fetch sensor sample", cmd_fetch_handler, 2, 0),
    SHELL_CMD_ARG(info, NULL, "Print device name and ready state", cmd_info_handler, 2, 0),
    SHELL_SUBCMD_SET_END,
);

SHELL_CMD_REGISTER(sensor, &our_driver_subcmds, "Commands for our driver", NULL);