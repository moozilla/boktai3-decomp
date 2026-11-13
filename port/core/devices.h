#ifndef BOKTAI_PORT_DEVICES_H
#define BOKTAI_PORT_DEVICES_H
#include "platform.h"
/* Attach the port-hw implementation to fixed-address memory and core timing.
 * The resolver supplied by the game link must return portable C IRQ handlers. */
void port_devices_init(PortIrqHandler (*resolve)(void *, uint32_t), void *context);
void port_devices_set_time(int64_t unix_seconds);
/* Core owns display timing and IRQ masks/flags. Copy those mapped registers to
 * the device shadow before bus access or device advancement, without replaying
 * writes (in particular, IF must not be mistaken for a write-one-to-clear). */
void port_devices_sync_timing(void);
/* Optional renderer receives visible lines before HBlank and register writes. */
void port_devices_set_renderer(void (*line)(void *, unsigned),
                               void (*write)(void *, unsigned, unsigned), void *context);
void port_devices_notify_write(uint32_t address, unsigned width);
#endif
