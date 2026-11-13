#include "ppu.h"

/* Renderer state is entirely owned by the caller. All memory reads are bytes:
 * the same code accepts live wasm memory and unaligned host-side dump files. */
static unsigned read16(const uint8_t *p, unsigned address)
{
    return p[address] | ((unsigned)p[address + 1] << 8);
}

static int32_t reference(const uint8_t *io, unsigned address)
{
    uint32_t n = read16(io, address) | (read16(io, address + 2) << 16);
    n &= 0x0fffffff;
    return (int32_t)(n ^ 0x08000000) - 0x08000000;
}

/* Defined arithmetic right shift, including on hosts that shift negatives
 * toward zero. Coordinate intermediates fit in int32_t. */
static int floor256(int32_t n)
{
    return n >= 0 ? n / 256 : -1 - (int)((-(int64_t)n - 1) / 256);
}

static int32_t wrap_reference(int64_t n)
{
    uint32_t bits = (uint32_t)n & 0x0fffffff;
    return (int32_t)(bits ^ 0x08000000) - 0x08000000;
}

void ppu_begin_frame(Ppu *ppu, const PpuMemory *m)
{
    unsigned i;
    for (i = 0; i < 2; ++i) {
        ppu->affine_x[i] = reference(m->io, 0x28 + i * 16);
        unsigned bounds = read16(m->io, 0x44 + i * 2);
        unsigned start = bounds >> 8, end = bounds & 255;
        ppu->affine_y[i] = reference(m->io, 0x2c + i * 16);
        /* Vertical window comparison runs through VBlank (228 lines). */
        ppu->window_active[i] = start < 228 && (end < start || end >= 228);
    }
    ppu->next_line = 0;
}

void ppu_write_register(Ppu *ppu, const PpuMemory *m, unsigned offset, unsigned width)
{
    unsigned i;
    if (offset >= PPU_IO_SIZE || width > PPU_IO_SIZE - offset)
        return;
    for (i = 0; i < 2; ++i) {
        unsigned base = 0x28 + i * 16;
        if (offset < base + 4 && offset + width > base)
            ppu->affine_x[i] = reference(m->io, base);
        if (offset < base + 8 && offset + width > base + 4)
            ppu->affine_y[i] = reference(m->io, base + 4);
    }
}

typedef struct Pixel {
    uint16_t color;
    uint8_t layer;
    uint8_t priority;
    uint8_t semi;
    uint8_t visible;
} Pixel;

static Pixel empty_pixel(void)
{
    Pixel p = {0, 0, 0, 0, 0};
    return p;
}

static uint16_t palette(const PpuMemory *m, unsigned n)
{
    return (uint16_t)(read16(m->palette, n * 2) & 0x7fff);
}

static uint8_t bg_byte(const PpuMemory *m, unsigned address)
{
    /* Background tile fetches cannot reach OBJ VRAM. */
    return m->vram[address & 0xffff];
}

static Pixel text_bg(const PpuMemory *m, unsigned bg, unsigned x, unsigned y, unsigned presentation)
{
    unsigned control = read16(m->io, 8 + bg * 2);
    unsigned size = control >> 14;
    unsigned width = (size & 1) ? 512 : 256;
    unsigned height = (size & 2) ? 512 : 256;
    unsigned sx, sy, block, map, entry, tile, index, tx, ty;
    Pixel p = empty_pixel();
    if (control & 0x40) {
        unsigned mosaic = read16(m->io, 0x4c);
        x -= x % ((mosaic & 15) + 1);
        y -= y % (((mosaic >> 4) & 15) + 1);
    }
    sx = (x + read16(m->io, 0x10 + bg * 4)) & (width - 1);
    sy = (y + read16(m->io, 0x12 + bg * 4)) & (height - 1);
    block = sx / 256 + (sy / 256) * (width / 256);
    map = ((control >> 8) & 31) * 0x800 + block * 0x800;
    map += (((sy & 255) / 8) * 32 + (sx & 255) / 8) * 2;
    entry = bg_byte(m, map) | ((unsigned)bg_byte(m, map + 1) << 8);
    tx = sx & 7;
    ty = sy & 7;
    if (entry & 0x400) tx = 7 - tx;
    if (entry & 0x800) ty = 7 - ty;
    tile = ((control >> 2) & 3) * 0x4000;
    if (control & 0x80) {
        tile += (entry & 1023) * 64 + ty * 8 + tx;
        if (tile >= 0x10000) return p;
        index = m->vram[tile];
    } else {
        tile += (entry & 1023) * 32 + ty * 4 + tx / 2;
        if (tile >= 0x10000) {
            unsigned scroll = read16(m->io, 0x10 + bg * 4) & 7;
            unsigned prefix = (8 - scroll) & 7;
            unsigned suffix = (PPU_WIDTH + scroll) & 7;
            /* mGBA's RGB888 path fetches clipped 4bpp edge tiles from the
             * adjoining OBJ bank, but rejects whole tiles outside BG VRAM. */
            if (!presentation || (control & 0x40) ||
                (x >= prefix && x < PPU_WIDTH - suffix)) return p;
        }
        index = (m->vram[tile] >> ((tx & 1) * 4)) & 15;
        if (index) index += (entry >> 12) * 16;
    }
    if (index) {
        p.color = palette(m, index);
        p.layer = (uint8_t)bg;
        p.priority = control & 3;
        p.visible = 1;
    }
    return p;
}

