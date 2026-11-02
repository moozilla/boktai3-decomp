/* Tentative family and field names, based on static accesses. */
#include "sprite_trail.h"
extern void *gUnk_08613E08[];
#define gSpriteGroupSlotCallbacks gUnk_08613E08
void sub_08249244(void *, void *, s32, void *);
u32 SpriteGroup_UpdateSlots(struct SpriteTrailPool *p)
{
    struct SpriteTrail *slot = p->entries;
    s32 i = 0;
    do {
        if (p->activeMask & (1 << i))
            sub_08249244(p, slot, i, gUnk_08613E08[slot->state]);
        i++;
        slot++;
    } while (i <= 3);
    return 0;
}
