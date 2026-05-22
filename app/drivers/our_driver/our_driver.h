#ifndef OUR_DRIVER_H
#define OUR_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif


#include <zephyr/drivers/sensor.h>
#include <zephyr/device.h>
#include <errno.h>
#include <stdint.h>

#define DT_DRV_COMPAT our_driver

struct our_driver_api {
    struct sensor_driver_api sensor;
    uint32_t (*increment)(struct device const* dev, uint32_t value);
};

static inline int our_driver_increment(struct device const* dev, uint32_t value) {
    struct our_driver_api const* api = (struct our_driver_api const*)dev->api;
    if ((api == NULL) || (api->increment == NULL)) {
        return -ENOSYS;
    }

    return api->increment(dev, value);
}

#ifdef __cplusplus
}
#endif

#endif // OUR_DRIVER_H