static Pixel affine_bg(const Ppu *ppu, const PpuMemory *m, unsigned bg,
                       unsigned mode, unsigned x, unsigned y, unsigned presentation)
{
    unsigned control = read16(m->io, 8 + bg * 2);
    unsigned reg = 0x20 + (bg - 2) * 16;
    int dx = (int16_t)read16(m->io, reg);
    int dy = (int16_t)read16(m->io, reg + 4);
    int32_t rx = ppu->affine_x[bg - 2], ry = ppu->affine_y[bg - 2];
    int sx, sy;
    unsigned index;
    Pixel p = empty_pixel();
    if (control & 0x40) {
        unsigned mosaic = read16(m->io, 0x4c);
        unsigned back = y % (((mosaic >> 4) & 15) + 1);
        unsigned horizontal = (mosaic & 15) + 1;
        rx -= (int16_t)read16(m->io, reg + 2) * (int)back;
        ry -= (int16_t)read16(m->io, reg + 6) * (int)back;
        if (presentation && mode >= 3 && horizontal > 1) {
            /* The software oracle starts a bitmap mosaic run at the first
             * in-bounds sample, retaining that sample through its block. */
            unsigned first = 0;
            int width = mode == 5 ? 160 : 240, height = mode == 5 ? 128 : 160;
            while (first <= x) {
                int px = floor256(rx + dx * (int)first);
                int py = floor256(ry + dy * (int)first);
                if (px >= 0 && py >= 0 && px < width && py < height) break;
                ++first;
            }
            if (first > x) return p;
            x = first + (x - first) / horizontal * horizontal;
        } else {
            x -= x % horizontal;
        }
    }
    sx = floor256(rx + dx * (int)x);
    sy = floor256(ry + dy * (int)x);
    if (mode < 3) {
        unsigned size = 128u << (control >> 14);
        unsigned tile, address;
        if (!(control & 0x2000) && (sx < 0 || sy < 0 || sx >= (int)size || sy >= (int)size))
            return p;
        sx &= size - 1;
        sy &= size - 1;
        address = ((control >> 8) & 31) * 0x800 + (unsigned)(sy / 8) * (size / 8) + (unsigned)sx / 8;
        tile = bg_byte(m, address);
        address = ((control >> 2) & 3) * 0x4000 + tile * 64 + (sy & 7) * 8 + (sx & 7);
        index = bg_byte(m, address);
        if (!index) return p;
        p.color = palette(m, index);
    } else {
        unsigned width = mode == 5 ? 160 : 240;
        unsigned height = mode == 5 ? 128 : 160;
        unsigned address;
        if (sx < 0 || sy < 0 || sx >= (int)width || sy >= (int)height) return p;
        address = (unsigned)sy * width + (unsigned)sx;
        if (mode == 4) {
            if (read16(m->io, 0) & 16) address += 0xa000;
            index = m->vram[address];
            if (!index) return p;
            p.color = palette(m, index);
        } else {
            address *= 2;
            if (mode == 5 && (read16(m->io, 0) & 16)) address += 0xa000;
            p.color = read16(m->vram, address) & 0x7fff;
        }
    }
    p.layer = (uint8_t)bg;
    p.priority = control & 3;
    p.visible = 1;
    return p;
}

static int inside(unsigned position, unsigned bounds)
{
    unsigned start = bounds >> 8, end = bounds & 255;
    return start <= end ? position >= start && position < end : position >= start || position < end;
}

static unsigned window_mask(const Ppu *ppu, const PpuMemory *m, unsigned display, unsigned x, unsigned objwin)
{
    unsigned i;
    if (!(display & 0xe000)) return 63;
    for (i = 0; i < 2; ++i) {
        if ((display & (0x2000 << i)) && ppu->window_active[i] &&
            inside(x, read16(m->io, 0x40 + i * 2)))
            return m->io[0x48 + i] & 63;
    }
    return m->io[0x4a + ((display & 0x8000) && objwin ? 1 : 0)] & 63;
}

