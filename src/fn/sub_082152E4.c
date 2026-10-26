#include "global.h"

struct ObjGfxSlot {
    u16 size;
    u16 offset;
    const void *src;
};
extern struct ObjGfxSlot gUnk_03001148[];
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
    for (slot = gUnk_03001148, i = 0; i < gQueuedObjSlotCount; i++, slot++) {
        if (slot->src == src)
            return slot->offset;
    }
    entry = &gUnk_03001148[gQueuedObjSlotCount];
    entry->size = size;
    entry->offset = gObjTileCursor;
    entry->src = src;
    gObjTileCursor += size;
    gDynamicTileSum += size;
    gQueuedObjSlotCount++;
    return entry->offset;
}
