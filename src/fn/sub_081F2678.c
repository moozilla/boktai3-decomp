/* Tentative family and field names, based on static accesses. */
#include "global.h"
struct SpriteGroupSlot {
    u8 resetRequested, unknown1[0x83];
    s32 age;
    u8 unknown88[0x40];
    u32 flags;
};
void SpriteGroup_RemoveSlot(void *, struct SpriteGroupSlot *, s32);
void SpriteTrail_UpdatePositions(struct SpriteGroupSlot *);
void SpriteGroup_UpdateExpiringSlot(void *group, struct SpriteGroupSlot *slot, s32 index)
{
    if (slot->resetRequested) {
        slot->resetRequested = 0;
        {
            u32 *flags = &slot->flags;
            u32 one = 1;
            *flags |= one;
        }
    }
    SpriteTrail_UpdatePositions(slot);
    if (slot->age > 0)
        SpriteGroup_RemoveSlot(group, slot, index);
    else
        slot->age++;
}
