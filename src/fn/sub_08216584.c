#include "global.h"

/* Metatile m into rows yi, yi+1 of column xi of the 32x32 tilemap. */

/* One cell at map tile (x, y). wrap: coordinates wrap modulo the map; check: off-map cells get fill. */
static inline void Tm216584_Cell(u16 *out, u16 *src, u8 *tiles, s32 pitch, s32 w2, s32 h2, s32 x,
                                 s32 y, s32 wrap, s32 check, u16 fill)
{
    s32 bx, by;
    if (check && (x < 0 || x >= w2 || y < 0 || y >= h2)) {
        out[((y & 31) << 5) + (x & 31)] = fill;
        out[(((y & 31) + 1) << 5) + (x & 31)] = fill;
        return;
    }
    if (wrap) {
        bx = Mod(x, w2) / 32;
        by = Mod(y, h2) / 32;
    } else {
        bx = x / 32;
        by = y / 32;
    }
    Tm216584_Put(out, tiles,
                 src[((by * pitch + bx) << 8) + (((y & 31) >> 1) << 4) + ((x & 31) >> 1)], x & 31,
                 y & 31);
}

/* Redraw the exposed strips (small move) or the whole window (large move). */

// confidence=low: streams one BG layer's 32x32 tilemap window from its metatile map as the scroll origin moves.

/* Metatile m into rows yi, yi+1 of column xi of the 32x32 tilemap. */

/* Tiles i0..i1-1 of the row at map tile y, origin ox. */

/* Tiles k = 0, 2, ..., 30 of the column at map tile x, origin oy, for k in [k0, k1). */

/* Redraw the exposed strips of a small move, or the whole window of a large one. */

// confidence=low: streams one BG layer's 32x32 tilemap window from its metatile map as the scroll origin moves.

/* Metatile m into rows yi, yi+1 of column xi of the 32x32 tilemap. */

/* Tiles i0..i1-1 of the row at map tile y, origin ox. */

/* Tiles k = 0, 2, ..., 30 of the column at map tile x, origin oy, for k in [k0, k1). */

/* Redraw the exposed strips of a small move, or the whole window of a large one. */

// confidence=low: streams one BG layer's 32x32 tilemap window from its metatile map as the scroll origin moves.

/* Metatile m into rows yi, yi+1 of column xi of the 32x32 tilemap. */

/* Tiles i0..i1-1 of the row at map tile y, origin ox. */

/* Tiles k = 0, 2, ..., 30 of the column at map tile x, origin oy, for k in [k0, k1). */

/* Redraw the exposed strips of a small move, or the whole window of a large one. */

// confidence=low: streams one BG layer's 32x32 tilemap window from its metatile map as the scroll origin moves.

/* Metatile m into rows yi, yi+1 of column xi of the 32x32 tilemap. */

/* Tiles i0..i1-1 of the row at map tile y, origin ox. */

/* Tiles k = 0, 2, ..., 30 of the column at map tile x, origin oy, for k in [k0, k1). */

/* Redraw the exposed strips of a small move, or the whole window of a large one. */

// confidence=low: streams one BG layer's 32x32 tilemap window from its metatile map as the scroll origin moves.
s32 Mod(s32, s32);
s32 sub_08249350(s32, s32);

struct Tm216584_Layer {
    s16 mode; /* 0x00: 1 wraps the origin, 2 fills off-map cells from the table */
    u8 unk02[0x0E];
    s16 tileX, tileY; /* 0x10, 0x12: origin last drawn */
    u8 unk14[4];
    s16 pitchField; /* 0x18: blocks per map row = pitchField >> 4 */
    u8 unk1A[2];
    s16 mapW, mapH;       /* 0x1C, 0x1E: map size, doubled when used */
    s16 screenX, screenY; /* 0x20, 0x22 */
    u8 *tiles;            /* 0x24: 8-byte metatiles (TL, TR, BL, BR) */
    u16 *map;             /* 0x28: default source map */
    u16 *output;          /* 0x2C: 32x32 BG tilemap */
};

/* Metatile m into rows yi, yi+1 of column xi of the 32x32 tilemap. */
static inline void Tm216584_Put(u16 *out, u8 *tiles, u16 m, s32 xi, s32 yi)
{
    u16 *cell = (u16 *)(tiles + ((u32)m << 3));
    if (xi & 1) {
        out[(yi << 5) + xi] = cell[1];
        out[((yi + 1) << 5) + xi] = cell[3];
    } else {
        out[(yi << 5) + xi] = cell[0];
        out[((yi + 1) << 5) + xi] = cell[2];
    }
}

/* Tiles i0..i1-1 of the row at map tile y, origin ox. */
static inline void Tm216584_RowCells(u16 *out, u16 *src, u8 *tiles, s32 pitch, s32 w2, s32 h2,
                                     s32 ox, s32 y, s32 i0, s32 i1, s32 wrap, s32 check, u16 fill)
{
    s32 i, x, xi, yi, by, bx, ymeta;
    if (check && (y < 0 || y >= h2)) {
        yi = y & 31;
        for (i = i0; i < i1; i++) {
            xi = (ox + i) & 31;
            out[(yi << 5) + xi] = fill;
            out[((yi + 1) << 5) + xi] = fill;
        }
        return;
    }
    by = wrap ? Mod(y, h2) / 32 : y / 32;
    by = by * pitch;
    yi = y & 31;
    ymeta = (yi >> 1) << 4;
    for (i = i0; i < i1; i++) {
        x = ox + i;
        xi = x & 31;
        if (check && (x < 0 || x >= w2)) {
            out[(yi << 5) + xi] = fill;
            out[((yi + 1) << 5) + xi] = fill;
            continue;
        }
        bx = wrap ? Mod(x, w2) / 32 : x / 32;
        Tm216584_Put(out, tiles, src[((by + bx) << 8) + ymeta + (xi >> 1)], xi, yi);
    }
}

