#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

enum rpio_cmd_type {
    rpio_ctype_nop = 0, // no-op / ignore

    rpio_ctype_fb,    // framebuffer related cmds
    rpio_ctype_hub75, // hub75 driver cmds
};

struct rpio_rgb {
    uint8_t r, g, b;
};

// == fb cmds ==

// The rpio has support for _4_ fixed-sized framebuffers. These can be used
// and generic image storage or actively used by other drivers on the rpio module. (eg. hub75)
//
// Every framebuffer stores an RGB888 (24-bits per pixel, 8-bits per channel) image with a size
// set at compile-time (by default 128x128). It is safe to read and write outside the range of
// the framebuffer and pixels outside it will be discarded or return as black.

enum rpio_fb_cmd {
    rpio_fb_clear_cmd, // clear a fb with a solid color
    rpio_fb_draw_cmd,  // draws a bitmap (sent with cmd) to a fb
    rpio_fb_blit_cmd,  // copy an area from a src fb to a dst fb

    rpio_fb_read_cmd, // read the contents of a fb back
};

struct rpio_fb_clear {
    struct rpio_rgb color;
    uint8_t fb;
};

struct rpio_fb_blit {
    uint16_t src_x, src_y; // source position of the area
    uint16_t dst_x, dst_y; // target position of the area
    uint16_t w, h;         // size of the area

    uint8_t src_fb, dst_fb; // source and target fb indices
};

struct rpio_fb_draw {
    uint16_t x, y; // target position of the image
    uint16_t w, h; // size of the image
    uint8_t fb;    // target fb index

    /* a `struct rpio_rgb bitmap[w * h]` follows on the interface */
};

struct rpio_fb_read {
    uint16_t x, y; // source position of the area
    uint16_t w, h; // size of the area
    uint8_t fb;    // source fb index

    /* a `struct rpio_rgb bitmap[w * h]` will be returned on the interface */
};

// == hub75 cmds ==

enum rpio_hub75_cmd {
    rpio_hub75_init_cmd = 0, // init the hub75 driver with user-defined pins and led count
    rpio_hub75_deinit_cmd,   // stop and deinit the hub75 driver
    rpio_hub75_flip_cmd,     // switch the framebuffer being used for hub75 output
};

struct rpio_hub75_init {
    uint16_t data_base; // base pin for the R0, G0, B0, R1, G1, B1 pins respectivelly
    uint16_t rows_base; // base pin for the E, A, B, C, D pins respectivelly
    uint16_t ctrl_base; // base pin for the LAT, OEn pins respectivelly
    uint16_t clk_pin;   // pin number for CLK

    uint8_t width, height; // size of the connected led matrix
};

struct rpio_hub75_flip {
    uint8_t fb; // target fb index to display
};

#ifdef __cplusplus
}
#endif
