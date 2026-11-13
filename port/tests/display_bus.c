#include "platform.h"
#include "devices.h"

uint16_t port_bus_load_i16(const volatile void *);
void port_bus_store_i16(volatile void *, uint16_t);
static uint16_t observed_vcount, observed_status;
static void sample_registers(void)
{
    observed_vcount = port_bus_load_i16((void *)(uintptr_t)0x04000006);
    observed_status = port_bus_load_i16((void *)(uintptr_t)0x04000004);
}
static PortIrqHandler resolve(void *context, uint32_t address)
{
    (void)context;
    return address == 0x08001235 ? sample_registers : NULL;
}
void port_test_display_setup(void)
{
    observed_vcount = observed_status = 0xffff;
    port_devices_init(resolve, NULL);
    *(uint32_t *)(uintptr_t)0x03003a10 = 0x08001235;
    port_bus_store_i16((void *)(uintptr_t)0x04000004, 0x4020);
    port_bus_store_i16((void *)(uintptr_t)0x04000200, 4);
    port_bus_store_i16((void *)(uintptr_t)0x04000208, 1);
}
unsigned port_test_display_vcount(void) { return observed_vcount; }
unsigned port_test_display_status(void) { return observed_status; }
