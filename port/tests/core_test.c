#include "platform.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static unsigned irq_counts[3], irq_order[512], irq_count, lines;
static uint64_t cycles;
static void vblank(void) { ++irq_counts[0]; irq_order[irq_count++] = 1; }
static void hblank(void) { ++irq_counts[1]; irq_order[irq_count++] = 2; }
static void vcount(void) { ++irq_counts[2]; irq_order[irq_count++] = 4; }
static PortIrqHandler resolve(void *context, uint32_t address)
{
    (void)context;
    switch (address) {
    case 0x08000001: return vblank;
    case 0x08000003: return hblank;
    case 0x08000005: return vcount;
    default: return NULL;
    }
}
static void step(void *context, uint32_t elapsed) { (void)context; cycles += elapsed; }
static void scanline(void *context, unsigned line)
{
    (void)context;
    assert(line == lines++);
}
#define IO16(n) (*(uint16_t *)(uintptr_t)(PORT_IO_BASE + (n)))
int main(void)
{
    uint8_t rom[0x100];
    struct PortPlatformHooks hooks = {0};
    assert(port_memory_init() == 0);
    assert(port_memory_init() == 0);
    assert(port_memory_pointer(PORT_EWRAM_BASE, PORT_EWRAM_SIZE));
    assert(!port_memory_pointer(PORT_EWRAM_BASE - 1, 1));
    assert(!port_memory_pointer(PORT_EWRAM_BASE + PORT_EWRAM_SIZE, 1));
    assert(!port_memory_pointer(PORT_ROM_BASE, (size_t)-1));
    assert(IO16(0x130) == 0x3ff);
    memset(rom, 0x5a, sizeof(rom));
    assert(port_load_rom(rom, sizeof(rom)) == 0);
    assert(!memcmp((void *)(uintptr_t)PORT_ROM_BASE, rom, sizeof(rom)));
    assert(port_load_rom(rom, 0xbf) < 0);
    assert(port_load_rom(NULL, sizeof(rom)) < 0);
    *(uint32_t *)(uintptr_t)PORT_EWRAM_BASE = 123;
    port_memory_reset();
    assert(*(uint32_t *)(uintptr_t)PORT_EWRAM_BASE == 0);
    assert(*(uint8_t *)(uintptr_t)PORT_ROM_BASE == 0x5a);
    assert(port_rom_size() == sizeof(rom));
    assert(port_load_rom(rom, 0xc0) == 0);
    assert(*(uint8_t *)(uintptr_t)(PORT_ROM_BASE + 0xc0) == 0);
    hooks.resolve = resolve;
    hooks.advance = step;
    hooks.scanline = scanline;
    port_set_hooks(&hooks);
    port_timing_reset();
    *(uint32_t *)(uintptr_t)0x03003a10 = 0x08000005;
    *(uint32_t *)(uintptr_t)0x03003a1c = 0x08000003;
    *(uint32_t *)(uintptr_t)0x03003a20 = 0x08000001;
    IO16(4) = 0x4038; /* VCount at 64; VBlank/HBlank/VCount IRQs */
    IO16(0x200) = 7;
    IO16(0x208) = 1;
    assert(port_run_frame() == 0);
    assert(irq_counts[0] == 1 && irq_counts[1] == 228 && irq_counts[2] == 1);
    assert(lines == 160 && cycles == 280896 && port_frame_count() == 1);
    assert((IO16(4) & 1) == 0); /* VBlank ends on 227 */
    port_irq_raise(7);
    irq_count = 0;
    assert(port_irq_dispatch() == 3);
    assert(irq_order[0] == 4 && irq_order[1] == 2 && irq_order[2] == 1);
    assert(IO16(0x202) == 0 && IO16(0x200) == 7 && IO16(0x208) == 1);
    IO16(0x208) = 0;
    port_irq_raise(1);
    assert(port_irq_dispatch() == 0 && IO16(0x202) == 1);
    assert(port_wait_vblank() < 0);
    IO16(0x208) = 1;
    *(uint32_t *)(uintptr_t)0x03003a20 = 0;
    assert(port_irq_dispatch() == -2);
    port_memory_destroy();
    assert(!port_memory_pointer(PORT_EWRAM_BASE, 1));
    puts("core: memory map, ROM loading, 280896-cycle frame and IntrMain dispatch PASS");
    return 0;
}
