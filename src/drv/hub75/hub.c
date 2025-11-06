#include "hub.h"

#include <fb/fb.h>
#include <hardware/regs/intctrl.h>
#include <rpio.h>

#include "hub75.pio.h"
#include <hardware/dma.h>
#include <hardware/irq.h>
#include <hardware/pio.h>

/*
 * This driver is based on the work done on the wonderful pico-examples
 * https://github.com/raspberrypi/pico-examples/blob/master/pio/hub75
 */

static PIO hub_pio;
static uint32_t hub_sm_row, hub_sm_data, hub_data_prog_offs;
static uint32_t config_width, config_height;

static uint32_t hub_dma_ch;

static uint32_t fb_idx;
static uint32_t row_index, row_bit;
static struct rpio_rgb row_buf[2 * FB_WIDTH];

// irq handlers //

static void hub_next_row() {
    // step to the next row
    row_index = (row_index + 1) % (config_height / 2);
    row_bit = 0;

    // load-in new rows from fb
    for (uint32_t x = 0; x < config_width; x++) {
        struct rpio_rgb *fb = fb_get(fb_idx);

        row_buf[x * 2 + 0] = fb[row_index * FB_WIDTH + x];
        row_buf[x * 2 + 1] = fb[row_index + 32 * FB_WIDTH + x];
    }
}

static void hub_row_irq() {
    if (!dma_irqn_get_channel_status(0, hub_dma_ch))
        return;
    dma_irqn_acknowledge_channel(0, hub_dma_ch);

    // end the current row bit stream //

    // Dummy pixel per lane
    pio_sm_put_blocking(hub_pio, hub_sm_data, 0);
    pio_sm_put_blocking(hub_pio, hub_sm_data, 0);
    // SM is finished when it stalls on empty TX FIFO
    hub75_wait_tx_stall(hub_pio, hub_sm_data);
    // Also check that previous OEn pulse is finished, else things can get out of sequence
    hub75_wait_tx_stall(hub_pio, hub_sm_row);

    // shuffle row_index to match the rowsel pinout
    uint32_t row_en = (row_en >> 4) | ((row_en << 1) & 31);

    // Latch row data, pulse output enable for new row.
    pio_sm_put_blocking(hub_pio, hub_sm_row, row_en | (100u * (1u << row_bit) << 5));

    // start the next row bit stream //

    row_bit++;
    if (row_bit == 8) {
        hub_next_row();
    }

    // setup the pio for the next bit
    hub75_data_rgb888_set_shift(hub_pio, hub_sm_data, hub_data_prog_offs, row_bit);

    // stream row data in the background
    dma_channel_transfer_from_buffer_now(hub_dma_ch, row_buf, sizeof(struct rpio_rgb) * 2 * config_width);
}

// public api //

void hub75_init(struct rpio_hub75_init *args) {
    if (args->width == 0 || config_width != 0)
        return;

    config_width = args->width;
    config_height = args->height;

    // setup pio //
    hub_pio = pio0; // FIXME: this needs better logic
    hub_sm_data = 0;
    hub_sm_row = 1;

    uint32_t data_offs = pio_add_program(hub_pio, &hub75_data_rgb888_program);
    uint32_t row_offs = pio_add_program(hub_pio, &hub75_row_program);

    hub75_data_rgb888_program_init(hub_pio, hub_sm_data, data_offs, args->data_base, args->clk_pin);
    hub75_row_program_init(hub_pio, hub_sm_row, row_offs, args->rows_base, 5, args->ctrl_base);
    hub_data_prog_offs = data_offs;

    // setup dma and irq //
    hub_dma_ch = dma_claim_unused_channel(true);

    dma_channel_config_t c = dma_channel_get_default_config(hub_dma_ch);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
    channel_config_set_dreq(&c, pio_get_dreq(hub_pio, hub_sm_data, true));

    dma_channel_set_config(hub_dma_ch, &c, false);

    dma_irqn_set_channel_enabled(0, hub_dma_ch, true);
    irq_add_shared_handler(DMA_IRQ_0, hub_row_irq, PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY);
    irq_set_enabled(DMA_IRQ_0, true);

    // start initial row
    row_index = config_height / 2;
    hub_next_row();

    hub75_data_rgb888_set_shift(hub_pio, hub_sm_data, hub_data_prog_offs, row_bit);
    dma_channel_transfer_from_buffer_now(hub_dma_ch, row_buf, sizeof(struct rpio_rgb) * 2 * config_width);
}

void hub75_flip(struct rpio_hub75_flip *args) {
    fb_idx = args->fb;
}
