#include "global.h"
#include "render.h"
#include "gba/io_reg.h"

struct BgHeader {
    u8 p0[0xC];
    u32 u0C;
    s16 s10;
    s16 tileIndex;
    u32 u14;
    u8 p18[4];
    u32 u1C;
    u8 p20[4];
    u32 u24;
    u8 p28[0x10];
    u32 u38;
    u32 u3C;
};

void *memcpy(void *, const void *, u32);
void Bg_SetTileUpload(s32, u32, s32, s32);
void Bg_SetPriority(s32, s32);
void Text_ClearGlyphCache(void);

void Bg_SetupLayer(s32 layer, u16 mode, const struct BgHeader *resource, s32 unused, s32 x,
                   s32 y, s32 priority, u16 *map)
{
    struct BgHeader hdr;
    u32 tileOffset;
    s32 px, py;
    struct BackgroundLayer *st = &gBgRecords[layer];
    st->flags = mode;
    st->layer = layer;
    st->x = 0;
    st->y = 0;
    st->scaleX = 0x1000;
    st->scaleY = 0x1000;
    px = (s16)x >> 4;
    py = (s16)y >> 4;
    memcpy(&hdr, resource, sizeof(hdr));
    hdr.u1C += (u32)resource;
    hdr.u24 += (u32)resource;
    hdr.u0C += (u32)resource;
    hdr.u14 += (u32)resource;
    tileOffset = 0;
    switch (layer) {
        case 0:
            tileOffset = 0x8000;
            Bg_EnableDisplay(DISPCNT_BG0_ON);
            break;
        case 1:
            tileOffset = hdr.tileIndex * 32;
            Bg_EnableDisplay(DISPCNT_BG1_ON);
            break;
        case 2:
            tileOffset = hdr.tileIndex * 32;
            Bg_EnableDisplay(DISPCNT_BG2_ON);
            break;
        case 3:
            tileOffset = hdr.tileIndex * 32;
            Bg_EnableDisplay(DISPCNT_BG3_ON);
            break;
    }
    Bg_SetTileUpload(layer, hdr.u14, tileOffset, hdr.s10);
    Bg_SetPriority(layer, priority);
    st->originX = px;
    st->originY = py;
    st->width = hdr.u38;
    st->height = hdr.u3C;
    st->extraData = (u16 *)hdr.u1C;
    st->mapData = (u16 *)hdr.u24;
    st->map = map;
    if (layer == 0)
        Text_ClearGlyphCache();
}
