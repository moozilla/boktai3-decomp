#include "platform.h"
#define REG16(offset) (*(volatile uint16_t *)(uintptr_t)(PORT_IO_BASE + (offset)))
static struct PortPlatformHooks platform;
static unsigned current_line;
static uint64_t frames;
static unsigned dispatch_depth;

void port_set_hooks(const struct PortPlatformHooks *hooks)
{
    if (hooks) platform = *hooks;
    else {
        struct PortPlatformHooks empty = {0};
        platform = empty;
    }
}

void port_timing_reset(void)
{
    current_line = 0;
    frames = 0;
    dispatch_depth = 0;
    REG16(6) = 0;
    REG16(4) &= (uint16_t)~7u;
}

void port_irq_raise(uint16_t mask) { REG16(0x202) |= mask & 0x3fff; }

int port_irq_dispatch(void)
{
    /* IntrMain, ROM 080001F0: table order is VCount, serial, timer3,
     * HBlank, VBlank, timer0, timer1, timer2, DMA0..3, keypad. */
    static const uint16_t order[] = {4, 128, 64, 2, 1, 8, 16, 32, 256, 512, 1024, 2048, 4096};
    unsigned serviced = 0;
    if (dispatch_depth >= 16) return -4;
    while (REG16(0x208) & 1) {
        uint16_t pending = REG16(0x200) & REG16(0x202);
        uint16_t saved_ie, saved_ime;
        uint32_t target;
        PortIrqHandler handler;
        unsigned slot;
        if (!pending) break;
        for (slot = 0; slot < sizeof(order) / sizeof(order[0]); ++slot)
            if (pending & order[slot]) break;
        if (slot == sizeof(order) / sizeof(order[0])) return -3; /* Game Pak halt */
        target = *(uint32_t *)(uintptr_t)(PORT_IWRAM_BASE + 0x3a10 + slot * 4);
        handler = platform.resolve ? platform.resolve(platform.context, target) : NULL;
        if (!handler) return -2;
        saved_ie = REG16(0x200);
        saved_ime = REG16(0x208);
        REG16(0x202) &= (uint16_t)~order[slot];
        /* The SDK permits serial/timer/video nesting. Its timer selection
         * comes from the serial state pointer at 03006A50. */
        {
            uint32_t state = *(uint32_t *)(uintptr_t)0x03006a50;
            uint8_t *timer = port_memory_pointer(state + 10, 1);
            uint16_t timer_mask = timer && *timer < 4 ? (uint16_t)(8u << *timer) : 0;
            REG16(0x200) = saved_ie & (uint16_t)(0x20c6 | timer_mask);
        }
        REG16(0x208) = slot == 0 ? 0 : 1;
        if (platform.trace) platform.trace(platform.context, "irq", order[slot]);
        ++dispatch_depth;
        handler();
        --dispatch_depth;
        REG16(0x200) = saved_ie;
        REG16(0x208) = saved_ime;
        if (++serviced == 64) return -4;
    }
    return (int)serviced;
}

static int advance(uint32_t cycles)
{
    if (platform.advance) platform.advance(platform.context, cycles);
    return port_irq_dispatch();
}

int port_run_scanline(void)
{
    uint16_t status;
    int result;
    REG16(6) = (uint16_t)current_line;
    status = REG16(4) & (uint16_t)~7u;
    /* Hardware drops VBlank status on line 227, before line 0. */
    if (current_line >= 160 && current_line < 227) status |= 1;
    if (current_line == (status >> 8)) status |= 4;
    REG16(4) = status;
    if (current_line == 160 && (status & 8)) port_irq_raise(1);
    if ((status & 4) && (status & 0x20)) port_irq_raise(4);
    result = port_irq_dispatch();
    if (result < 0) return result;
    result = advance(PORT_HDRAW_CYCLES);
    if (result < 0) return result;
    if (current_line < PORT_VISIBLE_LINES && platform.scanline)
        platform.scanline(platform.context, current_line);
    REG16(4) |= 2;
    if (REG16(4) & 0x10) port_irq_raise(2);
    result = port_irq_dispatch();
    if (result < 0) return result;
    result = advance(PORT_CYCLES_PER_LINE - PORT_HDRAW_CYCLES);
    if (result < 0) return result;
    if (++current_line == PORT_SCANLINES) { current_line = 0; ++frames; }
    return 0;
}

int port_run_frame(void)
{
    unsigned i;
    for (i = 0; i < PORT_SCANLINES; ++i) {
        int result = port_run_scanline();
        if (result < 0) return result;
    }
    return 0;
}

int port_wait_vblank(void)
{
    unsigned i;
    if (!(REG16(0x208) & 1) || !(REG16(0x200) & 1) || !(REG16(4) & 8)) return -1;
    for (i = 0; i < PORT_SCANLINES; ++i) {
        unsigned line = current_line;
        int result = port_run_scanline();
        if (result < 0) return result;
        if (line == PORT_VISIBLE_LINES) return 0;
    }
    return -1;
}

uint64_t port_frame_count(void) { return frames; }
