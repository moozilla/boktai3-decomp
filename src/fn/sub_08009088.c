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
extern struct Manager *gUnk_02000034;
void sub_0821FE6C(void *);
void sub_082195E0(void *);
void sub_08219D38(void *);
s32 Breakable_DestroyManager(struct Manager *manager)
{
    struct Slot *slot = manager->slots;
    u32 i;
    for (i = 0; i < manager->count; i++, slot++)
        if (slot->active) {
            sub_0821FE6C(&slot->collider);
            sub_082195E0(&slot->drawable);
        }
    if (manager->slots) {
        sub_08219D38(manager->slots);
        manager->slots = 0;
    }
    gUnk_02000034 = 0;
}
