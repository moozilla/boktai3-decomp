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
void Text_ClearGlyphCache(void);
/* Points background layer bg at map entry index of a map resource (data,
   size, scale 1.0); see Bg_SetupLayers. */
void Bg_SetLayerMap(s32 bg, struct BgResource *res, s32 index)
{
    struct BgResource hdr;
    struct BgMapEntry entry;
    struct BackgroundLayer *layer;
    if (bg == 0)
        Text_ClearGlyphCache();
    hdr = *res;
    hdr.maps += (u32)res;
    hdr.tiles += (u32)res;
    hdr.extra += (u32)res;
    layer = &gBgRecords[bg];
    entry = ((struct BgMapEntry *)hdr.maps)[index];
    entry.data += (u32)res;
    layer->scaleX = 0x1000;
    layer->scaleY = 0x1000;
    layer->mapData = (u16 *)entry.data;
    {
        u16 w, h;
        layer->width = w = entry.width;
        layer->height = h = entry.height;
        layer->halfWidth = w;
        layer->halfHeight = h;
    }
}