static void sprites(const PpuMemory *m, unsigned display, unsigned y,
                     Pixel pixels[PPU_WIDTH], uint8_t objwin[PPU_WIDTH])
{
    static const uint8_t sizes[3][4][2] = {
        {{8, 8}, {16, 16}, {32, 32}, {64, 64}},
        {{16, 8}, {32, 8}, {32, 16}, {64, 32}},
        {{8, 16}, {8, 32}, {16, 32}, {32, 64}}
    };
    unsigned obj;
    int cycles = display & 32 ? 954 : 1210;
    for (obj = 0; obj < 128; ++obj) {
        unsigned a = read16(m->oam, obj * 8);
        unsigned b = read16(m->oam, obj * 8 + 2);
        unsigned c = read16(m->oam, obj * 8 + 4);
        unsigned shape = a >> 14, size = b >> 14;
        unsigned affine = a & 0x100, double_size = affine && (a & 0x200);
        unsigned kind = (a >> 10) & 3;
        unsigned width, height, boxw, boxh, row, draw_width;
        int left, column, mosaic_origin, pa = 256, pb = 0, pc = 0, pd = 256;
        cycles -= 2;
        if (cycles <= 0) return;
        if (shape == 3 || kind == 3 || (!affine && (a & 0x200))) continue;
        width = sizes[shape][size][0];
        height = sizes[shape][size][1];
        boxw = width << double_size;
        boxh = height << double_size;
        row = (y - (a & 255)) & 255;
        if (row >= boxh) continue;
        left = b & 511;
        if (left >= 256) left -= 512;
        if (left >= PPU_WIDTH || left + (int)boxw <= 0) continue;
        if (affine) {
            unsigned matrix = ((b >> 9) & 31) * 32;
            pa = (int16_t)read16(m->oam, matrix + 6);
            pb = (int16_t)read16(m->oam, matrix + 14);
            pc = (int16_t)read16(m->oam, matrix + 22);
            pd = (int16_t)read16(m->oam, matrix + 30);
            cycles -= 8 + (int)boxw * 2 + (left < 0 ? left : 0);
        } else {
            cycles -= (int)boxw - 2 + (left < 0 ? floor256(left * 128) : 0);
        }
        mosaic_origin = left - 1;
        if (affine && (a & 0x1000)) {
            unsigned mosaic = read16(m->io, 0x4c);
            int oy = (int)row - (int)(y % (((mosaic >> 12) & 15) + 1));
            int first = left > 0 ? left : 0;
            if (oy < 0) oy = 0;
            for (; first < left + (int)boxw && first < PPU_WIDTH; ++first) {
                int cx = first - left - (int)boxw / 2, cy = oy - (int)boxh / 2;
                int sx = floor256(pa * cx + pb * cy + (int)width * 128);
                int sy = floor256(pc * cx + pd * cy + (int)height * 128);
                if (sx >= 0 && sy >= 0 && sx < (int)width && sy < (int)height) break;
            }
            mosaic_origin = first - 1;
        }
        draw_width = boxw;
        if (a & 0x1000) {
            unsigned mosaic = read16(m->io, 0x4c);
            unsigned horizontal = ((mosaic >> 8) & 15) + 1;
            int right = left + (int)boxw;
            if (right > 0 && right < PPU_WIDTH && right % (int)horizontal)
                draw_width += horizontal - right % (int)horizontal;
        }
        for (column = 0; column < (int)draw_width; ++column) {
            int x = left + column;
            int ox = column, oy = (int)row, sx, sy;
            unsigned tile, address, index, priority = (c >> 10) & 3;
            unsigned bpp8 = (a >> 13) & 1;
            if (x < 0 || x >= PPU_WIDTH) continue;
            if (a & 0x1000) {
                unsigned mosaic = read16(m->io, 0x4c);
                /* OBJ horizontal mosaic is aligned to screen coordinates. */
                ox -= x % (((mosaic >> 8) & 15) + 1);
                if (affine && ox + left < mosaic_origin) ox = mosaic_origin - left;
                if (affine && x <= mosaic_origin) continue;
                oy -= y % (((mosaic >> 12) & 15) + 1);
                if (oy < 0) oy = 0;
                if (!affine) {
                    if (ox < 0) ox = 0;
                    if (ox >= (int)width) ox = (int)width - 1;
                }
            }
            if (affine) {
                int cx = ox - (int)boxw / 2, cy = oy - (int)boxh / 2;
                sx = floor256(pa * cx + pb * cy + (int)width * 128);
                sy = floor256(pc * cx + pd * cy + (int)height * 128);
            } else {
                sx = (b & 0x1000) ? (int)width - 1 - ox : ox;
                sy = (b & 0x2000) ? (int)height - 1 - oy : oy;
            }
            if (sx < 0 || sy < 0 || sx >= (int)width || sy >= (int)height) continue;
            tile = c & 1023;
            if (display & 64) {
                tile += (unsigned)(sy / 8) * ((width / 8) << bpp8);
                tile += (unsigned)(sx / 8) << bpp8;
            } else {
                if (bpp8) tile &= ~1u;
                tile = (tile & ~31u) + (unsigned)(sy / 8) * 32 +
                       ((tile + ((unsigned)(sx / 8) << bpp8)) & 31);
            }
            tile &= 1023;
            if ((display & 7) >= 3 && tile < 512) continue;
            address = 0x10000 + tile * 32 + (sy & 7) * (bpp8 ? 8 : 4);
            address += bpp8 ? (sx & 7) : (sx & 7) / 2;
            address = 0x10000 + ((address - 0x10000) & 0x7fff);
            index = m->vram[address];
            if (!bpp8) index = (index >> ((sx & 1) * 4)) & 15;
            if (!index) {
                /* A transparent higher-priority OBJ still supplies priority
                 * and blend attributes for an earlier OBJ pixel underneath. */
                if (pixels[x].visible && pixels[x].priority > priority) {
                    pixels[x].priority = (uint8_t)priority;
                    pixels[x].semi = kind == 1;
                }
                continue;
            }
            if (kind == 2) {
                objwin[x] = 1;
                continue;
            }
            if (pixels[x].visible && pixels[x].priority <= priority) continue;
            if (!bpp8) index += (c >> 12) * 16;
            pixels[x].color = palette(m, 256 + index);
            pixels[x].layer = 4;
            pixels[x].priority = (uint8_t)priority;
            pixels[x].visible = 1;
            pixels[x].semi = kind == 1;
        }
    }
}

