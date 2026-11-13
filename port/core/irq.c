#include "platform.h"
#include "devices.h"

void Solar_OnTimerInterrupt(void);
void Solar_InitTimer(void);
static unsigned solar_calls;

static void solar_interrupt(void)
{
    ++solar_calls;
    Solar_OnTimerInterrupt();
}

static PortIrqHandler resolve_solar(void *context, uint32_t address)
{
    (void)context;
    /* Original ROM table entries are ARM addresses; writes made by compiled C
     * hold portable function pointers (wasm table indices). Recognize both. */
    if ((address & ~UINT32_C(1)) == UINT32_C(0x08243d28) ||
        address == (uint32_t)(uintptr_t)Solar_OnTimerInterrupt)
        return solar_interrupt;
    return NULL;
}

/* A focused runtime slice: original timer setup installs the original handler.
 * Unknown interrupt targets still fail explicitly in the core dispatcher. */
void port_solar_irq_init(void)
{
    solar_calls = 0;
    port_devices_init(resolve_solar, NULL);
    Solar_InitTimer();
}
unsigned port_solar_irq_calls(void) { return solar_calls; }
