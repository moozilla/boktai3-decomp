#include "global.h"

struct ObjGfxSlot {
    u16 size;
    u16 offset;
    const void *src;
};
extern struct ObjGfxSlot gUnk_03001148[];
extern u16 gUnk_030042D8;
extern s16 gUnk_030042E8;
extern u16 gUnk_030042F0;
extern s16 gUnk_030042F4;

s32 sub_082152E4(const void *src, s32 size)
{
    struct ObjGfxSlot *slot;
    struct ObjGfxSlot *entry;
    s32 i;

    if (gUnk_030042F0 > 0x7F)
        return gUnk_030042F4;
    if (gUnk_030042E8 + size > 0x3FF)
        return gUnk_030042F4;
    for (slot = gUnk_03001148, i = 0; i < gUnk_030042F0; i++, slot++) {
        if (slot->src == src)
            return slot->offset;
    }
    entry = &gUnk_03001148[gUnk_030042F0];
    entry->size = size;
    entry->offset = gUnk_030042E8;
    entry->src = src;
    gUnk_030042E8 += size;
    gUnk_030042D8 += size;
    gUnk_030042F0++;
    return entry->offset;
}