static int before(Pixel a, Pixel b)
{
    if (!b.visible) return 1;
    if (a.priority != b.priority) return a.priority < b.priority;
    /* OBJ precedes BG at equal priority; BG number resolves BG ties. */
    if (a.layer == 4) return 1;
    if (b.layer == 4) return 0;
    return a.layer < b.layer;
}

static void insert(Pixel p, Pixel *top, Pixel *second)
{
    if (!p.visible) return;
    if (before(p, *top)) {
        *second = *top;
        *top = p;
    } else if (before(p, *second)) {
        *second = p;
    }
}

static uint32_t blend(uint32_t a, uint32_t b, unsigned eva, unsigned evb, unsigned bits)
{
    unsigned channel, out = 0;
    for (channel = 0; channel < bits * 3; channel += bits) {
        unsigned maximum = (1u << bits) - 1;
        unsigned value = (((a >> channel) & maximum) * eva + ((b >> channel) & maximum) * evb) >> 4;
        if (value > maximum) value = maximum;
        out |= value << channel;
    }
    return out;
}

static uint32_t brightness(uint32_t color, unsigned amount, unsigned effect, unsigned bits)
{
    unsigned channel, out = 0;
    for (channel = 0; channel < bits * 3; channel += bits) {
        unsigned maximum = (1u << bits) - 1;
        unsigned value = (color >> channel) & maximum;
        if (effect == 2) value += ((maximum - value) * amount) >> 4;
        else if (bits == 8 && channel != 0) value = (value * (16 - amount)) >> 4;
        else value -= (value * amount) >> 4;
        out |= value << channel;
    }
    return out;
}

static void store_pixel(void *out, unsigned x, uint32_t color, unsigned bits)
{
    if (bits == 8) ((uint32_t *)out)[x] = color;
    else ((uint16_t *)out)[x] = (uint16_t)color;
}

