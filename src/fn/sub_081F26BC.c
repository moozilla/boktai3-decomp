/* Tentative family and field names, based on static accesses. */
#include "global.h"
struct SpriteGroupSlot {
    u8 resetRequested, state, unknown2[0x256];
};
struct SpriteGroup {
    u8 unknown0[0x18];
    u32 activeSlots;
    u8 unknown1C[4];
    struct SpriteGroupSlot slots[4];
};
extern void *gUnk_08613E08[];
#define gSpriteGroupSlotCallbacks gUnk_08613E08
void sub_08249244(void *, void *, s32, void *);
u32 SpriteGroup_UpdateSlots(struct SpriteGroup *p)
{
    struct SpriteGroupSlot *slot = p->slots;
    s32 i = 0;
    do {
        if (p->activeSlots & (1 << i))
            sub_08249244(p, slot, i, gUnk_08613E08[slot->state]);
        i++;
        slot++;
    } while (i <= 3);
    return 0;
}