/* Tiles k = 0, 2, ..., 30 of the column at map tile x, origin oy, for k in [k0, k1). */
static inline void Tm216584_ColCells(u16 *out, u16 *src, u8 *tiles, s32 pitch, s32 w2, s32 h2,
                                     s32 x, s32 oy, s32 k0, s32 k1, s32 wrap, s32 check, u16 fill)
{
    s32 k, y, yi, by, bx, xi, odd, ymeta;
    if (check && (x < 0 || x >= w2)) {
        xi = x & 31;
        for (k = k0; k < k1; k += 2) {
            yi = (oy + k) & 31;
            out[(yi << 5) + xi] = fill;
            out[((yi + 1) << 5) + xi] = fill;
        }
        return;
    }
    bx = wrap ? Mod(x, w2) / 32 : x / 32;
    xi = x & 31;
    odd = xi >> 1;
    for (k = k0; k < k1; k += 2) {
        y = oy + k;
        yi = y & 31;
        if (check && (y < 0 || y >= h2)) {
            out[(yi << 5) + xi] = fill;
            out[((yi + 1) << 5) + xi] = fill;
            continue;
        }
        by = wrap ? Mod(y, h2) / 32 : y / 32;
        ymeta = (yi >> 1) << 4;
        Tm216584_Put(out, tiles, src[((by * pitch + bx) << 8) + ymeta + odd], xi, yi);
    }
}

/* Redraw the exposed strips of a small move, or the whole window of a large one. */
static inline void Tm216584_Redraw(u16 *out, u16 *src, u8 *tiles, s32 pitch, s32 w2, s32 h2, s32 ox,
                                   s32 oy, s32 dx, s32 dy, s32 wrap)
{
    s32 i, k, start, limit;
    if (!((u32)(dx + 1) <= 2 && dy >= -2 && dy <= 2)) {
        for (k = 0; k < 32; k += 2)
            Tm216584_RowCells(out, src, tiles, pitch, w2, h2, ox, oy + k, 0, 32, wrap, 0, 0);
        return;
    }
    if (dy != 0) {
        start = dy > 0 ? 32 - dy : 0;
        limit = dy > 0 ? 32 : -dy;
        for (k = start; k < limit; k += 2)
            Tm216584_RowCells(out, src, tiles, pitch, w2, h2, ox, oy + k, 0, 32, wrap, 0, 0);
    }
    if (dx != 0) {
        start = dx > 0 ? 32 - dx : 0;
        limit = dx > 0 ? 32 : -dx;
        for (i = start; i < limit; i++)
            Tm216584_ColCells(out, src, tiles, pitch, w2, h2, ox + i, oy, 0, 32, wrap, 0, 0);
    }
}

#ifdef NONMATCHING
// fuzzy: 8.9%
// scope shape 868 (differences once low registers are set aside)
// confidence=low: streams one BG layer's 32x32 tilemap window from its metatile map as the scroll origin moves.
void BgGfx_LoadTilemap(u32 layer, u32 tileX, u32 tileY, u32 screenX, u32 screenY, u16 *src)
{
    struct Tm216584_Layer *L = (struct Tm216584_Layer *)(0x03004C30 + (((s32)layer * 3) << 4));
    s32 ox, oy, dx, dy, w2, h2, lx, ly, pitch, i, k;
    u16 *out;
    u8 *tiles;
    u16 fill;

    ox = (s32)tileX;
    oy = (s32)tileY;
    dx = ox - L->tileX;
    dy = oy - L->tileY;
    L->tileX = ox;
    L->tileY = oy;
    L->screenX = screenX;
    L->screenY = screenY;
    if (dx == 0 && dy == 0)
        return;

    out = L->output;
    if (src == NULL)
        src = L->map;
    tiles = L->tiles;
    w2 = L->mapW * 2;
    h2 = L->mapH * 2;
    lx = w2 - 32;
    ly = h2 - 32;
    pitch = L->pitchField >> 4;

    if (L->mode != 1) {
        if (ox < 0 || oy < 0 || ox >= lx || oy >= ly) {
            fill = 0;
            if (L->mode == 2)
                fill = *(u16 *)(tiles + ((u32)src[0] << 1));
            if (!((u32)(dx + 1) <= 2 && dy >= -2 && dy <= 2)) {
                for (k = 0; k < 32; k += 2)
                    Tm216584_RowCells(out, src, tiles, pitch, w2, h2, ox, oy + k, 0, 32, 0, 1,
                                      fill);
            } else {
                if (dx != 0)
                    Tm216584_ColCells(out, src, tiles, pitch, w2, h2, ox + (dx > 0 ? 31 : 0), oy, 0,
                                      32, 0, 1, fill);
                if (dy != 0)
                    Tm216584_RowCells(out, src, tiles, pitch, w2, h2, ox, oy + (dy > 0 ? 30 : 0), 0,
                                      32, 0, 1, fill);
            }
        } else {
            Tm216584_Redraw(out, src, tiles, pitch, w2, h2, ox, oy, dx, dy, 0);
        }
    } else {
        ox = sub_08249350(ox, w2);
        if (ox < 0)
            ox += w2;
        oy = sub_08249350(oy, h2);
        if (oy < 0)
            oy += h2;
        Tm216584_Redraw(out, src, tiles, pitch, w2, h2, ox, oy, dx, dy, 1);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", BgGfx_LoadTilemap);
#endif
