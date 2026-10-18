/* Tentative family and field names, based on static accesses. */
#define SpriteGroup_UpdateExpiringSlot sub_081F2678
#define SpriteGroup_RemoveSlot         sub_081F23A0
#define SpriteGroup_UpdateSlot         sub_081F2404
#include "global.h"
struct SpriteGroupSlot {
    u8 resetRequested, unknown1[0x83];
    s32 age;
    u8 unknown88[0x40];
    u32 flags;
};
void sub_081F23A0(void *, struct SpriteGroupSlot *, s32);
void sub_081F2404(struct SpriteGroupSlot *);
void sub_081F2678(void *group, struct SpriteGroupSlot *slot, s32 index)
{
    if (slot->resetRequested) {
        slot->resetRequested = 0;
        {
            u32 *flags = &slot->flags;
            u32 one = 1;
            *flags |= one;
        }
    }
    sub_081F2404(slot);
    if (slot->age > 0)
        sub_081F23A0(group, slot, index);
    else
        slot->age++;
}
