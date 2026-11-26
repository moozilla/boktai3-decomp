#include "global.h"

struct SpriteSlot {
    u8 pad00[0x86];
    s16 updateKind;
    u8 pad88[8];
};
struct SpriteSlots {
    u8 task[0x18];
    struct SpriteSlot slots[8];
    u32 activeMask;
};
extern void *gUnk_086064AC[];
u32 FloorSwitch_UpdateRegionVisibility(struct SpriteSlot *);
s32 Callback_InvokeActor(void *, void *);

static inline u8 SpriteSlots_IsActive(u32 *mask, s32 i)
{
    if (*mask & (1 << i))
        return TRUE;
    return FALSE;
}

u32 SpriteSlots_UpdateActive(struct SpriteSlots *self)
{
    struct SpriteSlot *slots = self->slots;
    struct SpriteSlot *slot;
    s32 i;

    if (!self->activeMask)
        return 0;
    for (i = 0; i < 8; i++) {
        slot = &self->slots[i];
        if (SpriteSlots_IsActive((u32 *)(slots + 8), i)) {
            if ((u8)FloorSwitch_UpdateRegionVisibility(slot))
                Callback_InvokeActor(slot, gUnk_086064AC[slot->updateKind]);
        }
    }
    return 0;
}
