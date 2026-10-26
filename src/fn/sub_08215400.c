#include "global.h"
#include "render.h"
struct Entry {
    u32 src;
    u16 size, count;
};
struct Dma {
    vu32 src, dest, control;
};
extern struct Entry gUnk_02039000[];
void ObjGfx_LoadTiles(u32 start)
{
    u32 i;
    struct Entry *entry = gUnk_02039000;
    u32 dst = 0x06010000 + (start << 5);
    u32 src;

    i = 0;
    /* Save the queue length before reusing src for the tile source address. */
    src = gArmTileUploadCount;
    if (i < src) {
        u32 entryCount = src;
        do {
            struct Entry *e = entry;
            s32 j;
            src = e->src;
            for (j = 0; j < e->count; j++) {
                struct Dma *dma = (struct Dma *)0x040000d4;
                dma->src = src;
                dma->dest = dst;
                dma->control = (e->size >> 2) | 0x84000000;
                (void)dma->control;
                src += 0x200;
                dst += e->size;
            }

            i++;
            entry++;
        } while (i < entryCount);
    }
    gArmTileUploadCount = 0;
}
