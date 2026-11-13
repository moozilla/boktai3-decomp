/* Contract tests for raw RGB555 output, state, byte order and unaligned inputs.
 * Differential presentation tests live in ppu_synthetic.py. */
#include "ppu.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static uint8_t io_data[PPU_IO_SIZE + 1], pal_data[PPU_PALETTE_SIZE + 1];
static uint8_t vram_data[PPU_VRAM_SIZE + 1], oam_data[PPU_OAM_SIZE + 1];
static PpuMemory m = {io_data + 1, pal_data + 1, vram_data + 1, oam_data + 1};
static uint16_t output[PPU_WIDTH * PPU_HEIGHT];
static uint32_t rgb[PPU_WIDTH];
static Ppu ppu;

static void write16(const uint8_t *data, unsigned address, unsigned value)
{
    uint8_t *p = (uint8_t *)data;
    p[address] = value;
    p[address + 1] = value >> 8;
}

static void write32(const uint8_t *data, unsigned address, uint32_t value)
{
    write16(data, address, value);
    write16(data, address + 2, value >> 16);
}

static void reset(void)
{
    unsigned i;
    memset(io_data, 0, sizeof(io_data));
    memset(pal_data, 0, sizeof(pal_data));
    memset(vram_data, 0, sizeof(vram_data));
    memset(oam_data, 0, sizeof(oam_data));
    for (i = 0; i < 128; ++i) write16(m.oam, i * 8, 0x200);
    ppu_begin_frame(&ppu, &m);
}

static void solid_backgrounds(void)
{
    reset();
    write16(m.io, 0, 0x300);
    write16(m.io, 8, 0x1000);
    write16(m.io, 10, 0x1105);
    memset((uint8_t *)m.vram, 0x11, 32);
    memset((uint8_t *)m.vram + 0x4000, 0x22, 32);
    write16(m.palette, 2, 31);
    write16(m.palette, 4, 31 << 10);
}

int main(void)
{
    unsigned i;
    reset();
    write16(m.io, 0, 0x80);
    ppu_render_frame(&ppu, &m, output);
    for (i = 0; i < PPU_WIDTH * PPU_HEIGHT; ++i) assert(output[i] == 0x7fff);
    assert(ppu_rgb888(0x7fff) == 0xffffff);
    assert(ppu_rgb888(0x001f) == 0x0000ff);
    assert(ppu_rgb888(0x03e0) == 0x00ff00);
    assert(ppu_rgb888(0x7c00) == 0xff0000);
    reset();
    write16(m.palette, 0, 0x9234);
    ppu_render_scanline(&ppu, &m, 0, output);
    assert(output[0] == 0x1234);
    output[0] = 0x4567;
    ppu_render_scanline(&ppu, &m, 160, output);
    assert(output[0] == 0x4567 && ppu.next_line == 1);

    solid_backgrounds();
    ppu_render_scanline(&ppu, &m, 0, output);
    assert(output[0] == 31 && output[239] == 31);
    write16(m.io, 0x50, 0x241);
    write16(m.io, 0x52, 0x0808);
    ppu_render_scanline(&ppu, &m, 1, output);
    assert(output[0] == (15 | (15 << 10)));
    ppu_render_scanline_rgb888(&ppu, &m, 2, rgb);
    assert(rgb[0] == 0x7f007f);
    write16(m.io, 0x52, 0x1f1f);
    ppu_render_scanline(&ppu, &m, 3, output);
    assert(output[0] == (31 | (31 << 10)));
    write16(m.io, 0x50, 0x81);
    write16(m.io, 0x54, 16);
    ppu_render_scanline(&ppu, &m, 4, output);
    assert(output[0] == 0x7fff);
    write16(m.io, 0x50, 0xc1);
    ppu_render_scanline(&ppu, &m, 5, output);
    assert(output[0] == 0);
    /* WIN0 enables BG0 but disables color effects; outside is backdrop. */
    write16(m.io, 0, 0x2300);
    write16(m.io, 0x40, 0x1464);
    write16(m.io, 0x44, 0x00a0);
    write16(m.io, 0x48, 1);
    ppu_render_scanline(&ppu, &m, 6, output);
    assert(output[19] == 0 && output[20] == 31 && output[99] == 31 && output[100] == 0);

    reset();
    write32(m.io, 0x28, 0x08000000);
    write32(m.io, 0x2c, 0x07ffffff);
    write32(m.io, 0x38, 0x0fffff00);
    ppu_begin_frame(&ppu, &m);
    assert(ppu.affine_x[0] == -0x08000000);
    assert(ppu.affine_y[0] == 0x07ffffff);
    assert(ppu.affine_x[1] == -256);
    write16(m.io, 0x26, 1);
    ppu_render_scanline(&ppu, &m, 0, output);
    assert(ppu.affine_y[0] == -0x08000000);
    write16(m.io, 0x28, 1234);
    ppu_write_register(&ppu, &m, 0x28, 2);
    assert(ppu.affine_x[0] == -0x08000000 + 1234);
    write16(m.io, 0x2a, 0);
    ppu_write_register(&ppu, &m, 0x2a, 2);
    assert(ppu.affine_x[0] == 1234);
    write32(m.io, 0x38, 0x1234567);
    ppu_write_register(&ppu, &m, 0x38, 4);
    assert(ppu.affine_x[1] == 0x1234567);
    ppu_write_register(&ppu, &m, 0x400, 4);
    ppu_write_register(&ppu, &m, 0x3ff, 4);
    assert(ppu.affine_x[1] == 0x1234567);

    /* An odd 8bpp OBJ tile can straddle the end of OBJ VRAM. */
    reset();
    write16(m.io, 0, 0x1040);
    write16(m.oam, 0, 0x2000);
    write16(m.oam, 2, 0xc000);
    write16(m.oam, 4, 1023);
    memset((uint8_t *)m.vram + 0x10000, 1, 0x8000);
    write16(m.palette, 514, 0x5432);
    ppu_render_frame(&ppu, &m, output);
    assert(output[0] == 0x5432);
    assert(output[63 + 63 * PPU_WIDTH] == 0x5432);
    puts("PPU RGB555/state/unaligned memory contracts: PASS");
    return 0;
}
