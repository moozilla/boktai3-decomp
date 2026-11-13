#include "platform.h"
#include "devices.h"

void sub_08213FD8(void);
void sub_08214144(void);
void InterruptsAndVideo_Initialize(void);
static unsigned vblank_calls;
static unsigned vcount_calls;
static void vblank(void)
{
    ++vblank_calls;
    sub_08213FD8();
}
static void vcount(void)
{
    ++vcount_calls;
    sub_08214144();
}
static PortIrqHandler resolve_video(void *context, uint32_t address)
{
    (void)context;
    if ((address & ~UINT32_C(1)) == UINT32_C(0x08213fd8) ||
        address == (uint32_t)(uintptr_t)sub_08213FD8)
        return vblank;
    if ((address & ~UINT32_C(1)) == UINT32_C(0x08214144) ||
        address == (uint32_t)(uintptr_t)sub_08214144)
        return vcount;
    return NULL;
}
void port_vblank_irq_init(void)
{
    vblank_calls = 0;
    vcount_calls = 0;
    port_devices_init(resolve_video, NULL);
    InterruptsAndVideo_Initialize();
}
unsigned port_vblank_irq_calls(void) { return vblank_calls; }
unsigned port_vcount_irq_calls(void) { return vcount_calls; }
