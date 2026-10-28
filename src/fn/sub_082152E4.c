#include "global.h"
#include "render.h"


s32 sub_082152E4(const void *src, s32 size)
{
    struct ObjGfxSlot *slot;
    struct ObjGfxSlot *entry;
    s32 i;

    if (gQueuedObjSlotCount > 0x7F)
        return ((s16)gStaticTileCount);
    if (gObjTileCursor + size > 0x3FF)
        return ((s16)gStaticTileCount);
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
