#include "platform.h"
#include "devices.h"
#include "hw.h"

static Hw hardware;
static int64_t unix_seconds = 1762992000;
static PortIrqHandler (*code_resolver)(void *, uint32_t);
static void *resolver_context;
static void (*render_line)(void *, unsigned);
static void (*render_write)(void *, unsigned, unsigned);
static void *render_context;
static void scanline(void *context, unsigned line)
{
    (void)context;
    if (render_line) render_line(render_context, line);
}
static uint8_t *map(void *context, uint32_t address, size_t size)
{
    (void)context;
#if defined(__wasm__)
    /* DMA sources can be locals on the compiler's portable stack, below the
     * hardware ranges. They are real linear-memory addresses, too. */
    if (address >= 0x10000 && address < PORT_EWRAM_BASE && size <= PORT_EWRAM_BASE - address)
        return (uint8_t *)(uintptr_t)address;
#endif
    return port_memory_pointer(address, size);
}
static int64_t now(void *context) { (void)context; return unix_seconds; }
static void step(void *context, uint32_t cycles)
{
    (void)context;
    hw_step_cycles(&hardware, cycles);
}
static void wait_irq(void *context, uint16_t mask, int discard)
{
    (void)context;
    (void)discard;
    if (mask & 1) (void)port_wait_vblank();
}
static PortIrqHandler resolve(void *context, uint32_t address)
{
    (void)context;
    if (code_resolver) return code_resolver(resolver_context, address);
    return NULL;
}
static void blank(void *context, unsigned line, int vblank)
{
    (void)context;
    /* Core raises VCount at the start of the line. hw_hblank additionally
     * performs HBlank DMA; suppress only its second VCount notification. */
    if (vblank) hw_vblank(&hardware);
    else {
        uint8_t enable = hardware.io[4] & 0x20;
        hardware.io[4] &= (uint8_t)~0x20;
        hw_hblank(&hardware, line);
        hardware.io[4] |= enable;
        *(uint16_t *)(uintptr_t)0x04000004 |= enable;
    }
}
static void acknowledge(void *context, uint16_t mask)
{
    (void)context;
    uint16_t flags = *(uint16_t *)(uintptr_t)0x04000202;
    hardware.io[0x202] = (uint8_t)flags;
    hardware.io[0x203] = (uint8_t)(flags >> 8);
    hw_write16(&hardware, 0x04000202, mask);
}
void port_devices_init(PortIrqHandler (*resolver)(void *, uint32_t), void *context)
{
    HwHost host = {0};
    struct PortPlatformHooks hooks = {0};
    code_resolver = resolver;
    resolver_context = context;
    host.map = map;
    host.unix_time = now;
    host.wait = wait_irq;
    hw_init(&hardware, &host);
    hw_bind(&hardware);
    hooks.advance = step;
    hooks.resolve = resolve;
    hooks.scanline = scanline;
    hooks.blank = blank;
    hooks.acknowledge = acknowledge;
    port_set_hooks(&hooks);
    port_timing_reset();
}
void port_devices_set_time(int64_t seconds) { unix_seconds = seconds; }

void port_devices_set_renderer(void (*line)(void *, unsigned),
                               void (*write)(void *, unsigned, unsigned), void *context)
{
    render_line = line;
    render_write = write;
    render_context = context;
}
void port_devices_notify_write(uint32_t address, unsigned width)
{
    if (render_write && address >= PORT_IO_BASE && address < PORT_IO_BASE + PORT_IO_SIZE)
        render_write(render_context, address - PORT_IO_BASE, width);
}

/* The generated hardware input renames only the implementation, leaving its
 * callers at this C boundary. Observe publications after the bytes are mapped;
 * DMA can reload affine latches even when the register value did not change. */
void port_hw_publish_io(Hw *hw, unsigned offset, unsigned size);
void hw_publish_io(Hw *hw, unsigned offset, unsigned size)
{
    static const unsigned references[] = {0x28, 0x2c, 0x38, 0x3c};
    unsigned i;
    port_hw_publish_io(hw, offset, size);
    if (hw != &hardware || !render_write || offset >= PORT_IO_SIZE) return;
    if (size > PORT_IO_SIZE - offset) size = PORT_IO_SIZE - offset;
    for (i = 0; i < sizeof(references) / sizeof(references[0]); ++i) {
        unsigned start = references[i];
        if (offset < start + 4 && offset + size > start)
            render_write(render_context, start, 4);
    }
}
