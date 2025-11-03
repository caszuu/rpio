#include <pico/unique_id.h>

#include <itf.h>
#include <rpio.h>

#include <stdint.h>
#include <string.h>

static void misc_stat() {
    struct rpio_misc_stat_resp resp = {
        .magic_num = 0xfe,
        .proto_ver = RPIO_VERSION,
    };

    itf_write(&resp, sizeof(resp), true);
}

static void misc_hwinfo() {
    struct rpio_misc_hwinfo_resp resp = {
        .itf_bitset = 0,
        .drv_bitset = 0, // FIXME: bitset macros
    };

    pico_unique_board_id_t id;
    pico_get_unique_board_id(&id);

    memcpy(resp.hw_uuid, id.id, sizeof(resp.hw_uuid));
    itf_write(&resp, sizeof(resp), true);
}

void misc_cmd(uint8_t cmd) {
    switch (cmd) {
    case rpio_misc_stat_cmd:
        misc_stat();
        return;

    case rpio_misc_hwinfo_cmd:
        misc_hwinfo();
        return;

    case rpio_misc_poll_cmd:
        itf_poll();
        return;
    }
}
