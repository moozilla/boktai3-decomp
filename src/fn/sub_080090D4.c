#include "global.h"
struct Resource {
    u32 data[8];
};
struct Slot {
    u8 data[236];
};
struct Manager {
    u8 pad[24];
    u32 count;
    struct Slot *slots;
    struct Resource resources[3];
};
extern struct Manager *gUnk_02000034;
extern u16 gUnk_086049C8[];
struct Resource *sub_0821A520(u32, u32);
void sub_082196C4(void *, void *);
u32 sub_0821ABA8(u32, u32);
void *sub_08219C40(u32);
void sub_08219DD8(void *, u32);
s32 sub_08008AF4(struct Manager *, struct Slot *);
#define Breakable_InitManager sub_080090D4
s32 Breakable_InitManager(struct Manager *manager)
{
    s32 i;
    u32 bytes;
    struct Slot *slot;
    gUnk_02000034 = manager;
    for (i = 0; i < 3; i++) {
        struct Resource *resource = sub_0821A520(0xcb05, gUnk_086049C8[i]);
        if (!resource)
            return -1;
        manager->resources[i] = *resource;
        sub_082196C4(&manager->resources[i], resource);
    }
    manager->count = sub_0821ABA8(110, 8);
    bytes = manager->count * sizeof(struct Slot);
    manager->slots = sub_08219C40(bytes);
    sub_08219DD8(manager->slots, bytes);
    slot = manager->slots;
    for (i = 0; (u32)i < manager->count; i++, slot++)
        sub_08008AF4(manager, slot);
}
