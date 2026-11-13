#ifndef BOKTAI_PORT_PLATFORM_H
#define BOKTAI_PORT_PLATFORM_H
#include <stddef.h>
#include <stdint.h>

/* Shared by the core, devices and renderer. Addresses are GBA byte addresses,
 * not offsets into a separate host array. wasm32 pointers retain the GBA ABI. */
#define PORT_EWRAM_BASE UINT32_C(0x02000000)
#define PORT_EWRAM_SIZE UINT32_C(0x00040000)
#define PORT_IWRAM_BASE UINT32_C(0x03000000)
#define PORT_IWRAM_SIZE UINT32_C(0x00008000)
#define PORT_IO_BASE UINT32_C(0x04000000)
#define PORT_IO_SIZE UINT32_C(0x00000400)
#define PORT_PALETTE_BASE UINT32_C(0x05000000)
#define PORT_PALETTE_SIZE UINT32_C(0x00000400)
#define PORT_VRAM_BASE UINT32_C(0x06000000)
#define PORT_VRAM_SIZE UINT32_C(0x00018000)
#define PORT_OAM_BASE UINT32_C(0x07000000)
#define PORT_OAM_SIZE UINT32_C(0x00000400)
#define PORT_ROM_BASE UINT32_C(0x08000000)
#define PORT_ROM_CAPACITY UINT32_C(0x02000000)
#define PORT_MEMORY_END (PORT_ROM_BASE + PORT_ROM_CAPACITY)
#define PORT_SCANLINES 228u
#define PORT_VISIBLE_LINES 160u
#define PORT_CYCLES_PER_LINE 1232u
#define PORT_HDRAW_CYCLES 960u

/* Return 0 on success. Host initialization maps only the seven regions, without
 * replacing existing mappings; wasm requires at least PORT_MEMORY_END bytes. */
int port_memory_init(void);
void port_memory_destroy(void);
void port_memory_reset(void);
void *port_memory_pointer(uint32_t address, size_t length);
int port_load_rom(const void *bytes, size_t length);
size_t port_rom_size(void);

/* Hooks remain owned by the caller. Core never retains a temporary hooks object.
 * advance runs devices for elapsed GBA cycles; scanline runs the PPU before
 * HBlank. resolve maps ARM ROM/IWRAM code addresses to callable portable code.
 * trace may log boot/IRQ calls. Unresolved code is an error, never executed. */
typedef void (*PortIrqHandler)(void);
struct PortPlatformHooks {
    void *context;
    void (*advance)(void *context, uint32_t cycles);
    void (*scanline)(void *context, unsigned line);
    PortIrqHandler (*resolve)(void *context, uint32_t gba_address);
    void (*trace)(void *context, const char *event, uint32_t value);
    void (*blank)(void *context, unsigned line, int vblank);
    void (*acknowledge)(void *context, uint16_t mask);
    /* Optional: call a handler by its GBA code address (full-game link).
     * Takes precedence over resolve; returns 0 on success. */
    int (*invoke)(void *context, uint32_t gba_address);
};
void port_set_hooks(const struct PortPlatformHooks *hooks);
void port_irq_raise(uint16_t mask);
int port_irq_dispatch(void);
int port_run_scanline(void);
int port_run_frame(void);
uint64_t port_frame_count(void);
/* Platform replacement for hardware busy waits. Advances to the next VBlank;
 * returns an error if interrupts cannot deliver it. Does not spin forever. */
int port_wait_vblank(void);
void port_timing_reset(void);
/* Advance scanlines until at least one interrupt has been serviced (a Halt),
 * at most two frames. Used when game code spins on memory an IRQ changes. */
int port_wait_interrupt(void);
unsigned port_current_line(void);
int port_spin_advance(void);

#endif
