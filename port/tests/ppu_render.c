/* Replay a scanline capture with no mGBA dependency. */
#include "ppu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint8_t record[16 + PPU_IO_SIZE + PPU_PALETTE_SIZE + PPU_VRAM_SIZE + PPU_OAM_SIZE];
static int32_t read32(const uint8_t *p)
{
    uint32_t n = p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
    return (int32_t)(n ^ 0x80000000) + (int64_t)INT32_MIN;
}

int main(int argc, char **argv)
{
    FILE *in = stdin, *out = stdout;
    char magic[8];
    unsigned y, x;
    Ppu ppu;
    PpuMemory m = {record + 16, record + 16 + PPU_IO_SIZE,
                   record + 16 + PPU_IO_SIZE + PPU_PALETTE_SIZE,
                   record + 16 + PPU_IO_SIZE + PPU_PALETTE_SIZE + PPU_VRAM_SIZE};
    if (argc > 3) { fprintf(stderr, "usage: ppu_render [dump.ppu [frame.ppm]]\n"); return 1; }
    if (argc > 1 && !(in = fopen(argv[1], "rb"))) { perror(argv[1]); return 1; }
    if (argc > 2 && !(out = fopen(argv[2], "wb"))) { perror(argv[2]); return 1; }
    if (fread(magic, 1, 8, in) != 8 || memcmp(magic, "PPUDUMP1", 8)) {
        fprintf(stderr, "invalid PPU capture\n"); return 1;
    }
    fprintf(out, "P6\n240 160\n255\n");
    for (y = 0; y < PPU_HEIGHT; ++y) {
        uint32_t pixels[PPU_WIDTH];
        if (fread(record, 1, sizeof(record), in) != sizeof(record)) {
            fprintf(stderr, "short PPU capture at line %u\n", y); return 1;
        }
        ppu.affine_x[0] = read32(record);
        ppu.affine_y[0] = read32(record + 4);
        ppu.affine_x[1] = read32(record + 8);
        ppu.affine_y[1] = read32(record + 12);
        ppu_render_scanline_rgb888(&ppu, &m, y, pixels);
        for (x = 0; x < PPU_WIDTH; ++x) {
            uint32_t color = pixels[x];
            uint8_t rgb[3] = {color, color >> 8, color >> 16};
            fwrite(rgb, 1, 3, out);
        }
    }
    if (fgetc(in) != EOF) { fprintf(stderr, "trailing PPU data\n"); return 1; }
    if (ferror(out)) { fprintf(stderr, "PPU output failed\n"); return 1; }
    if (in != stdin) fclose(in);
    if (out != stdout) fclose(out);
    return 0;
}
