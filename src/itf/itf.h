#pragma once

#include <stdbool.h>
#include <stdint.h>

// returns true if any data is available to read-in
bool itf_is_readable();

// read-in a segment of the cmd
void itf_read(void *dst, uint32_t size);

// write-out a segment of the cmd response, if the fin bit
// is set, no writes will follow for the current packet and
// can be submited on packet-based interfaces (eg. usb)
void itf_write(void *src, uint32_t size, bool fin);

// writes a packet to the sideband queue and waits for a poll
// before being transfered, can be aliased with itf_write if
// manual-polling is not required on the interface
void itf_side_write(void *src, uint32_t size);

// poll a packet from the sideband queue, if available
// can be a no-op if manual-polling is not required on
// the interface
void itf_poll();

#ifdef RPIO_INTERFACE_SPI
void itf_spi_init(uint32_t rate);
#endif
