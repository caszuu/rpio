#include "fb.h"
#include <itf.h>
#include <rpio.h>

#include <pico.h>
#include <stdint.h>
#include <string.h>

static struct rpio_rgb fbs[4][FB_HEIGHT][FB_WIDTH];

void fb_clear(uint8_t fb, struct rpio_rgb color) {
    for (uint32_t y = 0; y < FB_HEIGHT; y++) {
        for (uint32_t x = 0; x < FB_WIDTH; x++) {
            fbs[fb][y][x] = color;
        }
    }
}

void fb_draw_image(const struct rpio_fb_draw *args) {
    // compute start and end pixels of the draw
    int32_t bx = MAX(0, args->x), ex = MIN(FB_WIDTH, args->x + args->w);
    int32_t by = MAX(0, args->y), ey = MIN(FB_HEIGHT, args->y + args->h);

    int32_t ux = -MIN(0, args->x), ox = MAX(0, args->x + args->w - FB_WIDTH);
    int32_t uy = -MIN(0, args->y), oy = MAX(0, args->y + args->h - FB_HEIGHT);

    // blit bitmap from interface
    struct rpio_rgb temp;

    // discard underflow scanlines
    for (uint32_t i = 0; i < uy; i++)
        for (uint32_t x = 0; x < args->w; x++)
            itf_read(&temp, sizeof(temp));

    for (int32_t y = by; y < ey; y++) {
        // discard underflow pixels
        for (uint32_t i = 0; i < ux; i++)
            itf_read(&temp, sizeof(temp));

        // blit visible scanline
        itf_read(&fbs[args->fb][y][bx], (ex - bx) * sizeof(struct rpio_rgb));

        // discard overflow pixels
        for (uint32_t i = 0; i < ox; i++)
            itf_read(&temp, sizeof(temp));
    }

    // discard overflow scanlines
    for (uint32_t i = 0; i < oy; i++)
        for (uint32_t x = 0; x < args->w; x++)
            itf_read(&temp, sizeof(temp));
}

void fb_blit(struct rpio_fb_blit *args) {
    // compute write area clamped to fb edges
    int32_t wbx = MAX(0, args->dst_x), wex = MIN(FB_WIDTH, args->dst_x + args->w);
    int32_t wby = MAX(0, args->dst_y), wey = MIN(FB_HEIGHT, args->dst_y + args->h);

    int32_t wux = -MIN(0, args->dst_x), wuy = -MIN(0, args->dst_y);
    uint32_t cw = wex - wbx, ch = wey - wby;

    // compute read area with overflow and underflow
    int32_t bx = MAX(0, args->src_x + wux), ex = MIN(FB_WIDTH, args->src_x + wux + cw);
    int32_t by = MAX(0, args->src_y + wuy), ey = MIN(FB_HEIGHT, args->src_y + wuy + ch);

    int32_t ux = -MIN(0, args->src_x + wux), ox = MAX(0, args->src_x + wux + cw - FB_WIDTH);
    int32_t uy = -MIN(0, args->src_y + wuy), oy = MAX(0, args->src_y + wuy + ch - FB_HEIGHT);

    // perform blit

    // write blank underflow
    for (uint32_t i = 0; i < uy; i++)
        for (int32_t x = wbx; x < wex; x++)
            fbs[args->dst_fb][wby + i][x] = (struct rpio_rgb){0, 0, 0};

    for (int32_t wy = wby + uy, ry = by; wy < wey - oy; wy++, ry++) {
        // write blank underflow pixels
        for (uint32_t i = 0; i < ux; i++)
            fbs[args->dst_fb][wy][wbx + i] = (struct rpio_rgb){0, 0, 0};

        // blit active pixels
        memcpy(&fbs[args->dst_fb][wy][wbx + ux], &fbs[args->src_fb][ry][bx], (ex - bx) * sizeof(struct rpio_rgb));

        // write blank overflow pixels
        for (uint32_t i = 0; i < ox; i++)
            fbs[args->dst_fb][wy][wex - ox + i] = (struct rpio_rgb){0, 0, 0};
    }

    // write blank overflow
    for (uint32_t i = 0; i < oy; i++)
        for (int32_t x = wbx; x < wex; x++)
            fbs[args->dst_fb][wey - oy + i][x] = (struct rpio_rgb){0, 0, 0};
}

void fb_read(struct rpio_fb_read *args) {
    // compute start and end pixels of the read
    int32_t bx = MAX(0, args->x), ex = MIN(FB_WIDTH, args->x + args->w);
    int32_t by = MAX(0, args->y), ey = MIN(FB_HEIGHT, args->y + args->h);

    int32_t ux = -MIN(0, args->x), ox = MAX(0, args->x + args->w - FB_WIDTH);
    int32_t uy = -MIN(0, args->y), oy = MAX(0, args->y + args->h - FB_HEIGHT);

    struct rpio_rgb z = {0};

    // null underflow scanlines
    for (uint32_t i = 0; i < uy; i++)
        for (uint32_t x = 0; x < args->w; x++)
            itf_write(&z, sizeof(z), false);

    for (int32_t y = by; y < ey; y++) {
        // null underflow pixels
        for (uint32_t i = 0; i < ux; i++)
            itf_write(&z, sizeof(z), false);

        // read-back visible scanline
        itf_write(&fbs[args->fb][y][bx], (ex - bx) * sizeof(struct rpio_rgb), false);

        // null overflow pixels
        for (uint32_t i = 0; i < ox; i++)
            itf_write(&z, sizeof(z), false);
    }

    // null overflow scanlines
    for (uint32_t i = 0; i < oy; i++)
        for (uint32_t x = 0; x < args->w; x++)
            itf_write(&z, sizeof(z), false);

    itf_write(NULL, 0, true);
}

struct rpio_rgb *fb_get(uint8_t fb) {
    return (struct rpio_rgb *)fbs[fb];
}
