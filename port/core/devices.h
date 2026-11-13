#ifndef BOKTAI_PORT_DEVICES_H
#define BOKTAI_PORT_DEVICES_H
#include "platform.h"
/* Attach the port-hw implementation to fixed-address memory and core timing.
 * The resolver supplied by the game link must return portable C IRQ handlers. */
void port_devices_init(PortIrqHandler (*resolve)(void *, uint32_t), void *context);
void port_devices_set_time(int64_t unix_seconds);
#endif
