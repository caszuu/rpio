#include <hardware/watchdog.h>
#include <pico/multicore.h>

#include <itf.h>
#include <rpio.h>

#include <fb/fb.h>
#include <hub75/hub.h>

static void task_loop() {
}

extern void misc_cmd(uint8_t cmd);

static void cmd_loop() {
    while (true) {
        uint8_t ctype;
        itf_read(&ctype, 1);

        if (ctype == rpio_ctype_nop)
            continue;

        uint8_t cmd;
        itf_read(&cmd, 1);

        switch (ctype) {
        case rpio_ctype_misc:
            misc_cmd(cmd);
            break;

        case rpio_ctype_fb:
            fb_cmd(cmd);
            break;

        case rpio_ctype_hub75:
            hub75_cmd(cmd);
            break;

        default:
            break; // FIXME: handle on itf? reset?
        }
    }
}

int main() {
    // setup interface

#ifdef RPIO_INTERFACE_SPI
    itf_spi_init(1000 * 1000);
#endif

    // setup loops

    multicore_launch_core1(&task_loop);
    cmd_loop();
}
