#define gSpriteTileResource gUnk_030042EC
#define gSpriteTileCount    gStaticTileCount
#include "global.h"
#include "render.h"
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
void SpriteTiles_LoadResource(struct SpriteTileResource *resource)
{
    gUnk_030042EC = resource;
    gStaticTileCount = resource->tileCount;
    {
        struct SpriteTileDma *dma = (struct SpriteTileDma *)0x040000d4;
        dma->source = (u32)resource + resource->dataOffset;
        dma->destination = 0x06010000;
        dma->control = ((resource->tileCount << 5) / 4) | 0x84000000;
        dma->control;
    }
}
