#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(board_control, CONFIG_LOG_DEFAULT_LEVEL);

void board_late_init_hook(void) {
    LOG_INF("Board initialized!");
}