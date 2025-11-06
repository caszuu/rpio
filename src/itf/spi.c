// default spi config //

#ifndef RPIO_SPI_RX_PIN
#define RPIO_SPI_RX_PIN 16
#endif

#ifndef RPIO_SPI_SCK_PIN
#define RPIO_SPI_SCK_PIN 18
#endif

#ifndef RPIO_SPI_TX_PIN
#define RPIO_SPI_TX_PIN 19
#endif

#ifndef RPIO_SPI_CSN_PIN
#define RPIO_SPI_CSN_PIN 17
#endif

#ifndef RPIO_SPI_NUM
#define RPIO_SPI_NUM 0
#endif

// spi interface //

#include "itf.h"

#include <hardware/gpio.h>
#include <hardware/spi.h>
#include <stdbool.h>

static spi_inst_t *spi_inst;

bool itf_is_readable() {
    return spi_is_readable(spi_inst);
}

void itf_write(void *src, uint32_t size, bool fin) {
    if (size == 0)
        return;

    spi_write_blocking(spi_inst, src, size);
}

void itf_read(void *dst, uint32_t size) {
    if (size == 0)
        return;

    spi_read_blocking(spi_inst, 0, dst, size);
}

void itf_spi_init(uint32_t rate) {
    // setup gpio

    gpio_set_function(RPIO_SPI_RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(RPIO_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(RPIO_SPI_TX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(RPIO_SPI_CSN_PIN, GPIO_FUNC_SPI);

    // setup spi

    spi_inst = SPI_INSTANCE(RPIO_SPI_NUM);
    spi_init(spi_inst, rate);
    spi_set_slave(spi_inst, true);
}

void itf_side_write(void *src, uint32_t size) {
    // FIXME: implement sideband
}

void itf_poll() {
    // FIXME: implement sideband
}
