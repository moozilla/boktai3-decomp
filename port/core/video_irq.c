#include "platform.h"
#include "devices.h"

void sub_08213FD8(void);
void InterruptsAndVideo_Initialize(void);
static unsigned vblank_calls;
static void vblank(void)
{
    ++vblank_calls;
    sub_08213FD8();
}
static PortIrqHandler resolve_video(void *context, uint32_t address)
{
    (void)context;
    if ((address & ~UINT32_C(1)) == UINT32_C(0x08213fd8) ||
        address == (uint32_t)(uintptr_t)sub_08213FD8)
        return vblank;
    return NULL;
}
void port_vblank_irq_init(void)
{
    vblank_calls = 0;
    port_devices_init(resolve_video, NULL);
    InterruptsAndVideo_Initialize();
}
unsigned port_vblank_irq_calls(void) { return vblank_calls; }
