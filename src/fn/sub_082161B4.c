#include "global.h"
#include "render.h"
struct BgResource {
    u8 signature[2];
    u16 unk02;
    u16 unk04;
    u16 count;
    u16 unk08;
    u16 size;
    u32 maps;
    u32 tiles;
    u32 extra;
};
struct BgMapEntry {
    u16 width, height;
    u32 unk04;
    u32 data;
};
struct BgLayer {
    u16 resource;
    u16 bg;
    u8 pad4[0xC];
    u16 scaleX, scaleY;
    s16 x, y;
    u16 width, height;
    u16 width2, height2;
    u16 field20, field22;
    u32 extra;
    u32 data;
    void *map;
};
extern struct BgLayer gUnk_03004C30[];
void sub_08215AC4(s32 bg, s32 mode);
void Bg_SetTileUpload(s32 bg, u32 tiles, s32 offset, u16 size);
void Text_ClearGlyphCache(void);
void *Video_GetBackgroundMap(u32);
static inline void enableBg(s32 bits)
{
    gDispCntShadow |= bits;
}
/* Sets up the background layers of a map resource for one of eight layer
   modes: picks the BGs (sub_08215AC4) and DISPCNT bits, loads the tiles
   (Bg_SetTileUpload) and fills each layer's gUnk_03004C30 entry from the map
   entries, picked through maps[] for the first count layers. */
void Bg_SetupLayers(s32 mode, u16 resourceId, struct BgResource *res, u16 x, u16 y, s32 count,
                    s32 *maps)
{
    s32 bgs[4];
    struct BgResource hdr;
    struct BgMapEntry entry;
    s32 n, i;
    s32 offset;
    hdr = *res;
    hdr.maps += (u32)res;
    hdr.tiles += (u32)res;
    hdr.extra += (u32)res;
    switch (mode) {
        case 0:
            bgs[0] = 0;
            n = 1;
            offset = 0x8000;
            enableBg(0x100);
            sub_08215AC4(0, 2);
            break;
        case 1:
            sub_08215AC4(bgs[0] = 2, 0);
            if (count == 2) {
                enableBg(0x800);
                sub_08215AC4(bgs[1] = 3, 0);
                n = 2;
            } else
                n = 1;
            offset = hdr.size << 5;
            enableBg(0x400);
            break;
        case 2:
            sub_08215AC4(bgs[0] = 3, 0);
            n = 1;
            offset = hdr.size << 5;
            enableBg(0x800);
            break;
        case 3:
            sub_08215AC4(bgs[0] = 1, 0);
            n = 1;
            offset = hdr.size << 5;
            enableBg(0x200);
            break;
        case 4:
            sub_08215AC4(bgs[0] = 1, 0);
            if (count == 2) {
                enableBg(0x800);
                sub_08215AC4(bgs[1] = 3, 0);
                n = 2;
            } else
                n = 1;
            offset = hdr.size << 5;
            enableBg(0x200);
            break;
        case 5:
            sub_08215AC4(bgs[0] = 2, 0);
            if (count == 2) {
                enableBg(0x200);
                sub_08215AC4(bgs[1] = 1, 0);
                n = 2;
            } else
                n = 1;
            offset = hdr.size << 5;
            enableBg(0x400);
            break;
        case 6:
            sub_08215AC4(bgs[0] = 1, 2);
            n = 1;
            offset = 0x8000;
            enableBg(0x200);
            break;
        case 7:
            n = 3;
            bgs[0] = 2;
            bgs[1] = 3;
            bgs[2] = 1;
            sub_08215AC4(2, 0);
            sub_08215AC4(3, 0);
            sub_08215AC4(1, 0);
            {
                enableBg(0x200);
                enableBg(0x400);
                enableBg(0x800);
            }
            offset = hdr.size << 5;
            break;
        default:
            sub_08215AC4(bgs[0] = 2, 0);
            n = 0;
            offset = 0;
            break;
    }
    if (hdr.count && hdr.tiles != (u32)res)
        Bg_SetTileUpload(bgs[0], hdr.tiles, offset, hdr.count);
    for (i = 0; i < n; i++) {
        s32 bg = bgs[i];
        struct BgLayer *layer = &gUnk_03004C30[bg];
        struct BgMapEntry *entries = (struct BgMapEntry *)hdr.maps;
        if (bg == 0)
            Text_ClearGlyphCache();
        if (i < count && maps)
            entry = entries[maps[i]];
        else
            entry = entries[i];
        entry.data += (u32)res;
        layer->resource = resourceId;
        layer->bg = bg;
        layer->field20 = 0;
        layer->field22 = 0;
        layer->scaleX = 0x1000;
        layer->scaleY = 0x1000;
        {
            s16 lx = (s16)x >> 4, ly = (s16)y >> 4;
            layer->x = lx;
            layer->y = ly;
        }
        layer->extra = hdr.extra;
        layer->data = entry.data;
        {
            u16 w, h;
            layer->width = w = entry.width;
            layer->height = h = entry.height;
            layer->width2 = w;
            layer->height2 = h;
        }
        layer->map = Video_GetBackgroundMap(bg);
    }
}
