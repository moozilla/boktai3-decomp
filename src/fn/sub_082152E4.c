#include "global.h"
#include "render.h"

extern u16 gDynamicTileSum;
extern s16 gObjTileCursor;
extern u16 gQueuedObjSlotCount;
extern s16 gStaticTileCount;

s32 sub_082152E4(const void *src, s32 size)
{
    struct ObjGfxSlot *slot;
    struct ObjGfxSlot *entry;
    s32 i;

    if (gQueuedObjSlotCount > 0x7F)
        return gStaticTileCount;
    if (gObjTileCursor + size > 0x3FF)
        return gStaticTileCount;
    for (slot = gObjGfxSlots, i = 0; i < gQueuedObjSlotCount; i++, slot++) {
        if (slot->src == src)
            return slot->offset;
    }
    entry = &gObjGfxSlots[gQueuedObjSlotCount];
    entry->size = size;
    entry->offset = gObjTileCursor;
    entry->src = src;
    gObjTileCursor += size;
    gDynamicTileSum += size;
    gQueuedObjSlotCount++;
    return entry->offset;
}
