#pragma once

#ifndef FB_WIDTH
#define FB_WIDTH 128
#endif

#ifndef FB_HEIGHT
#define FB_HEIGHT 128
#endif

#include <itf.h>
#include <rpio.h>
#include <stdint.h>

void fb_clear(uint8_t fb, struct rpio_rgb color);
void fb_draw_image(const struct rpio_fb_draw *args);
void fb_blit(struct rpio_fb_blit *args);
void fb_read(struct rpio_fb_read *args);

static inline void fb_cmd(uint8_t cmd) {
    switch (cmd) {
    case rpio_fb_clear_cmd: {
        struct rpio_fb_clear args;
        itf_read(&args, sizeof(args));

        fb_clear(args.fb, args.color);
        break;
    }

    case rpio_fb_draw_cmd: {
        struct rpio_fb_draw args;
        itf_read(&args, sizeof(args));

        fb_draw_image(&args);
        break;
    }

    case rpio_fb_blit_cmd: {
        struct rpio_fb_blit args;
        itf_read(&args, sizeof(args));

        fb_blit(&args);
        break;
    }

    case rpio_fb_read_cmd: {
        struct rpio_fb_read args;
        itf_read(&args, sizeof(args));

        fb_read(&args);
        break;
    }

    default:
        break;
    }
}

struct rpio_rgb *fb_get(uint8_t fb);
