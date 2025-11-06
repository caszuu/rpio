#pragma once

#include <itf.h>
#include <rpio.h>

void hub75_init(struct rpio_hub75_init *args);
void hub75_flip(struct rpio_hub75_flip *args);

static inline void hub75_cmd(uint8_t cmd) {
    switch (cmd) {
    case rpio_hub75_init_cmd: {
        struct rpio_hub75_init args;
        itf_read(&args, sizeof(args));

        hub75_init(&args);
        break;
    }

    case rpio_hub75_flip_cmd: {
        struct rpio_hub75_flip args;
        itf_read(&args, sizeof(args));

        hub75_flip(&args);
        break;
    }

    default:
        break;
    }
}
