#define gSpriteTileCount    gStaticTileCount
#include "global.h"
#include "render.h"
struct SpriteTileDma {
    vu32 source, destination, control;
};
void SpriteTiles_LoadResource(struct SpriteTileResource *resource)
{
    gSpriteTileResource = resource;
    gStaticTileCount = resource->tileCount;
    {
        struct SpriteTileDma *dma = (struct SpriteTileDma *)0x040000d4;
        dma->source = (u32)resource + resource->dataOffset;
        dma->destination = 0x06010000;
        dma->control = ((resource->tileCount << 5) / 4) | 0x84000000;
        dma->control;
    }
}
