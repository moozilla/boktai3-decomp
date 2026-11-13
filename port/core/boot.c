#include "platform.h"
#include "boot.h"

void Rtc_Initialize(void);
void Clock_ClearState(void);
static unsigned boot_calls;

int port_boot_early(void)
{
    uint32_t cookie0, cookie1;
    uint32_t *ewram;
    uint32_t *iwram;
    unsigned i;
    if (!port_memory_pointer(PORT_EWRAM_BASE, PORT_EWRAM_SIZE) || port_rom_size() < 0x145c)
        return -1;
    cookie0 = *(uint32_t *)(uintptr_t)0x0201f400;
    cookie1 = *(uint32_t *)(uintptr_t)0x0201f404;
    *(uint16_t *)(uintptr_t)0x04000204 = 0x4014;
    ewram = (uint32_t *)(uintptr_t)PORT_EWRAM_BASE;
    for (i = 0; i < PORT_EWRAM_SIZE / 4; ++i) ewram[i] = 0;
    *(uint32_t *)(uintptr_t)0x0201f400 = cookie0;
    *(uint32_t *)(uintptr_t)0x0201f404 = cookie1;
    /* The original DMA clears 0x7D00 bytes, leaving the BIOS/stack tail. */
    iwram = (uint32_t *)(uintptr_t)PORT_IWRAM_BASE;
    for (i = 0; i < 0x1f40; ++i) iwram[i] = 0;
    boot_calls = 0;
    Rtc_Initialize();
    ++boot_calls;
    Clock_ClearState();
    ++boot_calls;
    return 0;
}

int port_boot_copy_iwram(void)
{
    const uint8_t *rom;
    uint8_t *iwram;
    unsigned i;
    if (port_rom_size() < 0x145c) return -1;
    rom = port_memory_pointer(PORT_ROM_BASE + 0x350, 0x110c);
    iwram = port_memory_pointer(PORT_IWRAM_BASE, 0x110c);
    if (!rom || !iwram) return -1;
    for (i = 0; i < 0x110c; ++i) iwram[i] = rom[i];
    return 0;
}

unsigned port_boot_call_count(void) { return boot_calls; }
