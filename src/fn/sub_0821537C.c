#include "global.h"
#include "gba/gba.h"
#include "render.h"

// Upload queued tile slots to OBJ VRAM using DMA3.
void ObjGfx_LoadTiles(u32 start);

void sub_0821537C(void)
{
    u32 dest = OBJ_VRAM0 + gStaticTileCount * 32;
    struct ObjGfxSlot *slot = gObjGfxSlots;
    s32 i;
    for (i = 0; i < gQueuedObjSlotCount; i++) {
        s32 bytes = slot->size << 5;
        DmaCopy32(3, slot->src, dest, bytes);
        dest += bytes;
        slot++;
    }
    gQueuedObjSlotCount = 0;
    ObjGfx_LoadTiles(gStaticTileCount + gDynamicTileSum);
}
