#include "global.h"
struct Drawable {
    u8 pad[8];
    u32 flags;
    u8 rest[84];
};
struct Collider {
    u8 pad[6];
    u16 flags;
    u8 rest[76];
};
struct Slot {
    u8 active, motion, state, unused, entered, flash;
    u8 pad6[6];
    s32 counter;
    u8 pad10[4];
    s32 region;
    u8 pad18[28];
    struct Drawable drawable;
    struct Collider collider;
    void *resource;
};
struct Manager {
    u8 pad[24];
    u32 count;
    struct Slot *slots;
};
struct World {
    u8 pad[0x5a0];
    s32 region;
};
extern struct World *gUnk_02000710;
extern void (*gUnk_086049E0[])(struct Manager *, struct Slot *);
void sub_08249240(struct Manager *, struct Slot *, void *);
void sub_08008F00(struct Slot *);
void sub_08008F64(struct Slot *);
void sub_08220F70(void *, void *);
void sub_0821FF24(void *, void *, u32);
static inline u8 sameRegion(s32 region)
{
    if (gUnk_02000710->region == region)
        return 1;
    return 0;
}
s32 Breakable_UpdateAll(struct Manager *manager)
{
    struct Slot *slot = manager->slots;
    u32 i;
    for (i = 0; i < manager->count; i++, slot++) {
        if (slot->active) {
            if (slot->region >= 0 && !sameRegion(slot->region)) {
                slot->drawable.flags |= 1;
                slot->collider.flags |= 4;
            } else {
                slot->drawable.flags &= ~1;
                slot->collider.flags &= ~4;
                sub_08249240(manager, slot, gUnk_086049E0[slot->state]);
                if (slot->motion == 1)
                    sub_08008F00(slot);
                else if (slot->motion == 2)
                    sub_08008F64(slot);
                sub_08220F70(&slot->drawable, slot->resource);
                sub_0821FF24(&slot->collider, (u8 *)slot + 0x54, 0);
                if (slot->flash) {
                    slot->flash--;
                    if (!slot->flash)
                        slot->collider.flags &= ~4;
                    else
                        slot->collider.flags |= 4;
                }
                slot->counter++;
            }
        }
    }
}
