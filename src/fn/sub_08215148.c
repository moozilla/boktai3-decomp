#define SpriteTiles_LoadResource sub_08215148
#define gSpriteTileResource      gUnk_030042EC
#define gSpriteTileCount         gUnk_030042F4
#include "global.h"
struct SpriteTileResource {
    u32 unknown0;
    u16 tileCount;
    u16 unknown6;
    u32 dataOffset;
};
struct SpriteTileDma {
    vu32 source, destination, control;
};
extern struct SpriteTileResource *gUnk_030042EC;
extern u16 gUnk_030042F4;
void sub_08215148(struct SpriteTileResource *resource)
{
    gUnk_030042EC = resource;
    gUnk_030042F4 = resource->tileCount;
    {
        struct SpriteTileDma *dma = (struct SpriteTileDma *)0x040000d4;
        dma->source = (u32)resource + resource->dataOffset;
        dma->destination = 0x06010000;
        dma->control = ((resource->tileCount << 5) / 4) | 0x84000000;
        dma->control;
    }
}
