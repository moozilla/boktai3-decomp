#include "global.h"

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
void sub_08218CB0(void);
/* Points background layer bg at map entry index of a map resource (data,
   size, scale 1.0); see Bg_SetupLayers. */
void Bg_SetLayerMap(s32 bg, struct BgResource *res, s32 index)
{
    struct BgResource hdr;
    struct BgMapEntry entry;
    struct BgLayer *layer;
    if (bg == 0)
        sub_08218CB0();
    hdr = *res;
    hdr.maps += (u32)res;
    hdr.tiles += (u32)res;
    hdr.extra += (u32)res;
    layer = &gUnk_03004C30[bg];
    entry = ((struct BgMapEntry *)hdr.maps)[index];
    entry.data += (u32)res;
    layer->scaleX = 0x1000;
    layer->scaleY = 0x1000;
    layer->data = entry.data;
    {
        u16 w, h;
        layer->width = w = entry.width;
        layer->height = h = entry.height;
        layer->width2 = w;
        layer->height2 = h;
    }
}
