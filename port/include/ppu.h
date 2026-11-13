#ifndef BOKTAI_PORT_PPU_H
#define BOKTAI_PORT_PPU_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PPU_WIDTH 240
#define PPU_HEIGHT 160
#define PPU_IO_SIZE 0x400
#define PPU_PALETTE_SIZE 0x400
#define PPU_VRAM_SIZE 0x18000
#define PPU_OAM_SIZE 0x400

/* Byte arrays in GBA little-endian order. No alignment requirement. */
typedef struct PpuMemory {
    const uint8_t *io;
    const uint8_t *palette;
    const uint8_t *vram;
    const uint8_t *oam;
} PpuMemory;

typedef struct Ppu {
    /* BG2/BG3 internal reference points, signed 20.8 fixed point. */
    int32_t affine_x[2];
    int32_t affine_y[2];
    unsigned next_line;
} Ppu;

/* Call at the beginning of visible line 0, after VBlank register writes. */
void ppu_begin_frame(Ppu *ppu, const PpuMemory *memory);
/* Call after writing either half of BG2X/Y or BG3X/Y to memory->io.
 * offset is relative to 0x04000000; width is 1, 2 or 4 bytes.
 * Other register writes need no notification: scanlines read the current IO. */
void ppu_write_register(Ppu *ppu, const PpuMemory *memory, unsigned offset, unsigned width);
/* Render visible lines in ascending order. Apply HBlank writes before the next
 * call. Output is GBA RGB555 (red in bits 0..4), bit 15 clear. No LCD filter.
 * Affine PB/PD advance even when a BG is disabled or forced blank is active. */
void ppu_render_scanline(Ppu *ppu, const PpuMemory *memory, unsigned y,
                         uint16_t pixels[PPU_WIDTH]);
/* Convenience for a frame with constant memory/registers (no raster effects). */
void ppu_render_frame(Ppu *ppu, const PpuMemory *memory,
                      uint16_t pixels[PPU_WIDTH * PPU_HEIGHT]);
/* mGBA software-renderer presentation: 0x00BBGGRR pixels. This path expands
 * palette channels before color effects, matching mGBA 0.11 screenshots.
 * RGB555 above retains the GBA's 5-bit color-effect arithmetic. */
void ppu_render_scanline_rgb888(Ppu *ppu, const PpuMemory *memory, unsigned y,
                                uint32_t pixels[PPU_WIDTH]);
void ppu_render_frame_rgb888(Ppu *ppu, const PpuMemory *memory,
                             uint32_t pixels[PPU_WIDTH * PPU_HEIGHT]);
/* Expand each 5-bit channel to 8 bits, replicating its high bits (mGBA 0.11). */
uint32_t ppu_rgb888(uint16_t color);

#ifdef __cplusplus
}
#endif
#endif
