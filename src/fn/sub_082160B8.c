#include "global.h"
#include "render.h"
struct BgHeader {
    u8 p0[0xC];
    u32 u0C;
    s16 s10;
    s16 s12;
    u32 u14;
    u8 p18[4];
    u32 u1C;
    u8 p20[4];
    u32 u24;
    u8 p28[0x10];
    u32 u38;
    u32 u3C;
};
struct BgLayerState {
    u16 mode;
    u16 layer;
    u8 p4[0xC];
    u16 h10, h12, h14, h16, h18, h1A;
    u8 p1C[4];
    u16 h20, h22;
    u32 w24, w28, w2C;
};
extern struct BgLayerState gUnk_03004C30[];
void *memcpy(void *, const void *, u32);
void Bg_SetTileUpload(s32, u32, s32, s32);
void Bg_SetPriority(s32, s32);
void Text_ClearGlyphCache(void);
// MATCHING_CONCESSION: register-pinning
void Bg_SetupLayer(s32 layer, u16 mode, u32 base, s32 unused, s32 x, s32 y, s32 priority, u32 map)
{
    struct BgHeader hdr;
    u16 bits;
    s32 px, py;
    struct BgLayerState *st = &gUnk_03004C30[layer];
    st->mode = mode;
    st->layer = layer;
    st->h20 = 0;
    st->h22 = 0;
    st->h10 = 0x1000;
    st->h12 = 0x1000;
    px = (s16)x >> 4;
    py = (s16)y >> 4;
    memcpy(&hdr, (void *)base, sizeof(hdr));
    hdr.u1C += base;
    hdr.u24 += base;
    hdr.u0C += base;
    hdr.u14 += base;
    base = 0;
    switch (layer) {
        case 0:
            base = 0x8000;
            bits = 0x100;
            break;
        case 1:
            base = hdr.s12 << 5;
            bits = 0x200;
            break;
        case 2:
            do {
                base = hdr.s12 << 5;
                bits = 0x400;
            } while (0);
            break;
        case 3:
            base = hdr.s12 << 5;
            bits = 0x800;
            break;
        default:
            goto skip;
    }
    {
        register u16 *g asm("r2") = &gDispCntShadow;
        register u16 cur asm("r1");
        cur = *g;
        *g = cur | bits;
    }
skip:
    Bg_SetTileUpload(layer, hdr.u14, base, hdr.s10);
    Bg_SetPriority(layer, priority);
    st->h14 = px;
    st->h16 = py;
    st->h18 = hdr.u38;
    st->h1A = hdr.u3C;
    st->w24 = hdr.u1C;
    st->w28 = hdr.u24;
    st->w2C = map;
    if (layer == 0)
        Text_ClearGlyphCache();
}
