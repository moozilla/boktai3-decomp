#include "platform.h"
#include "devices.h"
#include "ppu.h"

static Ppu ppu;
static PpuMemory memory;
static uint16_t framebuffer[PPU_WIDTH * PPU_HEIGHT];
static void render(void *context, unsigned line)
{
    (void)context;
    if (line == 0) ppu_begin_frame(&ppu, &memory);
    ppu_render_scanline(&ppu, &memory, line, framebuffer + line * PPU_WIDTH);
}
static void write_register(void *context, unsigned offset, unsigned width)
{
    (void)context;
    ppu_write_register(&ppu, &memory, offset, width);
}
int port_video_init(void)
{
    memory.io = port_memory_pointer(PORT_IO_BASE, PORT_IO_SIZE);
    memory.palette = port_memory_pointer(PORT_PALETTE_BASE, PORT_PALETTE_SIZE);
    memory.vram = port_memory_pointer(PORT_VRAM_BASE, PORT_VRAM_SIZE);
    memory.oam = port_memory_pointer(PORT_OAM_BASE, PORT_OAM_SIZE);
    if (!memory.io || !memory.palette || !memory.vram || !memory.oam) return -1;
    port_devices_set_renderer(render, write_register, NULL);
    return 0;
}
const uint16_t *port_video_framebuffer(void) { return framebuffer; }
