#include "global.h"
#include "render.h"
#include "obj_tile_upload.h"
struct Dma {
    vu32 src, dest, control;
};
void ObjGfx_LoadTiles(u32 start)
{
    u32 i;
    struct ObjTileUploadRequest *entry = gObjTileUploadRequests;
    u32 dst = 0x06010000 + (start << 5);
    u32 src;

    i = 0;
    /* Save the queue length before reusing src for the tile source address. */
    src = gArmTileUploadCount;
    if (i < src) {
        u32 entryCount = src;
        do {
            struct ObjTileUploadRequest *e = entry;
            s32 j;
            src = e->src;
            for (j = 0; j < e->transfer.rows.count; j++) {
                struct Dma *dma = (struct Dma *)0x040000d4;
                dma->src = src;
                dma->dest = dst;
                dma->control = (e->transfer.rows.size >> 2) | 0x84000000;
                (void)dma->control;
                src += 0x200;
                dst += e->transfer.rows.size;
            }

            i++;
            entry++;
        } while (i < entryCount);
    }
    gArmTileUploadCount = 0;
}
