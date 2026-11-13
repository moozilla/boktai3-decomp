/* Included into a private build copy of tools/emu/harness.c by ppu_regression.py.
 * Keep the normal harness untouched; its commands and deterministic RTC remain
 * the capture protocol. The only hooks observe renderer inputs and register
 * writes, then forward to mGBA's original software renderer. */
#include <mgba/internal/gba/gba.h>
#include "ppu.h"

static void (*ppu_original_draw)(struct GBAVideoRenderer *, int);
static uint16_t (*ppu_original_write)(struct GBAVideoRenderer *, uint32_t, uint16_t);
static Ppu ppu_capture_state;
static uint32_t ppu_capture_frame[PPU_WIDTH * PPU_HEIGHT];
static unsigned ppu_capture_lines;
#define PPU_RECORD_SIZE (16 + PPU_IO_SIZE + PPU_PALETTE_SIZE + PPU_VRAM_SIZE + PPU_OAM_SIZE)
static uint8_t ppu_capture_records[PPU_RECORD_SIZE * PPU_HEIGHT];

static PpuMemory ppu_capture_memory(void)
{
    struct GBA *gba = core->board;
    PpuMemory m = {(const uint8_t *)gba->memory.io,
                   (const uint8_t *)gba->video.palette,
                   (const uint8_t *)gba->video.vram,
                   (const uint8_t *)gba->video.oam.raw};
    return m;
}

static void ppu_record32(uint8_t *out, int32_t value)
{
    unsigned i;
    for (i = 0; i < 4; ++i) out[i] = (uint32_t)value >> (i * 8);
}

static void ppu_capture_draw(struct GBAVideoRenderer *renderer, int y)
{
    PpuMemory m = ppu_capture_memory();
    uint8_t *record = ppu_capture_records + y * PPU_RECORD_SIZE;
    if (y == 0) {
        ppu_begin_frame(&ppu_capture_state, &m);
        ppu_capture_lines = 0;
    }
    ppu_record32(record, ppu_capture_state.affine_x[0]);
    ppu_record32(record + 4, ppu_capture_state.affine_y[0]);
    ppu_record32(record + 8, ppu_capture_state.affine_x[1]);
    ppu_record32(record + 12, ppu_capture_state.affine_y[1]);
    record += 16;
    memcpy(record, m.io, PPU_IO_SIZE); record += PPU_IO_SIZE;
    memcpy(record, m.palette, PPU_PALETTE_SIZE); record += PPU_PALETTE_SIZE;
    memcpy(record, m.vram, PPU_VRAM_SIZE); record += PPU_VRAM_SIZE;
    memcpy(record, m.oam, PPU_OAM_SIZE);
    ppu_render_scanline_rgb888(&ppu_capture_state, &m, (unsigned)y, ppu_capture_frame + y * PPU_WIDTH);
    ++ppu_capture_lines;
    ppu_original_draw(renderer, y);
}

static uint16_t ppu_capture_write(struct GBAVideoRenderer *renderer, uint32_t address, uint16_t value)
{
    uint16_t result = ppu_original_write(renderer, address, value);
    /* mGBA calls writeVideoRegister before updating the IO backing array. */
    if (address >= 0x28 && address < 0x40 && (address & 15) >= 8) {
        PpuMemory m = ppu_capture_memory();
        uint8_t io[PPU_IO_SIZE];
        memcpy(io, m.io, sizeof(io));
        io[address] = value;
        io[address + 1] = value >> 8;
        m.io = io;
        ppu_write_register(&ppu_capture_state, &m, address, 2);
    }
    return result;
}

static void ppu_capture_install(void)
{
    struct GBA *gba = core->board;
    struct GBAVideoRenderer *renderer = gba->video.renderer;
    ppu_original_draw = renderer->drawScanline;
    ppu_original_write = renderer->writeVideoRegister;
    renderer->drawScanline = ppu_capture_draw;
    renderer->writeVideoRegister = ppu_capture_write;
}

static void ppu_capture_shot(const char *name)
{
    char path[1024];
    FILE *out;
    unsigned i, mismatches = 0;
    if (ppu_capture_lines != PPU_HEIGHT) {
        fprintf(stderr, "PPU capture %s is incomplete (%u lines)\n", name, ppu_capture_lines);
        exit(1);
    }
    snprintf(path, sizeof(path), "%s/%s.ppu", outdir, name);
    out = fopen(path, "wb");
    if (!out) { perror(path); exit(1); }
    if (fwrite("PPUDUMP1", 1, 8, out) != 8 ||
        fwrite(ppu_capture_records, 1, sizeof(ppu_capture_records), out) != sizeof(ppu_capture_records)) {
        perror(path); exit(1);
    }
    fclose(out);
    snprintf(path, sizeof(path), "%s/%s.port.ppm", outdir, name);
    out = fopen(path, "wb");
    if (!out) { perror(path); exit(1); }
    fprintf(out, "P6\n240 160\n255\n");
    for (i = 0; i < PPU_WIDTH * PPU_HEIGHT; ++i) {
        uint32_t color = ppu_capture_frame[i];
        uint8_t rgb[3] = {color, color >> 8, color >> 16};
        if ((fb[i] & 0xffffff) != color) ++mismatches;
        fwrite(rgb, 1, 3, out);
    }
    fclose(out);
    fprintf(stderr, "PPU %s: %u / 38400 pixel mismatches\n", name, mismatches);
}