static void render_scanline(Ppu *ppu, const PpuMemory *m, unsigned y, void *out, unsigned bits)
{
    unsigned display, mode, x, bg, i;
    Pixel objects[PPU_WIDTH];
    uint8_t objwin[PPU_WIDTH];
    if (y >= PPU_HEIGHT) return;
    display = read16(m->io, 0);
    mode = display & 7;
    for (i = 0; i < 2; ++i) {
        unsigned bounds = read16(m->io, 0x44 + i * 2);
        if (y == (bounds >> 8)) ppu->window_active[i] = 1;
        if (y == (bounds & 255)) ppu->window_active[i] = 0;
    }
    for (x = 0; x < PPU_WIDTH; ++x) {
        objects[x] = empty_pixel();
        objwin[x] = 0;
    }
    if ((display & 0x1000) && !(display & 0x80)) sprites(m, display, y, objects, objwin);
    for (x = 0; x < PPU_WIDTH; ++x) {
        Pixel top = {0, 5, 4, 0, 1}, second = {0, 5, 4, 0, 0};
        unsigned mask, control, effect, eva, evb, evy;
        uint32_t color, below;
        if (display & 0x80) {
            store_pixel(out, x, bits == 8 ? 0xffffff : 0x7fff, bits);
            continue;
        }
        top.color = palette(m, 0);
        mask = window_mask(ppu, m, display, x, objwin[x]);
        for (bg = 0; bg < 4; ++bg) {
            Pixel p;
            if (!(display & (0x100 << bg)) || !(mask & (1 << bg))) continue;
            if (mode == 0 || (mode == 1 && bg < 2)) p = text_bg(m, bg, x, y, bits == 8);
            else if ((mode == 1 && bg == 2) || (mode == 2 && bg >= 2) ||
                     (mode >= 3 && mode <= 5 && bg == 2)) p = affine_bg(ppu, m, bg, mode, x, y, bits == 8);
            else continue;
            insert(p, &top, &second);
        }
        if (mask & 16) insert(objects[x], &top, &second);
        control = read16(m->io, 0x50);
        effect = (control >> 6) & 3;
        eva = m->io[0x52] & 31;
        evb = m->io[0x53] & 31;
        evy = m->io[0x54] & 31;
        if (eva > 16) eva = 16;
        if (evb > 16) evb = 16;
        if (evy > 16) evy = 16;
        color = bits == 8 ? ppu_rgb888(top.color) : top.color;
        below = bits == 8 ? ppu_rgb888(second.color) : second.color;
        if (mask & 32) {
            if ((top.semi || (effect == 1 && (control & (1 << top.layer)))) &&
                second.visible && (control & (0x100 << second.layer)))
                color = blend(color, below, eva, evb, bits);
            else if ((effect == 2 || effect == 3) &&
                     ((control & (1 << top.layer)) ||
                      (bits == 8 && top.semi &&
                       (control & (0x2000 | (display & 0x0f00))))))
                color = brightness(color, evy, effect, bits);
        }
        store_pixel(out, x, color, bits);
    }
    if (read16(m->io, 2) & 1) {
        for (x = 0; x < PPU_WIDTH; x += 2) {
            unsigned mask = bits == 8 ? 0xff00 : 0x3e0;
            uint32_t a = bits == 8 ? ((uint32_t *)out)[x] : ((uint16_t *)out)[x];
            uint32_t b = bits == 8 ? ((uint32_t *)out)[x + 1] : ((uint16_t *)out)[x + 1];
            store_pixel(out, x, (a & ~mask) | (b & mask), bits);
            store_pixel(out, x + 1, (b & ~mask) | (a & mask), bits);
        }
    }
    for (i = 0; i < 2; ++i) {
        ppu->affine_x[i] = wrap_reference((int64_t)ppu->affine_x[i] + (int16_t)read16(m->io, 0x22 + i * 16));
        ppu->affine_y[i] = wrap_reference((int64_t)ppu->affine_y[i] + (int16_t)read16(m->io, 0x26 + i * 16));
    }
    ppu->next_line = y + 1;
}

void ppu_render_scanline(Ppu *ppu, const PpuMemory *m, unsigned y, uint16_t *out)
{
    render_scanline(ppu, m, y, out, 5);
}

void ppu_render_scanline_rgb888(Ppu *ppu, const PpuMemory *m, unsigned y, uint32_t *out)
{
    render_scanline(ppu, m, y, out, 8);
}

void ppu_render_frame_rgb888(Ppu *ppu, const PpuMemory *m, uint32_t *out)
{
    unsigned y;
    ppu_begin_frame(ppu, m);
    for (y = 0; y < PPU_HEIGHT; ++y) ppu_render_scanline_rgb888(ppu, m, y, out + y * PPU_WIDTH);
}

void ppu_render_frame(Ppu *ppu, const PpuMemory *m, uint16_t *out)
{
    unsigned y;
    ppu_begin_frame(ppu, m);
    for (y = 0; y < PPU_HEIGHT; ++y) ppu_render_scanline(ppu, m, y, out + y * PPU_WIDTH);
}

uint32_t ppu_rgb888(uint16_t color)
{
    unsigned r = color & 31, g = (color >> 5) & 31, b = (color >> 10) & 31;
    return ((r << 3) | (r >> 2)) | (((g << 3) | (g >> 2)) << 8) |
           (((b << 3) | (b >> 2)) << 16);
}
